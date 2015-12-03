//version 1.4

#include <GXEngine/GXShaderStorage.h>
#include <GXEngine/GXShaderUtils.h>
#include <GXCommon/GXStrings.h>


GXShaderUnit* gx_strg_Shaders = 0;


GXVoid GXCALL AddShaderUnit ( GXShaderUnit* unit )
{
	if ( !gx_strg_Shaders )
	{
		gx_strg_Shaders = unit;
		unit->next = unit->prev = 0;
	}
	else
	{
		unit->next = gx_strg_Shaders;
		unit->prev = 0;
		gx_strg_Shaders->prev = unit;
		gx_strg_Shaders = unit;
	}
}

GXShaderUnit* GXCALL FindShaderUnitByShaderName ( const GXWChar* shaderName )
{
	GXShaderUnit* p = gx_strg_Shaders;
	while ( p )
	{
		if ( GXWcscmp ( p->shaderFullName, shaderName ) == 0 ) return p;
		p = p->next;
	}
	return 0;
}

GXShaderUnit* GXCALL FindShaderUnitByShaderProgram ( GLuint shaderProgram )
{
	GXShaderUnit* p = gx_strg_Shaders;
	while ( p )
	{
		if ( p->shaderProgram == shaderProgram ) return p;
		p = p->next;
	}
	return 0;
}

GXVoid GXCALL DeleteShaderUnit ( GLuint shaderProgram )
{
	GXShaderUnit* p = gx_strg_Shaders;
	while ( p )
	{
		if ( p->shaderProgram == shaderProgram ) break;
		p = p->next;
	}
	if ( p == 0 ) return;
	if ( p == gx_strg_Shaders )
	{
		gx_strg_Shaders = p->next;
		if ( gx_strg_Shaders )
			gx_strg_Shaders->prev = 0;
		GXSafeDelete ( p );
	}
	else 
	{
		if ( p->next == 0)
		{
			p->prev->next = 0;
			GXSafeDelete ( p );
		}
		else
		{
			p->prev->next = p->next;
			p->next->prev = p->prev;
			GXSafeDelete ( p );
		}
	}
}

GLuint GXCALL GXGetShaderProgram ( GXShaderInfo &shader_info )
{
	GXUShort vShaderLen = GXWcslen ( shader_info.vShaderFileName );
	GXUShort gShaderLen = shader_info.gShaderFileName ? GXWcslen ( shader_info.gShaderFileName ) : 0;
	GXUShort fShaderLen = GXWcslen ( shader_info.fShaderFileName );

	GXWChar* shaderName = (GXWChar*)malloc ( ( vShaderLen + gShaderLen + fShaderLen + 6 + 1 ) * sizeof ( GXWChar ) );

	wsprintfW ( shaderName, L"%s + %s + %s", shader_info.vShaderFileName, shader_info.gShaderFileName, shader_info.fShaderFileName );

	GXShaderUnit* shader = FindShaderUnitByShaderName ( shaderName );
	//Если шейдерная программа уже загружена
	if ( shader )
	{
		shader->count_ref++;
		shader_info.uiId = shader->shaderProgram;
		free ( shaderName );
		return shader->shaderProgram;
	}

	//Если шейдерную программу необходимо загрузить
	shader = new GXShaderUnit ();
	shader->count_ref = 1;
	shader->shaderFullName = shaderName;

	shader->shaderProgram = GXCreateShaderProgram ( shader_info.vShaderFileName, shader_info.gShaderFileName, shader_info.fShaderFileName );
	shader_info.uiId = shader->shaderProgram;
	AddShaderUnit ( shader );

	return shader->shaderProgram;
}

GLuint GXCALL GXGetTransformFeedbackShaderProgram ( GXShaderInfo &shader_info, GXUChar numVaryings, const GXChar** varyings, GLenum bufferMode )
{
	GXUShort vShaderLen = wcslen ( shader_info.vShaderFileName );
	GXUShort gShaderLen = shader_info.gShaderFileName ? wcslen ( shader_info.gShaderFileName ) : 0;
	GXUShort fShaderLen = wcslen ( shader_info.fShaderFileName );

	GXWChar* shaderName = (GXWChar*)malloc ( ( vShaderLen + gShaderLen + fShaderLen + 6 + 1 ) * sizeof ( GXWChar ) );

	wsprintfW ( shaderName, L"%s + %s + %s", shader_info.vShaderFileName, shader_info.gShaderFileName, shader_info.fShaderFileName );

	GXShaderUnit* shader = FindShaderUnitByShaderName ( shaderName );
	//Если шейдерная программа уже загружена
	if ( shader )
	{
		shader->count_ref++;
		shader_info.uiId = shader->shaderProgram;
		free ( shaderName );
		return shader->shaderProgram;
	}

	//Если шейдерную программу необходимо загрузить
	shader = new GXShaderUnit ();
	shader->count_ref = 1;
	shader->shaderFullName = shaderName;

	shader->shaderProgram = GXCreateTransformFeedbackShaderProgram ( shader_info.vShaderFileName, shader_info.gShaderFileName, shader_info.fShaderFileName, numVaryings, varyings, bufferMode );
	shader_info.uiId = shader->shaderProgram;
	AddShaderUnit ( shader );

	return shader->shaderProgram;
}

GXVoid GXCALL GXRemoveShaderProgram ( GXShaderInfo &shader_info  )
{
	GXShaderUnit* shader = FindShaderUnitByShaderProgram ( shader_info.uiId );

	if ( !shader ) return;

	shader->count_ref--;
	if ( shader->count_ref > 0 ) return;

	free ( shader->shaderFullName );
	DeleteShaderUnit ( shader->shaderProgram );

	glUseProgram ( 0 );
	glDeleteProgram ( shader_info.uiId );
}

GXUInt GXCALL GXGetTotalShaderStorageObjects ( GXWChar** lastShader )
{
	GXUInt objects = 0;
	*lastShader = 0;

	for ( GXShaderUnit* p = gx_strg_Shaders; p; p = p->next )
	{
		*lastShader = p->shaderFullName;
		objects++;
	}

	return objects;
}