//versino 1.2

#include "TSBillboard.h"
#include <GXEngine/GXGlobals.h>


#define TS_LOCKED_NONE		0
#define TS_LOCKED_RIGHT		1
#define TS_LOCKED_UP		2
#define TS_LOCKED_FORWARD	3


TSBillboard::TSBillboard ( const GXWChar* materialFile )
{
	GXUInt size = sizeof ( GXWChar ) * ( wcslen ( materialFile ) + 1 );
	this->materialFile = (GXWChar*)malloc ( size );
	memcpy ( this->materialFile, materialFile, size );

	lockedAxis = TS_LOCKED_NONE;

	isDelete = GX_FALSE;
}

GXVoid TSBillboard::Draw ()
{
	if ( vaoInfo.vao == 0 )
		InitGraphicalResources ();

	if ( isDelete )
	{
		delete this;
		return;
	}

	GXMat4 rot;
	GXSetMat4Identity ( rot );

	GXMat4 cam_rot;
	GXVec3 cam_loc;
	gx_ActiveCamera->GetRotation ( cam_rot );
	gx_ActiveCamera->GetLocation ( cam_loc );

	switch ( lockedAxis )
	{
		case TS_LOCKED_NONE:
			rot = cam_rot;
		break;

		case TS_LOCKED_RIGHT:
		{
			rot.yv = rot_mat.xv;
			
			GXCrossVec3Vec3 ( rot.xv, rot.yv, cam_rot.zv );
			GXCrossVec3Vec3 ( rot.zv, rot.xv, rot.yv );
		}
		break;

		case TS_LOCKED_UP:
		{
			rot.yv = rot_mat.yv;

			GXCrossVec3Vec3 ( rot.xv, rot.yv, cam_rot.zv );
			GXCrossVec3Vec3 ( rot.zv, rot.xv, rot.yv );
		}
		break;

		case TS_LOCKED_FORWARD:
			{
				rot.yv = rot_mat.zv;

				GXCrossVec3Vec3 ( rot.xv, rot.yv, cam_rot.zv );
				GXCrossVec3Vec3 ( rot.zv, rot.xv, rot.yv );
			}
		break;
	}

	GXMulMat4Mat4 ( scale_rot_mat, scale_mat, rot );

	mod_mat = scale_rot_mat;
	mod_mat.wv = trans_mat.wv;

	GXMat4 mod_view_proj_mat;
	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();

	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 0 ].uiId );

	glDepthMask ( GL_FALSE );
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if ( lockedAxis == TS_LOCKED_NONE )
		glBindVertexArray ( vaoInfo.vao );
	else
		glBindVertexArray ( lockedQuad.vao );

	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	glDepthMask ( GL_TRUE );
	glDisable ( GL_BLEND );
}

GXVoid TSBillboard::Delete ()
{
	isDelete = GX_TRUE;
}

GXVoid TSBillboard::LockRight ()
{
	lockedAxis = TS_LOCKED_RIGHT;
}

GXVoid TSBillboard::LockUp ()
{
	lockedAxis = TS_LOCKED_UP;
}

GXVoid TSBillboard::LockForward ()
{
	lockedAxis = TS_LOCKED_FORWARD;
}

GXVoid TSBillboard::UnlockAxis ()
{
	lockedAxis = TS_LOCKED_NONE;
}

TSBillboard::~TSBillboard ()
{
	GXSafeDelete ( materialFile );

	GXRemoveVAO ( vaoInfo );
	GXRemoveVAO ( lockedQuad );
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
	GXRemoveTexture ( mat_info.textures[ 0 ] );
}

GXVoid TSBillboard::Load3DModel ()
{
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/System/ScreenQuad.obj", L"../3D Models/System/Cache/ScreenQuad.cache" );
	GXGetVAOFromOBJ ( lockedQuad, L"../3D Models/System/BottomPivotQuad.obj", L"../3D Models/System/Cache/BottomPivotQuad.cache" );
	
	GXLoadMTR ( materialFile, mat_info );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
	GXGetTexture ( mat_info.textures[ 0 ] );

	GXSafeDelete ( materialFile );
}

GXVoid TSBillboard::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	mod_view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[0].uiId, "mod_view_proj_mat" );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "Texture" ), 0 );

	glUseProgram ( 0 );
}

GXVoid TSBillboard::InitGraphicalResources ()
{
	Load3DModel ();
	InitUniforms ();
}