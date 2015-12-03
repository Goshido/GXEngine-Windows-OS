//version 1.8

#include <GXEngine/GXShaderStorageExt.h>
#include <GXEngine/GXShaderUtils.h>
#include <GXCommon/GXStrings.h>


class GXShaderUnit
{
	private:
		GXShaderUnit* 	next;
		GXShaderUnit* 	prev;

		GXInt			refs;

		GXWChar*		vs;
		GXWChar*		gs;
		GXWChar*		fs;

		GLuint			program;
		GXBool			isSamplersTuned;

	public:
		GXShaderUnit ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs );

		GXVoid AddRef ();
		GXVoid Release ();

		GXShaderUnit* Find ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs );
		GXShaderUnit* Find ( GLuint program );

		GXBool IsSamplersTuned ();
		GXVoid SetSamplersTuned ();

		GLuint GetProgram ();
		GXUInt CheckMemoryLeak ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS );

	private:
		~GXShaderUnit ();
};

GXShaderUnit* gx_strg_ShadersExt = 0;

GXShaderUnit::GXShaderUnit ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs )
{
	if ( !gx_strg_ShadersExt )
		next = prev = 0;
	else
	{
		next = gx_strg_ShadersExt;
		prev = 0;
		gx_strg_ShadersExt->prev = this;
	}

	gx_strg_ShadersExt = this;

	refs = 1;

	GXWcsclone ( &this->vs, vs );

	if ( gs )
		GXWcsclone ( &this->gs, gs );
	else
		this->gs = 0;

	GXWcsclone ( &this->fs, fs );

	program = GXCreateShaderProgram ( vs, gs, fs );
	isSamplersTuned = GX_FALSE;
}

GXVoid GXShaderUnit::AddRef ()
{
	refs++;
}

GXVoid GXShaderUnit::Release ()
{
	refs--;

	if ( refs < 1 )
		delete this;
}

GXShaderUnit* GXShaderUnit::Find ( const GXWChar* vs, const GXWChar* gs, const GXWChar* fs )
{
	GXShaderUnit* p = this;

	while ( p )
	{
		if ( ( GXWcscmp ( p->vs, vs ) == 0 ) && ( GXWcscmp ( p->fs, fs ) == 0 ) )
		{
			if ( !gs || GXWcscmp ( p->gs, gs ) == 0 )
				return p;
		}

		p = p->next;
	}

	return 0;
}

GXShaderUnit* GXShaderUnit::Find ( GLuint program )
{
	GXShaderUnit* p = this;

	while ( p )
	{
		if ( p->program == program )
			return p;

		p = p->next;
	}

	return 0;
}

GXBool GXShaderUnit::IsSamplersTuned ()
{
	return isSamplersTuned;
}

GXVoid GXShaderUnit::SetSamplersTuned ()
{
	isSamplersTuned = GX_TRUE;
}

GLuint GXShaderUnit::GetProgram ()
{
	return program;
}

GXUInt GXShaderUnit::CheckMemoryLeak ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS )
{
	GXUInt objects = 0;
	*lastVS = *lastFS = 0;

	for ( GXShaderUnit* p = gx_strg_ShadersExt; p; p = p->next )
	{
		*lastVS = p->vs;
		*lastGS = p->gs;
		*lastFS = p->fs;
		objects++;
	}

	return objects;
}

GXShaderUnit::~GXShaderUnit ()
{
	if ( next ) next->prev = prev;

	if ( prev )
		prev->next = next;
	else
		gx_strg_ShadersExt = next;

	free ( vs );
	free ( fs );

	glUseProgram ( 0 );
	glDeleteProgram ( program );
}

//------------------------------------------------------------------------------------------------

GXVoid GXCALL GXGetShaderProgramExt ( GXShaderInfo &shader_info, const GXWChar* vs, const GXWChar* gs, const GXWChar* fs )
{
	GXWcsclone ( &shader_info.vShaderFileName, vs );

	if ( gs )
		GXWcsclone ( &shader_info.gShaderFileName, gs );
	else
		shader_info.gShaderFileName = 0;

	GXWcsclone ( &shader_info.fShaderFileName, fs );

	GXShaderUnit* shader;

	if ( !gx_strg_ShadersExt )
		shader = new GXShaderUnit ( vs, gs, fs );
	else
	{
		shader = gx_strg_ShadersExt->Find ( vs, gs, fs );

		if ( shader )
			shader->AddRef ();
		else
			shader = new GXShaderUnit ( vs, gs, fs );
	}

	shader_info.isSamplersTuned = shader->IsSamplersTuned ();
	shader_info.uiId = shader->GetProgram ();
}

GXVoid GXCALL GXGetShaderProgramExt ( GXShaderInfo &shader_info )
{
	GXShaderUnit* shader;

	if ( !gx_strg_ShadersExt )
		shader = new GXShaderUnit ( shader_info.vShaderFileName, shader_info.gShaderFileName, shader_info.fShaderFileName );
	else
	{
		shader = gx_strg_ShadersExt->Find ( shader_info.vShaderFileName, shader_info.gShaderFileName, shader_info.fShaderFileName );

		if ( shader )
			shader->AddRef ();
		else
			shader = new GXShaderUnit ( shader_info.vShaderFileName, shader_info.gShaderFileName, shader_info.fShaderFileName );
	}

	shader_info.isSamplersTuned = shader->IsSamplersTuned ();
	shader_info.uiId = shader->GetProgram ();
}

GXVoid GXCALL GXRemoveShaderProgramExt ( const GXShaderInfo &shader_info )
{
	if ( !gx_strg_ShadersExt ) return;

	GXShaderUnit* shader = gx_strg_ShadersExt->Find ( shader_info.uiId );

	if ( !shader ) return;

	shader->Release ();
}

GXVoid GXCALL GXTuneShaderSamplers ( GXShaderInfo &shader_info, const GLuint* samplerIndexes, const GLchar** samplerNames, GXUByte numSamplers )
{
	if ( !gx_strg_ShadersExt ) return;

	GXShaderUnit* shader = gx_strg_ShadersExt->Find ( shader_info.uiId );

	if ( !shader ) return;

	glUseProgram ( shader_info.uiId );

	for ( GXUByte i = 0; i < numSamplers; i++ )
		glUniform1i ( GXGetUniformLocation ( shader_info.uiId, samplerNames[ i ] ), samplerIndexes[ i ] );

	glUseProgram ( 0 );

	shader->SetSamplersTuned ();
	shader_info.isSamplersTuned = GX_TRUE;
}

GXUInt GXCALL GXGetTotalShaderStorageObjectsExt ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS )
{
	if ( !gx_strg_ShadersExt )
	{
		*lastVS = *lastGS = *lastFS = 0;
		return 0;
	}

	return gx_strg_ShadersExt->CheckMemoryLeak ( lastVS, lastGS, lastFS );
}
