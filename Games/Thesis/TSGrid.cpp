//version 1.2

#include "TSGrid.h"
#include <GXEngine/GXGlobals.h>


TSGrid::TSGrid ( GXUInt cellFactor )
{
	this->cellFactor = cellFactor;
	GXColorToVec4 ( gridColor, 30, 40, 30, 255 );

	if ( cellFactor < 2 )
		GXLogW ( L"TSGrid::TSGrid::Error - cellFactor не может быть меньше 2\n" );

	Load3DModel ();
	InitUniforms ();
}

TSGrid::~TSGrid ()
{
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );

	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
	glDeleteBuffers ( 1, &meshVBO );

	glBindVertexArray ( 0 );
	glDeleteVertexArrays ( 1, &meshVAO );
}

GXVoid TSGrid::Draw ()
{
	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, view_proj_mat->A );
	glUniform4fv ( lineColorLocation, 1, gridColor.v );

	glBindVertexArray ( meshVAO );
	glDrawArrays ( GL_LINES, 0, numVertices );
}

GXVoid TSGrid::Load3DModel ()
{
	GXVec3 start;
	GXVec3 finish;

	GXLoadMTR ( L"../Materials/TSGrid.mtr", mat_info );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );

	GXUInt size = cellFactor * 4 * sizeof ( GXVec3 );
	GXVec3* data = (GXVec3*)malloc ( size );
	GXUInt ind = 0;

	GXInt half = cellFactor / 2 + 1;
	GXInt limit = half - 1;

	GXFloat range = (GXFloat)limit;

	for ( GXInt i = half - cellFactor; i <= limit; i++ )
	{
		GXFloat a = (GXFloat)i;

		data[ ind ].x = -range;
		data[ ind ].y = 0.0f;
		data[ ind++ ].z = a;

		data[ ind ].x = range;
		data[ ind ].y = 0.0f;
		data[ ind++ ].z = a;

		data[ ind ].x = a;
		data[ ind ].y = 0.0f;
		data[ ind++ ].z = range;

		data[ ind ].x = a;
		data[ ind ].y = 0.0f;
		data[ ind++ ].z = -range;
	}

	numVertices = ind;

	glGenBuffers ( 1, &meshVBO );
	glBindBuffer ( GL_ARRAY_BUFFER, meshVBO );
	glBufferData ( GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW );

	glGenVertexArrays ( 1, &meshVAO );
	glBindVertexArray ( meshVAO );
	//{
		glBindBuffer ( GL_ARRAY_BUFFER, meshVBO );
		glVertexAttribPointer ( GX_VERTEX_INDEX, 3, GL_FLOAT, GL_TRUE, sizeof ( GXVec3 ), 0 );
		glEnableVertexAttribArray ( GX_VERTEX_INDEX );
	//}

	glBindVertexArray ( 0 );
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	free ( data );
}

GXVoid TSGrid::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	lineColorLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "lineColor" );
	mod_view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_proj_mat" );

	glUseProgram ( 0 );
}