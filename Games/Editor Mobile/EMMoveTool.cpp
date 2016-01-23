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
	memset ( startLocationWorld.v, 0, sizeof ( GXVec3 ) );
	memset ( deltaWorld.v, 0, sizeof ( GXVec3 ) );

	memset ( &xAxis, 0, sizeof ( GXVAOInfo ) );

	activeAxis = EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN;

	isDeleted = GX_FALSE;
	isLMBPressed = GX_FALSE;

	mouseX = mouseY = 0xFFFF;

	gismoScaleCorrector = 1.0f;

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

		startLocationWorld = actorOrigin.wv;
	}
	else
		startLocationWorld = actorOrigin.wv;
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

	GXMat4 rot_mat;
	GXMat4 scale_mat;
	GXMat4 mod_mat;
	GXMat4 mod_view_proj_mat;

	const GXMat4& actorOrigin = actor->GetOrigin ();
	rot_mat = actorOrigin;
	rot_mat.wv = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
	GXSetMat4Scale ( scale_mat, 1.0f, 1.0f, 1.0f );
	GXMulMat4Mat4 ( mod_mat, rot_mat, scale_mat );
	GXSetMat4TranslateTo ( mod_mat, actorOrigin.m41, actorOrigin.m42, actorOrigin.m43 );
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, gx_ActiveCamera->GetViewProjectionMatrix () );

	glUniformMatrix4fv ( clr_mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

	glDisable ( GL_BLEND );

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

	glEnable ( GL_BLEND );

	glBindVertexArray ( 0 );
	glUseProgram ( 0 );
}

GXVoid EMMoveTool::OnDrawHudMaskPass ()
{
	if ( !actor ) return;
	
	glUseProgram ( maskShader.uiId );

	GXMat4 rot_mat;
	GXMat4 scale_mat;
	GXMat4 mod_mat;
	GXMat4 mod_view_proj_mat;

	const GXMat4& actorOrigin = actor->GetOrigin ();
	rot_mat = actorOrigin;
	rot_mat.wv = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
	GXSetMat4Scale ( scale_mat, 1.0f, 1.0f, 1.0f );
	GXMulMat4Mat4 ( mod_mat, rot_mat, scale_mat );
	GXSetMat4TranslateTo ( mod_mat, actorOrigin.m41, actorOrigin.m42, actorOrigin.m43 );
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, gx_ActiveCamera->GetViewProjectionMatrix () );

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
	if ( activeAxis == EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN ) return;

	GXVec3 axisLocationView;
	GXMulVec3Mat4AsPoint ( axisLocationView, startLocationWorld, gx_ActiveCamera->GetViewMatrix () );

	GXVec3 axisDirectionView;
	GetAxis ( axisDirectionView );

	GXVec3 rayView;
	GetRayPerspective ( rayView );

	GXFloat axisParameterDelta = GetAxisParameter ( axisLocationView, axisDirectionView, rayView ) - axisStartParameter;

	GXVec3 deltaView;
	deltaView.x = axisDirectionView.x * axisParameterDelta;
	deltaView.y = axisDirectionView.y * axisParameterDelta;
	deltaView.z = axisDirectionView.z * axisParameterDelta;

	GXMulVec3Mat4AsNormal ( deltaWorld, deltaView, gx_ActiveCamera->GetModelMatrix () );
	GXMat4 newOrigin = actor->GetOrigin ();
	GXSumVec3Vec3 ( newOrigin.wv, startLocationWorld, deltaWorld );

	actor->SetOrigin ( newOrigin );
}

GXFloat EMMoveTool::GetScaleFactor ( const GXVec3 &deltaWorld )
{
	//TODO
	return 1.0f;
}

GXVoid EMMoveTool::GetAxis ( GXVec3& axisView )
{
	GXVec3 axisWorld;

	switch ( activeAxis )
	{
		case EM_MOVE_TOOL_ACTIVE_AXIS_X:
			axisWorld = GXCreateVec3 ( 1.0f, 0.0f, 0.0f );
		break;

		case EM_MOVE_TOOL_ACTIVE_AXIS_Y:
			axisWorld = GXCreateVec3 ( 0.0f, 1.0f, 0.0f );
		break;

		case EM_MOVE_TOOL_ACTIVE_AXIS_Z:
			axisWorld = GXCreateVec3 ( 0.0f, 0.0f, 1.0f );
		break;

		default:
			GXLogW ( L"EMMoveTool::GetAxis::Error - Неизвестная ось\n" );
			return;
		break;
	}

	GXMulVec3Mat4AsNormal ( axisView, axisWorld, gx_ActiveCamera->GetViewMatrix () );
}

GXVoid EMMoveTool::GetRayPerspective ( GXVec3 &rayView )
{
	GXRenderer* renderer = gx_Core->GetRenderer ();
	const GXMat4& proj_mat = gx_ActiveCamera->GetProjectionMatrix ();

	GXVec2 mouseCVV;
	mouseCVV.x = ( mouseX / (GXFloat)renderer->GetWidth () ) * 2.0f - 1.0f;
	mouseCVV.y = ( mouseY / (GXFloat)renderer->GetHeight () ) * 2.0f - 1.0f;

	GXGetRayPerspective ( rayView, proj_mat, mouseCVV );
}

GXFloat EMMoveTool::GetAxisParameter ( const GXVec3 &axisLocationView, const GXVec3 &axisDirectionView, const GXVec3 &rayView )
{
	GXFloat b = axisLocationView.x * rayView.x + axisLocationView.y * rayView.y + axisLocationView.z * rayView.z;
	GXFloat c = axisDirectionView.x * rayView.x + axisDirectionView.y * rayView.y + axisDirectionView.z * rayView.z;
	GXFloat d = axisDirectionView.x * axisDirectionView.x + axisDirectionView.y * axisDirectionView.y + axisDirectionView.z * axisDirectionView.z;
	GXFloat e = axisLocationView.x * axisDirectionView.x + axisLocationView.y * axisDirectionView.y + axisLocationView.z * axisDirectionView.z;
	GXFloat zeta = ( rayView.x * rayView.x + rayView.y * rayView.y + rayView.z * rayView.z ) * c;
	GXFloat omega = c * c;

	return ( zeta * e - b * omega ) / ( c * omega - zeta * d );
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

	if ( em_mt_Me->activeAxis == EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN ) return;

	if ( em_mt_Me->mode == EM_MOVE_TOOL_WORLD_MODE )
	{
		const GXMat4& origin = em_mt_Me->actor->GetOrigin ();
		em_mt_Me->startLocationWorld = origin.wv;
	}
	else
		memset ( em_mt_Me->startLocationWorld.v, 0, sizeof ( GXVec3 ) );

	GXVec3 axisLocationView;
	GXMulVec3Mat4AsPoint ( axisLocationView, em_mt_Me->startLocationWorld, gx_ActiveCamera->GetViewMatrix () );

	GXVec3 axisDirectionView;
	em_mt_Me->GetAxis ( axisDirectionView );

	GXVec3 rayView;
	em_mt_Me->GetRayPerspective ( rayView );

	em_mt_Me->axisStartParameter = em_mt_Me->GetAxisParameter ( axisLocationView, axisDirectionView, rayView );
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
		em_Renderer->GetObject ( em_mt_Me->mouseX, em_mt_Me->mouseY );
	}
	else
	{
		if ( em_mt_Me->activeAxis == EM_MOVE_TOOL_ACTIVE_AXIS_UNKNOWN ) return;

		em_mt_Me->startLocationWorld.x += em_mt_Me->deltaWorld.x;
		em_mt_Me->startLocationWorld.y += em_mt_Me->deltaWorld.y;
		em_mt_Me->startLocationWorld.z += em_mt_Me->deltaWorld.z;

		GXMat4 newOrigin = em_mt_Me->actor->GetOrigin ();
		newOrigin.wv = em_mt_Me->startLocationWorld;
		em_mt_Me->actor->SetOrigin ( newOrigin );

		em_mt_Me->isLMBPressed = GX_FALSE;
	}
}
