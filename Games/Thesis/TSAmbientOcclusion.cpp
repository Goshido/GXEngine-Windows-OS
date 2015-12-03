//version 1.1

#include "TSAmbientOcclusion.h"
#include <GXEngine/GXGlobals.h>
#include <GXEngine/GXVAOStorage.h>
#include <GXEngine/GXShaderStorage.h>
#include <GXEngine/GXShaderUtils.h>
#include <GXEngine/GXTextureUtils.h>
#include <GXCommon/GXMTRLoader.h>
#include <time.h>


#define TS_SAMPLES				16
#define TS_NOISE_RESOLUTION		4


TSAmbientOcclusion::TSAmbientOcclusion ()
{
	InitKernel ();
	InitNoiseTexture ();
	InitScreenQuad ();
	InitFBO ();
	InitShaders ();

	SetKernelRadius ( 0.5f );
	SetMaxDistance ( 200.0f );
}

TSAmbientOcclusion::~TSAmbientOcclusion ()
{
	free ( kernel );

	GXRemoveVAO ( screenQuad );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0 );
	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glDeleteTextures ( 1, &texture1 );
	glDeleteTextures ( 1, &texture2 );

	glDeleteTextures ( 1, &noiseTexture );

	GXRemoveShaderProgram ( material.shaders[ 0 ] );
	GXRemoveShaderProgram ( material.shaders[ 1 ] );
	GXRemoveShaderProgram ( material.shaders[ 2 ] );

	material.Cleanup ();
}

GLuint TSAmbientOcclusion::GetAmbientOcclusion ( GLuint depthTexture, GLuint normalTexture )
{
	glDisable ( GL_STENCIL_TEST );
	glDisable ( GL_BLEND );
	glDisable ( GL_CULL_FACE );
	glDepthMask ( GL_FALSE );

	glViewport ( 0, 0, gx_EngineSettings.rendererWidth, gx_EngineSettings.rendererHeight );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture1, 0 );
	static const GLenum buf = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buf );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"TSAmbientOcclusion::GetAmbientOcclusion::Error - Что-то с fbo (1)\n" );
		GXDebugBox ( L"TSAmbientOcclusion::GetAmbientOcclusion::Error - Что-то с fbo (1)" );
	}

	glUseProgram ( material.shaders[ 2 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, depthTexture );
	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, normalTexture );
	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, noiseTexture );

	GXMat4 invProj;
	const GXMat4& projMat = gx_ActiveCamera->GetProjectionMatrix ();
	GXSetMat4Inverse ( invProj, projMat );

	GXVec2 ns;
	ns.x = gx_EngineSettings.rendererWidth / (GXFloat)TS_NOISE_RESOLUTION;
	ns.y = gx_EngineSettings.rendererHeight / (GXFloat)TS_NOISE_RESOLUTION;

	glUniformMatrix4fv ( inv_proj_matLocation, 1, GL_FALSE, invProj.A );
	glUniformMatrix4fv ( proj_matLocation, 1, GL_FALSE, projMat.A );
	glUniform3fv ( kernelLocation, TS_SAMPLES, kernel->v );
	glUniform2fv ( noiseScaleLocation, 1, ns._v );
	glUniform1fv ( kernelRadiusLocation, 1, &kernelRadius );
	glUniform1fv ( maxDistanceLocation, 1, &maxDistance );

	glBindVertexArray ( screenQuad.vao );
	glDrawArrays ( GL_TRIANGLES, 0, screenQuad.numVertices );

	//return texture1;

	glUseProgram ( material.shaders[ 0 ].uiId );

	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture2, 0 );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"TSAmbientOcclusion::GetAmbientOcclusion::Error - Что-то с fbo (2)\n" );
		GXDebugBox ( L"TSAmbientOcclusion::GetAmbientOcclusion::Error - Что-то с fbo (2)" );
	}

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, texture1 );

	glDrawArrays ( GL_TRIANGLES, 0, screenQuad.numVertices );

	glUseProgram ( material.shaders[ 1 ].uiId );

	glFramebufferTexture ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture1, 0 );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"TSAmbientOcclusion::GetAmbientOcclusion::Error - Что-то с fbo (3)\n" );
		GXDebugBox ( L"TSAmbientOcclusion::GetAmbientOcclusion::Error - Что-то с fbo (3)" );
	}

	glBindTexture ( GL_TEXTURE_2D, texture2 );

	glDrawArrays ( GL_TRIANGLES, 0, screenQuad.numVertices );

	return texture1;
}

GXVoid TSAmbientOcclusion::SetKernelRadius ( GXFloat kernelRadius )
{
	this->kernelRadius = kernelRadius;
}

GXVoid TSAmbientOcclusion::SetMaxDistance ( GXFloat maxDistance )
{
	this->maxDistance = maxDistance;
}

GXVoid TSAmbientOcclusion::InitKernel ()
{
	kernel = (GXVec3*)malloc ( TS_SAMPLES * sizeof ( GXVec3 ) );

	GXFloat factor = 1.0f / (GXFloat)TS_SAMPLES;
	srand ( (GXUInt)time ( 0 ) );

	for ( GXUShort i = 0; i < TS_SAMPLES; i++ )
	{
		GXFloat scale = i * factor;
		scale *= scale;

		GXVec3 v;
		v.x = -1.0f + 2.0f * GXRandomNormalize ();
		v.y = -1.0f + 2.0f * GXRandomNormalize ();
		v.z = GXRandomNormalize ();
		GXNormalizeVec3 ( v );

		scale = 0.1f + scale * 0.9f;

		kernel[ i ].x = scale * v.x;
		kernel[ i ].y = scale * v.y;
		kernel[ i ].z = scale * v.z;
	}
}

GXVoid TSAmbientOcclusion::InitNoiseTexture ()
{
	GXUInt pixels = TS_NOISE_RESOLUTION * TS_NOISE_RESOLUTION;
	GXUChar* n = (GXUChar*)malloc ( pixels * 3 * sizeof ( GXUChar ) );

	for ( GXUInt i = 0; i < pixels; i++ )
	{
		GXVec3 v;
		v.x = -1.0f + 2.0f * GXRandomNormalize ();
		v.y = -1.0f + 2.0f * GXRandomNormalize ();
		v.z = 0.0f;

		GXNormalizeVec3 ( v );

		n[ 3 * i ] = (GXUChar)( 128 + 128 * v.x );
		n[ 3 * i + 1 ] = (GXUChar)( 128 + 128 * v.y );
		n[ 3 * i + 2 ] = 128;
	}

	GXGLTextureStruct ts;
	ts.data = n;
	ts.width = ts.height = TS_NOISE_RESOLUTION;
	ts.format = GL_RGB;
	ts.internalFormat = GL_RGB8;
	ts.type = GL_UNSIGNED_BYTE;
	ts.wrap = GL_REPEAT;

	noiseTexture = GXCreateTexture ( ts );

	free ( n );
}

GXVoid TSAmbientOcclusion::InitScreenQuad ()
{
	GXGetVAOFromOBJ ( screenQuad, L"../3D Models/System/ScreenQuad.obj", L"../3D Models/System/Cache/ScreenQuad.cache" );
}

GXVoid TSAmbientOcclusion::InitFBO ()
{
	glGenFramebuffers ( 1, &fbo );

	GXGLTextureStruct ts;
	ts.width = gx_EngineSettings.rendererWidth;
	ts.height = gx_EngineSettings.rendererHeight;
	ts.format = GL_RED;
	ts.internalFormat = GL_R8;
	ts.type = GL_UNSIGNED_BYTE;

	texture1 = GXCreateTexture ( ts );
	texture2 = GXCreateTexture ( ts );
}

GXVoid TSAmbientOcclusion::InitShaders ()
{
	GXLoadMTR ( L"../Materials/Thesis/SSAO.mtr", material );

	GXGetShaderProgram ( material.shaders[ 0 ] );
	GXGetShaderProgram ( material.shaders[ 1 ] );
	GXGetShaderProgram ( material.shaders[ 2 ] );

	InitUniforms ();
}

GXVoid TSAmbientOcclusion::InitUniforms ()
{
	glUseProgram ( material.shaders[ 0 ].uiId );
	glUniform1i ( GXGetUniformLocation ( material.shaders[ 0 ].uiId, "sampler" ), 0 );

	glUseProgram ( material.shaders[ 1 ].uiId );
	glUniform1i ( GXGetUniformLocation ( material.shaders[ 1 ].uiId, "sampler" ), 0 );

	glUseProgram ( material.shaders[ 2 ].uiId );
	glUniform1i ( GXGetUniformLocation ( material.shaders[ 2 ].uiId, "depthSampler" ), 0 );
	glUniform1i ( GXGetUniformLocation ( material.shaders[ 2 ].uiId, "normalSampler" ), 1 );
	glUniform1i ( GXGetUniformLocation ( material.shaders[ 2 ].uiId, "noiseSampler" ), 2 );
	kernelLocation = GXGetUniformLocation ( material.shaders[ 2 ].uiId, "kernel" );
	noiseScaleLocation = GXGetUniformLocation ( material.shaders[ 2 ].uiId, "noiseScale" );
	kernelRadiusLocation = GXGetUniformLocation ( material.shaders[ 2 ].uiId, "kernelRadius" );
	maxDistanceLocation = GXGetUniformLocation ( material.shaders[ 2 ].uiId, "maxDistance" );
	inv_proj_matLocation = GXGetUniformLocation ( material.shaders[ 2 ].uiId, "inv_proj_mat" );
	proj_matLocation = GXGetUniformLocation ( material.shaders[ 2 ].uiId, "proj_mat" );
}