#include "EMMoveTool.h"
#include "EMGlobals.h"
#include <GXEngine/GXShaderStorageExt.h>
#include <GXEngine/GXShaderUtils.h>
#include <GXCommon/GXLogger.h>


#define EM_MOVE_TOOL_LOCAL_MODE		0
#define EM_MOVE_TOOL_WORLD_MODE		1

#define EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN	0xFF
#define EM_MOVE_TOOL_ACTIVE_AXIS_X			0
#define EM_MOVE_TOOL_ACTIVE_AXIS_Y			1
#define EM_MOVE_TOOL_ACTIVE_AXIS_Z			2


EMMoveTool* em_mt_Me;

EMMoveTool::EMMoveTool ()
{
	mode = EM_MOVE_TOOL_WORLD_MODE;
	GXSetMat4Identity ( origin );

	memset ( &xAxis, 0, sizeof ( GXVAOInfo ) );

	activeAxis = EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN;

	isDeleted = GX_FALSE;
	isLMBPressed = GX_FALSE;

	mouseX = mouseY = mouseBeginX = mouseBeginY = 0xFFFF;

	em_mt_Me = this;
}

GXVoid EMMoveTool::Delete ()
{
	isDeleted = GX_TRUE;
}

GXVoid EMMoveTool::Bind ()
{
	em_Tool = this;
	actor = 0;
	activeAxis = EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN;
	isLMBPressed = GX_FALSE;

	em_Renderer->SetOnObjectCallback ( &OnObject );

	gx_Core->GetInput ()->BindMouseMoveFunc ( &OnMouseMove );
	gx_Core->GetInput ()->BindMouseButtonsFunc ( &OnMouseButton );
}

GXVoid EMMoveTool::SetActor ( EMActor* actor )
{
	this->actor = actor;

	const GXMat4& actorOrigin = actor->GetOrigin ();
	GXMat4 rot;

	if ( mode == EM_MOVE_TOOL_LOCAL_MODE )
	{
		GXVec3 scaleFactor;
		scaleFactor.x = 1.0f / GXLengthVec3 ( actorOrigin.xv );
		scaleFactor.y = 1.0f / GXLengthVec3 ( actorOrigin.yv );
		scaleFactor.z = 1.0f / GXLengthVec3 ( actorOrigin.zv );

		rot.m11 = actorOrigin.m11 * scaleFactor.x;
		rot.m12 = actorOrigin.m12 * scaleFactor.x;
		rot.m13 = actorOrigin.m13 * scaleFactor.x;

		rot.m21 = actorOrigin.m21 * scaleFactor.y;
		rot.m22 = actorOrigin.m22 * scaleFactor.y;
		rot.m23 = actorOrigin.m23 * scaleFactor.y;

		rot.m31 = actorOrigin.m31 * scaleFactor.z;
		rot.m32 = actorOrigin.m32 * scaleFactor.z;
		rot.m33 = actorOrigin.m33 * scaleFactor.z;

		rot.m14 = rot.m24 = rot.m34 = rot.m41 = rot.m42 = rot.m43 = 0.0f;
		rot.m44 = 1.0f;

		memcpy ( &origin, &rot, sizeof ( GXMat4 ) );
		GXSetMat4TranslateTo ( origin, actorOrigin.m41, actorOrigin.m42, actorOrigin.m43 );
	}
	else
		GXSetMat4Translation ( origin, actorOrigin.m41, actorOrigin.m42, actorOrigin.m43 );
}

GXVoid EMMoveTool::UnBind ()
{
	em_Tool = 0;

	gx_Core->GetInput ()->UnBindMouseMoveFunc ();
	gx_Core->GetInput ()->UnBindMouseButtonsFunc ();

	em_Renderer->SetOnObjectCallback ( 0 );
}

GXVoid EMMoveTool::OnDrawHudColorPass ()
{
	if ( xAxis.vao == 0 )
	{
		Load3DModels ();
		InitUniforms ();
	}

	if ( isDeleted )
	{
		delete this;
		return;
	}

	if ( !actor ) return;

	glUseProgram ( colorShader.uiId );

	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, origin, gx_ActiveCamera->GetViewProjectionMatrix () );
	glUniformMatrix4fv ( clr_mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

	if ( activeAxis == EM_MOVE_TOOL_ACTIVE_AXIS_X )
		glUniform4f ( clr_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f );
	else
		glUniform4f ( clr_colorLocation, 1.0f, 0.0f, 0.0f, 1.0f );
	glBindVertexArray ( xAxis.vao );
	glDrawArrays ( GL_TRIANGLES, 0, xAxis.numVertices );

	if ( activeAxis == EM_MOVE_TOOL_ACTIVE_AXIS_Y )
		glUniform4f ( clr_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f );
	else
		glUniform4f ( clr_colorLocation, 0.0f, 1.0f, 0.0f, 1.0f );
	glBindVertexArray ( yAxis.vao );
	glDrawArrays ( GL_TRIANGLES, 0, yAxis.numVertices );

	if ( activeAxis == EM_MOVE_TOOL_ACTIVE_AXIS_Z )
		glUniform4f ( clr_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f );
	else
		glUniform4f ( clr_colorLocation, 0.0f, 0.0f, 1.0f, 1.0f );
	glBindVertexArray ( zAxis.vao );
	glDrawArrays ( GL_TRIANGLES, 0, zAxis.numVertices );

	glUniform4f ( clr_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f );
	glBindVertexArray ( center.vao );
	glDrawArrays ( GL_TRIANGLES, 0, center.numVertices );

	glBindVertexArray ( 0 );
	glUseProgram ( 0 );
}

GXVoid EMMoveTool::OnDrawHudMaskPass ()
{
	if ( !actor ) return;
	
	glUseProgram ( maskShader.uiId );

	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, origin, gx_ActiveCamera->GetViewProjectionMatrix () );
	glUniformMatrix4fv ( msk_mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

	em_Renderer->SetObjectMask ( (GXUInt)( &xAxisMask ) );
	glBindVertexArray ( xAxisMask.vao );
	glDrawArrays ( GL_TRIANGLES, 0, xAxisMask.numVertices );

	em_Renderer->SetObjectMask ( (GXUInt)( &yAxisMask ) );
	glBindVertexArray ( yAxisMask.vao );
	glDrawArrays ( GL_TRIANGLES, 0, yAxisMask.numVertices );

	em_Renderer->SetObjectMask ( (GXUInt)( &zAxisMask ) );
	glBindVertexArray ( zAxisMask.vao );
	glDrawArrays ( GL_TRIANGLES, 0, zAxisMask.numVertices );

	glBindVertexArray ( 0 );
	glUseProgram ( 0 );
}

GXVoid EMMoveTool::SetLocalMode ()
{
	mode = EM_MOVE_TOOL_LOCAL_MODE;
}

GXVoid EMMoveTool::SetWorldMode ()
{
	mode = EM_MOVE_TOOL_WORLD_MODE;
}

EMMoveTool::~EMMoveTool ()
{
	if ( xAxis.vao == 0 ) return;

	GXRemoveVAO ( xAxis );
	GXRemoveVAO ( xAxisMask );
	GXRemoveVAO ( yAxis );
	GXRemoveVAO ( yAxisMask );
	GXRemoveVAO ( zAxis );
	GXRemoveVAO ( zAxisMask );
	GXRemoveVAO ( center );

	GXRemoveShaderProgramExt ( colorShader );
	GXRemoveShaderProgramExt ( maskShader );
}

GXVoid EMMoveTool::Load3DModels ()
{
	GXGetVAOFromNativeStaticMesh ( xAxis, L"../3D Models/Editor Mobile/Move gismo X axis.stm" );
	GXGetVAOFromNativeStaticMesh ( xAxisMask, L"../3D Models/Editor Mobile/Move gismo X axis mask.stm" );
	GXGetVAOFromNativeStaticMesh ( yAxis, L"../3D Models/Editor Mobile/Move gismo Y axis.stm" );
	GXGetVAOFromNativeStaticMesh ( yAxisMask, L"../3D Models/Editor Mobile/Move gismo Y axis mask.stm" );
	GXGetVAOFromNativeStaticMesh ( zAxis, L"../3D Models/Editor Mobile/Move gismo Z axis.stm" );
	GXGetVAOFromNativeStaticMesh ( zAxisMask, L"../3D Models/Editor Mobile/Move gismo z axis mask.stm" );
	GXGetVAOFromNativeStaticMesh ( center, L"../3D Models/Editor Mobile/Move gismo center.stm" );

	GXGetShaderProgramExt ( colorShader, L"../Shaders/Editor Mobile/VertexOnly_vs.txt", 0, L"../Shaders/Editor Mobile/Color_fs.txt" );
	GXGetShaderProgramExt ( maskShader, L"../Shaders/Editor Mobile/MaskVertexOnly_vs.txt", 0, L"../Shaders/Editor Mobile/Mask_fs.txt" );
}

GXVoid EMMoveTool::InitUniforms ()
{
	clr_mod_view_proj_matLocation = GXGetUniformLocation ( colorShader.uiId, "mod_view_proj_mat" );
	clr_colorLocation = GXGetUniformLocation ( colorShader.uiId, "color" );
	msk_mod_view_proj_matLocation = GXGetUniformLocation ( maskShader.uiId, "mod_view_proj_mat" );
}

GXVoid EMMoveTool::OnMoveActor ()
{
	GXLogW ( L"EMMoveTool::OnMoveActor::Info - Move!\n" );
}

GXVoid GXCALL EMMoveTool::OnObject ( GXUInt object )
{
	if ( object == (GXUInt)( &em_mt_Me->xAxisMask ) )
		em_mt_Me->activeAxis = EM_MOVE_TOOL_ACTIVE_AXIS_X;
	else if ( object == (GXUInt)( &em_mt_Me->yAxisMask ) )
		em_mt_Me->activeAxis = EM_MOVE_TOOL_ACTIVE_AXIS_Y;
	else if ( object == (GXUInt)( &em_mt_Me->zAxisMask ) )
		em_mt_Me->activeAxis = EM_MOVE_TOOL_ACTIVE_AXIS_Z;
	else
		em_mt_Me->activeAxis = EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN;
}

GXVoid GXCALL EMMoveTool::OnMouseMove ( GXInt win_x, GXInt win_y )
{
	em_mt_Me->mouseX = (GXUShort)win_x;
	em_mt_Me->mouseY = (GXUShort)( gx_Core->GetRenderer ()->GetHeight () - win_y );

	if ( em_mt_Me->isLMBPressed && em_mt_Me->activeAxis != EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN )
		em_mt_Me->OnMoveActor ();
}

GXVoid GXCALL EMMoveTool::OnMouseButton ( EGXInputMouseFlags mouseflags )
{
	if ( mouseflags.lmb ) 
	{
		em_mt_Me->isLMBPressed = GX_TRUE;
		em_mt_Me->mouseBeginX = em_mt_Me->mouseX;
		em_mt_Me->mouseBeginY = em_mt_Me->mouseY;
		em_Renderer->GetObject ( em_mt_Me->mouseX, em_mt_Me->mouseY );
	}
	else
		em_mt_Me->isLMBPressed = GX_FALSE;
}
