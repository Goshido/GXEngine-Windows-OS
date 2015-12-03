//version 1.0

#include "TSCascadeShadowmap.h"
#include "TSLight.h"
#include <GXEngine/GXGlobals.h>
#include <GXCommon/GXMTRLoader.h>


#define TS_WRONG_INDEX		0xFFFFFFFF
#define TS_LAMBDA_FACTOR	0.9f
#define TS_NUM_CASCADES		4


TSCascadeShadowmap::TSCascadeShadowmap ( GXUShort resolution, GXFloat altitude, GXFloat zFarLimit )
{
	this->numLevels = TS_NUM_CASCADES;
	this->zFarLimit = zFarLimit;

	splits = (GXFloat*)malloc ( ( numLevels + 1 ) * sizeof ( GXFloat ) );
	lightVPC = (GXMat4*)malloc ( numLevels * sizeof ( GXMat4 ) );
	lightVPCB = (GXMat4*)malloc ( numLevels * sizeof ( GXMat4 ) );
	totalMVPC = (GXMat4*)malloc ( numLevels * sizeof ( GXMat4 ) );

	GXSetMat4Scale ( bias, 0.5f, 0.5f, 0.5f );
	bias.wv = GXCreateVec3 ( 0.5f, 0.5f, 0.5f );

	this->resolution = resolution;
	this->altitude = altitude;
	
	InitFBO ();
	InitVisualResources ();
	InitShadowCasters ();
}

TSCascadeShadowmap::~TSCascadeShadowmap ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0 );
	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D_ARRAY, 0 );
	glDeleteTextures ( 1, &shadowmaps );

	GXRemoveShaderProgram ( matInfo.shaders[ 0 ] );

	matInfo.Cleanup ();

	free ( splits );
	free ( lightVPC );
	free ( totalMVPC );
	free ( lightVPCB );
}

GXVoid TSCascadeShadowmap::MakeShadowmaps ( const GXMat4** lightVPCB, const GXFloat** splits, GLuint &csm, TSDirectedLight* light, GXCameraPerspective* view  )
{
	UpdateSplits ( light, view );
	UpdateShadows ( light, view );

	for ( GXUChar i = 0; i < numLevels; i++ )
		GXMulMat4Mat4 ( this->lightVPCB[ i ], lightVPC[ i ], bias );

	*lightVPCB = this->lightVPCB;
	*splits = this->splits;
	csm = shadowmaps;
}

GXVoid TSCascadeShadowmap::IncludeShadowCaster ( GXMesh* caster )
{
	GXUInt insert = numShadowCasters;
	numShadowCasters++;

	if ( numShadowCasters >= maxShadowCasters )
	{
		maxShadowCasters += 32;

		GXMesh** temp = (GXMesh**)malloc ( sizeof ( GXMesh* ) * maxShadowCasters );
		memcpy ( temp, shadowCasters, sizeof ( GXMesh* ) * insert );

		free ( shadowCasters );
		shadowCasters = temp;
	}

	shadowCasters[ insert ] = caster;
}

GXVoid TSCascadeShadowmap::ExcludeShadowCaster ( GXMesh* caster )
{
	GXUInt i = FindShadowCasterIndex ( caster );

	if ( i == TS_WRONG_INDEX ) return;

	memmove ( shadowCasters + i, shadowCasters + i + 1, ( numShadowCasters - i - 1 ) * sizeof ( GXMesh* ) );

	numShadowCasters--;
}

GXVoid TSCascadeShadowmap::InitFBO ()
{
	glGenTextures ( 1, &shadowmaps );
	glBindTexture ( GL_TEXTURE_2D_ARRAY, shadowmaps );
	glTexParameteri ( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 0 );
	glTexParameteri ( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
	glTexImage3D ( GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32, resolution, resolution, (GLsizei)numLevels, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0 ); 

	glGenFramebuffers ( 1, &fbo );
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowmaps, 0 );

	const GLenum buffers = GL_NONE;
	glDrawBuffers ( 1, &buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"TSCascadeShadowmap::TSCascadeShadowmap::Error - Что-то не так с FBO\n" );
		MessageBoxW ( 0, L"Что-то не так с FBO", L"Ошибка", MB_ICONERROR );
	}

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
}

GXVoid TSCascadeShadowmap::InitVisualResources ()
{
	GXLoadMTR ( L"../Materials/Thesis/Cascade_Shadowmap.mtr", matInfo );
	GXGetShaderProgram ( matInfo.shaders[ 0 ] );

	glUseProgram ( matInfo.shaders[ 0 ].uiId );
	mod_view_proj_crop_matLocation = GXGetUniformLocation ( matInfo.shaders[ 0 ].uiId, "mod_view_proj_crop_mat" );
}

GXVoid TSCascadeShadowmap::InitShadowCasters ()
{
	maxShadowCasters = 32;
	numShadowCasters = 0;
	shadowCasters = (GXMesh**)malloc ( sizeof ( GXMesh* ) * maxShadowCasters );
}

GXVoid TSCascadeShadowmap::UpdateSplits ( TSDirectedLight* light, GXCameraPerspective* view )
{
	GXFloat n = view->GetZnear ();
	GXFloat f = view->GetZfar ();
	if ( f > zFarLimit ) f = zFarLimit;
	GXFloat factor = 1.0f / (GXFloat)( numLevels );

	for ( GXUChar i = 0; i <= numLevels; i++ )
	{
		GXFloat cLog = n * powf ( f / n, i * factor );
		GXFloat cUni = n + ( f - n ) * i * factor;
		splits[ i ] = TS_LAMBDA_FACTOR * cLog + ( 1.0f - TS_LAMBDA_FACTOR ) * cUni;
	}
}

GXVoid TSCascadeShadowmap::UpdateShadows ( TSDirectedLight* light, GXCameraPerspective* view )
{
	for ( GXUChar i = 0; i < numLevels; i++ )
	{
		UpdateFrustumSegment ( view, i );
		UpdateLightVPC ( light, view, i );
	}

	GLuint oldFBO;
	glGetIntegerv ( GL_FRAMEBUFFER_BINDING, (GLint*)&oldFBO );

	GLint oldVP[ 4 ];
	glGetIntegerv ( GL_VIEWPORT, oldVP );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glViewport ( 0, 0, resolution, resolution );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"TSCascadeShadowmap::UpdateShadows::Error - Что-то не так с FBO\n" );
		MessageBoxW ( 0, L"Что-то не так с FBO", L"Ошибка", MB_ICONERROR );
	}

	glClear ( GL_DEPTH_BUFFER_BIT );

	glUseProgram ( matInfo.shaders[ 0 ].uiId );

	for ( GXUInt i = 0; i < numShadowCasters; i++ )
	{
		const GXAABB& bounds = shadowCasters[ i ]->GetBoundsWorld ();

		GXBool isVisible = GX_FALSE;
		for ( GXUByte j = 0; j < numLevels; j++ )
		{
			GXProjectionClipPlanes cp ( lightVPC[ j ] );

			if ( cp.IsVisible ( bounds ) )
			{
				isVisible = GX_TRUE;
				break;
			}
		}

		if ( !isVisible ) continue;

		const GXVAOInfo& vaoInfo = shadowCasters[ i ]->GetMeshVAOInfo ();
		const GXMat4& mod_mat = shadowCasters[ i ]->GetModelMatrix ();

		for ( GXUChar j = 0; j < numLevels; j++ )
			GXMulMat4Mat4 ( totalMVPC[ j ], mod_mat, lightVPC[ j ] );
		
		glUniformMatrix4fv ( mod_view_proj_crop_matLocation, numLevels, GL_FALSE, totalMVPC->A );

		glBindVertexArray ( vaoInfo.vao );
		glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
	}

	glBindFramebuffer ( GL_FRAMEBUFFER, oldFBO );
	glViewport ( oldVP[ 0 ], oldVP[ 1 ], oldVP[ 2 ], oldVP[ 3 ] );
}

GXUInt TSCascadeShadowmap::FindShadowCasterIndex ( const GXMesh* caster )
{
	for ( GXUInt i = 0; i < maxShadowCasters; i++ )
		if ( shadowCasters[ i ] == caster ) 
			return i;

	return TS_WRONG_INDEX;
}

GXVoid TSCascadeShadowmap::UpdateFrustumSegment ( GXCameraPerspective* view, GXUChar level  )
{
	GXFloat aspect = view->GetAspectRatio ();
	GXFloat factor = tanf ( view->GetFov () * 0.5f );

	GXFloat y = splits[ level ] * factor;
	GXFloat x = aspect * y;
	GXFloat z = splits[ level ];

	segmentLocal.nearA = GXCreateVec3 ( x, y, z );
	segmentLocal.nearB = GXCreateVec3 ( -x, y, z );
	segmentLocal.nearC = GXCreateVec3 ( -x, -y, z );
	segmentLocal.nearD = GXCreateVec3 ( x, -y, z );

	y = splits[ level + 1 ] * factor;
	x = aspect * y;
	z = splits[ level + 1 ];

	segmentLocal.farA = GXCreateVec3 ( x, y, z );
	segmentLocal.farB = GXCreateVec3 ( -x, y, z );
	segmentLocal.farC = GXCreateVec3 ( -x, -y, z );
	segmentLocal.farD = GXCreateVec3 ( x, -y, z );

	const GXMat4& camera_mod_mat = view->GetModelMatrix ();

	GXMulVec3Mat4AsPoint ( segmentWorld.nearA, segmentLocal.nearA, camera_mod_mat );
	GXMulVec3Mat4AsPoint ( segmentWorld.nearB, segmentLocal.nearB, camera_mod_mat );
	GXMulVec3Mat4AsPoint ( segmentWorld.nearC, segmentLocal.nearC, camera_mod_mat );
	GXMulVec3Mat4AsPoint ( segmentWorld.nearD, segmentLocal.nearD, camera_mod_mat );

	GXMulVec3Mat4AsPoint ( segmentWorld.farA, segmentLocal.farA, camera_mod_mat );
	GXMulVec3Mat4AsPoint ( segmentWorld.farB, segmentLocal.farB, camera_mod_mat );
	GXMulVec3Mat4AsPoint ( segmentWorld.farC, segmentLocal.farC, camera_mod_mat );
	GXMulVec3Mat4AsPoint ( segmentWorld.farD, segmentLocal.farD, camera_mod_mat );

	factor = 0.125f;		// 1.0f / 8.0f

	segmentCenter.x = segmentWorld.v[ 0 ];
	segmentCenter.y = segmentWorld.v[ 1 ];
	segmentCenter.z = segmentWorld.v[ 2 ];

	for ( GXUChar i = 1; i < 8; i++ )
	{
		segmentCenter.x += segmentWorld.v[ i * 3 ];
		segmentCenter.y += segmentWorld.v[ i * 3 + 1 ];
		segmentCenter.z += segmentWorld.v[ i * 3 + 2 ];
	}

	segmentCenter.x *= factor;
	segmentCenter.y *= factor;
	segmentCenter.z *= factor;
}

GXVoid TSCascadeShadowmap::UpdateFrustumSegmentAABB ()
{
	GXMulVec3Mat4AsPoint ( segmentLightClip.nearA, segmentWorld.nearA, lightVP );
	GXMulVec3Mat4AsPoint ( segmentLightClip.nearB, segmentWorld.nearB, lightVP );
	GXMulVec3Mat4AsPoint ( segmentLightClip.nearC, segmentWorld.nearC, lightVP );
	GXMulVec3Mat4AsPoint ( segmentLightClip.nearD, segmentWorld.nearD, lightVP );

	GXMulVec3Mat4AsPoint ( segmentLightClip.farA, segmentWorld.farA, lightVP );
	GXMulVec3Mat4AsPoint ( segmentLightClip.farB, segmentWorld.farB, lightVP );
	GXMulVec3Mat4AsPoint ( segmentLightClip.farC, segmentWorld.farC, lightVP );
	GXMulVec3Mat4AsPoint ( segmentLightClip.farD, segmentWorld.farD, lightVP );

	GXSetAABBEmpty ( segmentAABB );

	GXAddVertexToAABB ( segmentAABB, segmentLightClip.nearA );
	GXAddVertexToAABB ( segmentAABB, segmentLightClip.nearB );
	GXAddVertexToAABB ( segmentAABB, segmentLightClip.nearC );
	GXAddVertexToAABB ( segmentAABB, segmentLightClip.nearD );

	GXAddVertexToAABB ( segmentAABB, segmentLightClip.farA );
	GXAddVertexToAABB ( segmentAABB, segmentLightClip.farB );
	GXAddVertexToAABB ( segmentAABB, segmentLightClip.farC );
	GXAddVertexToAABB ( segmentAABB, segmentLightClip.farD );
}

GXVoid TSCascadeShadowmap::UpdateLightVPC ( TSDirectedLight* light, GXCameraPerspective* view, GXUChar level )
{
	//Find ray intersection with plane: y = altitude;

	light->GetRotation ( lightM );
	
	GXFloat t = ( segmentCenter.y - altitude ) / lightM.zv.y + 1000.0f;

	lightM.m41 = segmentCenter.x - t * lightM.zv.x;
	lightM.m42 = altitude;
	lightM.m43 = segmentCenter.z - t * lightM.zv.z;

	GXSetMat4Inverse ( lightV, lightM );

	GXFloat zfar = GetBestFarPlaneDistance ();
	GXSetMat4Ortho ( lightP, 1, 1, 0.0f, zfar );

	GXMulMat4Mat4 ( lightVP, lightV, lightP );

	UpdateFrustumSegmentAABB ();

	GXFloat	sx = 2.0f / ( segmentAABB.max.x - segmentAABB.min.x );
	GXFloat	sy = 2.0f / ( segmentAABB.max.y - segmentAABB.min.y );
	GXFloat ox = -0.5f * ( segmentAABB.max.x + segmentAABB.min.x ) * sx;
	GXFloat oy = -0.5f * ( segmentAABB.max.y + segmentAABB.min.y ) * sy;

	lightC.m11 = sx;
	lightC.m22 = sy;
	lightC.m33 = 1.0f;
	lightC.m44 = 1.0f;

	lightC.m41 = ox;
	lightC.m42 = oy;
	lightC.m43 = 0.0f;

	lightC.m12 = lightC.m13 = lightC.m14 = 0.0f;
	lightC.m21 = lightC.m23 = lightC.m24 = 0.0f;
	lightC.m31 = lightC.m32 = lightC.m34 = 0.0f;

	GXMulMat4Mat4 ( lightVPC[ level ], lightVP, lightC );
}

GXFloat TSCascadeShadowmap::GetBestFarPlaneDistance ()
{
	//Find max z in light's view space

	GXFloat maxZ = lightV.m13 * segmentWorld.v[ 0 ] + lightV.m23 * segmentWorld.v[ 1 ] + lightV.m33 * segmentWorld.v[ 2 ] + lightV.m43;
	for ( GXUChar i = 3; i < 24; i += 3 )
	{
		GXFloat z = lightV.m13 * segmentWorld.v[ i ] + lightV.m23 * segmentWorld.v[ i + 1 ] + lightV.m33 * segmentWorld.v[ i + 2 ] + lightV.m43;
		if ( maxZ < z ) maxZ = z;
	}

	return maxZ + 1000.0f;
}