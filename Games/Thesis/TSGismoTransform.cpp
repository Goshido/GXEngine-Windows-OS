//version 1.1


#include <GXEngine/GXGlobals.h>
#include "TSGismoTransform.h"


TSGismoTransform::TSGismoTransform ()
{
	Load3DModel ();
	InitUniforms ();
}

TSGismoTransform::~TSGismoTransform ()
{
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );

	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
	glDeleteBuffers ( 1, &meshVBO );

	glBindVertexArray ( 0 );
	glDeleteVertexArrays ( 1, &meshVAO );
}

GXVoid TSGismoTransform::Draw ()
{
	GXMat4 view_proj_mat;
	view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrix ();

	GXMat4 mod_view_proj;
	GXMulMat4Mat4 ( mod_view_proj, mod_mat, view_proj_mat );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj.A );

	glBindVertexArray ( meshVAO );
	glDrawArrays ( GL_LINES, 0, numVertices );
}

GXVoid TSGismoTransform::Load3DModel ()
{
	GXLoadMTR ( L"../Materials/Thesis/Gismo_Axis.mtr", mat_info );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );

	numVertices = 6;

	GXUShort size = numVertices * 2 * sizeof ( GXVec3 );
	GXVec3* data = (GXVec3*)malloc ( 6 * 2 * sizeof ( GXVec3 ) );

	data[ 0 ].x = data[ 0 ].y = data[ 0 ].z = 0.0f;
	data[ 1 ].r = 1.0f;
	data[ 1 ].g = data[ 1 ].b = 0.0f;

	data[ 2 ].x = 1.0f;
	data[ 2 ].y = data[ 2 ].z = 0.0f;
	data[ 3 ].r = 1.0f;
	data[ 3 ].g = data[ 3 ].b = 0.0f;

	data[ 4 ].x = data[ 4 ].y = data[ 4 ].z = 0.0f;
	data[ 5 ].g = 1.0f;
	data[ 5 ].r = data[ 5 ].b = 0.0f;

	data[ 6 ].y = 1.0f;
	data[ 6 ].x = data[ 6 ].z = 0.0f;
	data[ 7 ].g = 1.0f;
	data[ 7 ].r = data[ 7 ].b = 0.0f;

	data[ 8 ].x = data[ 8 ].y = data[ 8 ].z = 0.0f;
	data[ 9 ].b = 1.0f;
	data[ 9 ].r = data[ 9 ].g = 0.0f;

	data[ 10 ].z = 1.0f;
	data[ 10 ].x = data[ 10 ].y = 0.0f;
	data[ 11 ].b = 1.0f;
	data[ 11 ].r = data[ 1 ].g = 0.0f;

	glGenBuffers ( 1, &meshVBO );
	glBindBuffer ( GL_ARRAY_BUFFER, meshVBO );
	glBufferData ( GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW );

	glGenVertexArrays ( 1, &meshVAO );
	glBindVertexArray ( meshVAO );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, meshVBO );

		#define TS_VERTEX	0
		#define TS_COLOR	1

		glVertexAttribPointer ( TS_VERTEX, 3, GL_FLOAT, GL_TRUE, 2 * sizeof ( GXVec3 ), 0 );
		glEnableVertexAttribArray ( TS_VERTEX );

		const GXUInt offs = sizeof ( GXVec3 );
		glVertexAttribPointer ( TS_COLOR, 3, GL_FLOAT, GL_TRUE, 2 * sizeof ( GXVec3 ), (const GLvoid*)offs );
		glEnableVertexAttribArray ( TS_COLOR );

		#undef TS_VERTEX
		#undef TS_COLOR

	//}

	glBindVertexArray ( 0 );
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	free ( data );
}

GXVoid TSGismoTransform::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	mod_view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_proj_mat" );

	glUseProgram ( 0 );
}