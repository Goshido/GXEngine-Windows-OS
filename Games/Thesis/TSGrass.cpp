//version 1.0

#include "TSGrass.h"
#include <GXEngine/GXGlobals.h>


TSGrass::TSGrass ( GXWChar* meshFile, GXWChar* meshCache, GXWChar* materialFile, GXBool twoSided ) :
TSMesh ( meshFile, meshCache, materialFile, twoSided )
{
	accum = 0.0f;
	speed = 0.01f;

	constOffset = 0.2f;
	amplitude = GXCreateVec2 ( 0.3f, 0.05f );
	direction = GXCreateVec2 ( 0.4f, 0.1f );
	offset = GXCreateVec2 ( 0.0f, 0.0f );

	InitUniforms ();
}

TSGrass::~TSGrass ()
{
	//NOTHING
}

GXVoid TSGrass::Draw ()
{
	const GXMat4* view_mat = gx_ActiveCamera->GetViewMatrixPtr ();
	GXMat4 mod_view_mat;
	GXMulMat4Mat4 ( mod_view_mat, mod_mat, *view_mat );

	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniformMatrix4fv ( mod_view_matLocation, 1, GL_FALSE, mod_view_mat.A );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );
	glUniform2fv ( offsetLocation, 1, offset._v );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 0 ].uiId );

	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 1 ].uiId );

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 2 ].uiId );

	glActiveTexture ( GL_TEXTURE3 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 3 ].uiId );

	glBindVertexArray ( vaoInfo.vao );
	if ( twoSided )
	{
		glDisable ( GL_CULL_FACE );
		glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
		glEnable ( GL_CULL_FACE );
	}
	else
		glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
}

GXVoid TSGrass::UpdateWind ( GXDword deltaTime )
{
	accum += deltaTime * speed;

	GXFloat factor = sinf ( accum ) + constOffset;
	offset.x = amplitude.x * direction.x * factor;
	offset.y = amplitude.y * direction.y * factor;
}

GXVoid TSGrass::SetWindSpeed ( GXFloat speed )
{
	this->speed = speed;
}

GXVoid TSGrass::SetWindDirection ( const GXVec2 &direction )
{
	this->direction = direction;
}

GXVoid TSGrass::SetWindAmplitude ( const GXVec2 &amplitude )
{
	this->amplitude = amplitude;
}

GXVoid TSGrass::InitUniforms ()
{
	offsetLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "offset" );
}