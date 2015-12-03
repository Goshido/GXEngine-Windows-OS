//version 1.1

#include "TSHud2DPen.h"
#include <GXEngine/GXGlobals.h>


TSHud2DPen::TSHud2DPen ()
{
	Load3DModel ();
	InitUniforms ();

	GXVec3 start ( -1.0f, 0.0f, 0.0f );
	GXVec3 finish ( 1.0f, 0.0f, 0.0f );

	SetEnds ( start, finish );

	thickness = 1;
	GXColorToVec4 ( color, 255, 255, 255, 255 );
}

TSHud2DPen::~TSHud2DPen ()
{
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );

	glDeleteBuffers ( 1, &meshVBO );
	glDeleteVertexArrays ( 1, &meshVAO );
}

GXVoid TSHud2DPen::Draw ( GXVoid* arg )
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	GXMat4 view_proj_mat;
	view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrix ();

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, view_proj_mat.A );
	glUniform4fv ( lineColorLocation, 1, color.v );

	glBindVertexArray ( meshVAO );
	
	GXMat4 offset;
	GXSetMat4Translation ( offset, 0.0f, 0.0f, 0.0f );

	GXShort halfThick = thickness / 2;

	offset.m41 = -(GXFloat)( -halfThick ) * normal.x;
	offset.m42 = -(GXFloat)( -halfThick ) * normal.y;

	GXMat4 mod_view_prog_mat;

	for ( GXShort i = -halfThick; i <= halfThick; i++ )
	{
		GXMulMat4Mat4 ( mod_view_prog_mat, offset, view_proj_mat );

		glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_prog_mat.A );

		glDrawArrays ( GL_LINES, 0, 2 );

		offset.m41 -= normal.x;
		offset.m42 -= normal.y;
	}
}

GXVoid TSHud2DPen::SetColor ( GXFloat r, GXFloat g, GXFloat b, GXFloat a )
{
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
}

GXVoid TSHud2DPen::SetThickness ( GXUShort thickness )
{
	if ( thickness == 0 )
	{
		GXLogW ( L"TSHud2DPen::SetThickness::Error - Толщина линии не может быть 0\n" );
		return;
	}

	this->thickness = thickness;
}

GXVoid TSHud2DPen::SetEnds ( GXVec3 &start, GXVec3 &finish )
{
	points[ 0 ] = start;
	points[ 1 ] = finish;

	normal.x = points[ 0 ].y - points[ 1 ].y;
	normal.y = points[ 1 ].x - points[ 0 ].x;
	normal.z = 0.0f;

	GXNormalizeVec3 ( normal );

	normal.x *= 0.75f;
	normal.y *= 0.75f;

	glBindBuffer ( GL_ARRAY_BUFFER, meshVBO );
	glBufferData ( GL_ARRAY_BUFFER, 2 * sizeof ( GXVec3 ), points, GL_DYNAMIC_DRAW );
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
}

GXVoid TSHud2DPen::Load3DModel ()
{
	numVertices = 2;

	memset ( points, 0, 2 * sizeof ( GXVec3 ) );

	glGenVertexArrays ( 1, &meshVAO );
	glBindVertexArray ( meshVAO );
	//{
		glGenBuffers ( 1, &meshVBO );
		glBindBuffer ( GL_ARRAY_BUFFER, meshVBO );
		//{
			glBufferData ( GL_ARRAY_BUFFER, 2 * sizeof ( GXVec3 ), points, GL_DYNAMIC_DRAW );
			glVertexAttribPointer ( GX_VERTEX_INDEX, 3, GL_FLOAT, GL_TRUE, sizeof ( GXVec3 ), 0 );
			glEnableVertexAttribArray ( GX_VERTEX_INDEX );
		//}
	//}

	glBindVertexArray ( 0 );
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );

	GXLoadMTR ( L"../Materials/HudLine.mtr", mat_info );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
}

GXVoid TSHud2DPen::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	lineColorLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "lineColor" );
	mod_view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_proj_mat" );

	glUseProgram ( 0 );
}