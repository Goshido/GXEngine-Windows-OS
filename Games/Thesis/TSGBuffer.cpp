//version 1.8

#include <GXEngine/GXGlobals.h>
#include "TSGBuffer.h"

#define TS_GBUFFER_EXPOSURE_TIME				1.0f
#define TS_GBUFFER_INV_EXPOSURE_TIME			1.0f

//------------------------------------------------------------------------------------------------------

TSGBuffer::TSGBuffer ( GXCamera* observerCamera )
{
	SetObserverCamera ( observerCamera );

	exposureTimer = TS_GBUFFER_EXPOSURE_TIME;
	exposureInit = GX_TRUE;
	needUpdateLum = GX_TRUE;

	InitFBOs ();
	InitShaders ();
	
	GXGetVAOFromOBJ ( screenQuadVAO, L"../3D Models/System/ScreenQuad.obj", L"../3D Models/System/Cache/ScreenQuad.cache" );

	ts_light_Shadowmap = new TSShadowmap ();
	ts_light_CascadeShadowmap = new TSCascadeShadowmap ( 4096, 300.0f, 1200.0f );
	bloom = new TSBloomEffect ( gx_EngineSettings.rendererWidth, gx_EngineSettings.rendererHeight, hdrOutTexture );

	ambientOcclusion = new TSAmbientOcclusion ();

	currentPass = TS_COMMON;
}

TSGBuffer::~TSGBuffer ()
{
	delete ts_light_Shadowmap;
	delete ts_light_CascadeShadowmap;
	delete bloom;
	delete ambientOcclusion;

	GXRemoveVAO ( screenQuadVAO );
	GXRemoveShaderProgram ( matInfo.shaders[ 0 ] );
	GXRemoveShaderProgram ( matInfo.shaders[ 1 ] );
	GXRemoveShaderProgram ( matInfo.shaders[ 2 ] );
	GXRemoveShaderProgram ( matInfo.shaders[ 3 ] );
	GXRemoveShaderProgram ( matInfo.shaders[ 4 ] );
	GXRemoveShaderProgram ( matInfo.shaders[ 5 ] );
	GXRemoveShaderProgram ( matInfo.shaders[ 6 ] );
	GXRemoveShaderProgram ( matInfo.shaders[ 7 ] );
	GXRemoveShaderProgram ( matInfo.shaders[ 8 ] );

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glDeleteTextures ( 1, &diffuseTexture );
	glDeleteTextures ( 1, &normalTexture );
	glDeleteTextures ( 1, &emitTexture );
	glDeleteTextures ( 1, &specColorTexture );
	glDeleteTextures ( 1, &specPowMetalIntensTexture );
	glDeleteTextures ( 1, &objectTexture );
	glDeleteTextures ( 1, &depthTexture );
	glDeleteTextures ( 1, &lightUpTexture );
	glDeleteTextures ( 1, &hdrOutTexture );
	glDeleteTextures ( 1, &outTexture );

	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glDeleteFramebuffers ( 1, &fbo );

	glDeleteTextures ( 1, &resampleTexture[ 0 ] );
	glDeleteTextures ( 1, &resampleTexture[ 1 ] );
	glDeleteFramebuffers ( 1, &resampleFBO );
}

GLuint TSGBuffer::GetFrameBuffer ()
{
	return fbo;
}

GLuint TSGBuffer::GetColorTexture ()
{
	return diffuseTexture;
}

GLuint TSGBuffer::GetDepthTexture ()
{
	return depthTexture;
}

GLuint TSGBuffer::GetObjectTexture ()
{
	return objectTexture;
}

GXVoid TSGBuffer::SetObserverCamera ( GXCamera* observerCamera )
{
	if ( !observerCamera )
		GXLogW ( L"TSGBuffer::SetObserverCamera::Error - Не установлена камера наблюдателя\n" );

	this->observerCamera = observerCamera;
	const GXMat4* temp;
	temp = observerCamera->GetProjectionMatrixPtr ();
	GXSetMat4Inverse ( observer_inv_proj_mat, *temp );

	observer_view_mat = observerCamera->GetViewMatrixPtr ();
}

GLuint ambientTexture;

GXVoid TSGBuffer::ShowTexture ( eTSGBufferSlot slot )
{
	glDisable ( GL_BLEND );

	switch ( slot )
	{
		case TS_DIFFUSE:
			textureOuter.SetTexture ( diffuseTexture );
		break;

		case TS_NORMALS:
			textureOuter.SetTexture ( normalTexture );
		break;

		case TS_DEPTH:
			textureOuter.SetTexture ( depthTexture );
		break;

		case TS_LIGHTUP:
			textureOuter.SetTexture ( lightUpTexture );
		break;

		case TS_EMISSION:
			textureOuter.SetTexture ( emitTexture );
		break;

		case TS_SPEC_COLOR:
			textureOuter.SetTexture ( specColorTexture );
		break;

		case TS_SPEC_PARAMS:
			textureOuter.SetTexture ( specPowMetalIntensTexture );
		break;

		case TS_OUT:
			textureOuter.SetTexture ( outTexture );
			//textureOuter.SetTexture ( ambientTexture );
		break;
	}

	textureOuter.Draw ();
}

GXVoid TSGBuffer::ClearGBuffer ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, diffuseTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, emitTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, specColorTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, specPowMetalIntensTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0 );

	static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers ( 5, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"TSGBuffer::ClearGBuffer::Error - Что-то с fbo\n" );
		GXDebugBox ( L"TSGBuffer::ClearGBuffer::Error - Что-то с fbo" );
	}

	glEnable ( GL_DEPTH_TEST );

	glViewport ( 0, 0, gx_EngineSettings.rendererWidth, gx_EngineSettings.rendererHeight );
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

GXVoid TSGBuffer::StartSkyboxPass ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, emitTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0 );

	static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"TSGBuffer::StartSkyboxPass::Error - Что-то с fbo\n" );
		GXDebugBox ( L"TSGBuffer::StartSkyboxPass::Error - Что-то с fbo" );
	}

	glViewport ( 0, 0, gx_EngineSettings.rendererWidth, gx_EngineSettings.rendererHeight );

	currentPass = TS_SKYBOX;
}

GXVoid TSGBuffer::StartCommonPass ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, diffuseTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, emitTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, specColorTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, specPowMetalIntensTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0 );

	static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers ( 5, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"TSGBuffer::StartCommonPass::Error - Что-то с fbo\n" );
		GXDebugBox ( L"TSGBuffer::StartCommonPass::Error - Что-то с fbo" );
	}

	glDisable ( GL_BLEND );
	glEnable ( GL_CULL_FACE );
	glEnable ( GL_DEPTH_TEST );

	glViewport ( 0, 0, gx_EngineSettings.rendererWidth, gx_EngineSettings.rendererHeight );

	currentPass = TS_COMMON;
}

GXVoid TSGBuffer::StartLightUpPass ()
{
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightUpTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0 );

	glEnable ( GL_STENCIL_TEST );
	glDisable ( GL_DEPTH_TEST );
	glDepthMask ( GL_FALSE );

	static const GLenum buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );
	
	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"TSGBuffer::StartAlbedoPass::Error - Что-то с fbo\n" );
		GXDebugBox ( L"TSGBuffer::StartAlbedoPass::Error - Что-то с fbo" );
	}

	//glEnable ( GL_BLEND );
	glBlendEquation ( GL_FUNC_ADD );
	glBlendFunc ( GL_ONE, GL_ONE );

	glCullFace ( GL_BACK );

	glViewport ( 0, 0, gx_EngineSettings.rendererWidth, gx_EngineSettings.rendererHeight );
	glClear ( GL_COLOR_BUFFER_BIT );

	currentPass = TS_ALBEDO;
}

GXVoid TSGBuffer::StartHudDepthDependentPass ()
{
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0 );

	glEnable ( GL_CULL_FACE );
	glCullFace ( GL_FRONT );

	glEnable ( GL_DEPTH_TEST );

	currentPass = TS_HUD_DEPTH_DEPENDENT;
}

GXVoid TSGBuffer::StartHudDepthIndependentPass ()
{
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE );

	glDepthMask ( GL_TRUE );

	glClear ( GL_DEPTH_BUFFER_BIT );

	currentPass = TS_HUD_DEPTH_INDEPENDENT;
}

GXVoid TSGBuffer::LightBy ( TSLightEmitter* emitter )
{
	if ( !emitter )
	{
		GXLogW ( L"TSGBuffer::LightBy::Error - Попытка использовать нулевай указатель\n" );
		return;
	}

	if ( currentPass != TS_ALBEDO )
	{
		GXLogW ( L"TSGBuffer::LightBy::Error - Вызов функции минуя проход расчёта альбедо\n" );
		return;
	}

	eTSLightEmitterType type = emitter->GetType ();
	if ( type == TS_UNKNOWN )
	{
		GXLogW ( L"TSGBuffer::LightBy::Error - Попытка использовать базовый источник света\n" );
		return;
	}

	if ( type == TS_DIRECTED )
	{
		glCullFace ( GL_FRONT );
		glDisable ( GL_STENCIL_TEST );

		glDisable ( GL_DEPTH_TEST );
		LightByDirectedLight ( (TSDirectedLight*)emitter );
		glCullFace ( GL_BACK );
		glEnable ( GL_STENCIL_TEST );
		return;
	}

	MarkLightVolume ( emitter );

	static const GLenum buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	glDisable ( GL_DEPTH_TEST );
	glEnable ( GL_BLEND );

	glEnable ( GL_CULL_FACE );
	glStencilFunc ( GL_NOTEQUAL, 0, 0xFF );

	switch ( type )
	{
		case TS_BULP:
			LightByBulp ( (TSBulp*)emitter );
		break;

		case TS_SPOT:
			LightBySpotlight ( (TSSpotlight*)emitter );
		break;
	}
}

GXVoid TSGBuffer::CombineSlots ()
{
	ambientTexture = ambientOcclusion->GetAmbientOcclusion ( depthTexture, normalTexture );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrOutTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0 );

	static const GLenum buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"TSGBuffer::StartComposePass::Error - Что-то с fbo\n" );
		GXDebugBox ( L"TSGBuffer::StartComposePass::Error - Что-то с fbo" );
	}

	glDisable ( GL_STENCIL_TEST );
	glDisable ( GL_BLEND );
	glDisable ( GL_CULL_FACE );

	glViewport ( 0, 0, gx_EngineSettings.rendererWidth, gx_EngineSettings.rendererHeight );
	glClear ( GL_COLOR_BUFFER_BIT );

	currentPass = TS_COMPOSE;

	if ( currentPass != TS_COMPOSE )
	{
		GXLogW ( L"TSGBuffer::CombineSlots::Error - Попытра собрать GBuffer не выбраз завершающий проход\n" );
		return;
	}

	glUseProgram ( matInfo.shaders[ 0 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, diffuseTexture );

	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, lightUpTexture );

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, emitTexture );

	glActiveTexture ( GL_TEXTURE3 );
	glBindTexture ( GL_TEXTURE_2D, specColorTexture );

	glActiveTexture ( GL_TEXTURE4 );
	glBindTexture ( GL_TEXTURE_2D, specPowMetalIntensTexture );

	glActiveTexture ( GL_TEXTURE5 );
	glBindTexture ( GL_TEXTURE_2D, ambientTexture );

	glBindVertexArray ( screenQuadVAO.vao );
	glDrawArrays ( GL_TRIANGLES, 0, screenQuadVAO.numVertices );
}

GXVoid TSGBuffer::AddBloom ()
{
	static const GLenum buffer = GL_COLOR_ATTACHMENT0;

	if ( exposureInit )
	{
		glBindFramebuffer ( GL_FRAMEBUFFER, resampleFBO );
		glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resampleTexture[ activeResampleTexture ], 0 );
		glDrawBuffers ( 1, &buffer );

		glViewport ( 0, 0, resampleX, resampleY );

		glUseProgram ( matInfo.shaders[ 3 ].uiId );
		glActiveTexture ( GL_TEXTURE0 );
		glBindTexture ( GL_TEXTURE_2D, hdrOutTexture );

		glDrawArrays ( GL_TRIANGLES, 0, screenQuadVAO.numVertices );

		glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
		glGenerateMipmap ( GL_TEXTURE_2D );

		bloom->SetAverageTexture ( resampleTexture[ activeResampleTexture ] );

		exposureInit = GX_FALSE;
	}

	if ( needUpdateLum )
	{
		activeResampleTexture = ( activeResampleTexture == 0 ) ? 1 : 0;

		glBindFramebuffer ( GL_FRAMEBUFFER, resampleFBO );
		glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resampleTexture[ activeResampleTexture ], 0 );
		glDrawBuffers ( 1, &buffer );

		glViewport ( 0, 0, resampleX, resampleY );

		glUseProgram ( matInfo.shaders[ 3 ].uiId );
		glActiveTexture ( GL_TEXTURE0 );
		glBindTexture ( GL_TEXTURE_2D, hdrOutTexture );

		glDrawArrays ( GL_TRIANGLES, 0, screenQuadVAO.numVertices );

		glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
		glGenerateMipmap ( GL_TEXTURE_2D );

		bloom->SetAverageTexture ( resampleTexture[ activeResampleTexture ] );
		
		needUpdateLum = GX_FALSE;
	}

	bloom->Draw ();

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outTexture, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, 0, 0 );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, 0, 0 );

	static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers ( 1, buffers );

	GLenum status = glCheckFramebufferStatus ( GL_FRAMEBUFFER );
	if ( status != GL_FRAMEBUFFER_COMPLETE )
	{
		GXLogW ( L"TSGBuffer::StartComposePass::Error - Что-то с fbo\n" );
		GXDebugBox ( L"TSGBuffer::StartComposePass::Error - Что-то с fbo" );
	}

	glViewport ( 0, 0, gx_EngineSettings.rendererWidth, gx_EngineSettings.rendererHeight );
	glClear ( GL_COLOR_BUFFER_BIT );

	currentPass = TS_BLOOM;

	GLuint bloomTexture = bloom->GetBloomTexture ();

	glUseProgram ( matInfo.shaders[ 2 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, hdrOutTexture );

	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, bloomTexture );

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, resampleTexture[ activeResampleTexture ] );

	glActiveTexture ( GL_TEXTURE3 );
	glBindTexture ( GL_TEXTURE_2D, resampleTexture[ activeResampleTexture ? 0 : 1 ] );

	GXFloat lerpFactor = cosf ( exposureTimer * TS_GBUFFER_INV_EXPOSURE_TIME * GX_MATH_HALFPI );

	glUniform1fv ( tone_lerpFactorLocation, 1, &lerpFactor );

	glBindVertexArray ( screenQuadVAO.vao );
	glDrawArrays ( GL_TRIANGLES, 0, screenQuadVAO.numVertices );
}

GXVoid TSGBuffer::UpdateExposure ( GXFloat delta )
{
	exposureDelta = delta;
	if ( exposureTimer <= delta )
	{
		exposureTimer = TS_GBUFFER_EXPOSURE_TIME;
		needUpdateLum = GX_TRUE;
	}
	else
		exposureTimer -= delta;
}

GXVoid TSGBuffer::InitFBOs ()
{
	invScreenResolution.x = 1.0f / (GXFloat)gx_EngineSettings.rendererWidth;
	invScreenResolution.y = 1.0f / (GXFloat)gx_EngineSettings.rendererHeight;

	GXGLTextureStruct ts;
	ts.data = 0;
	ts.format = GL_RGBA;
	ts.internalFormat = GL_RGBA8;
	ts.type = GL_UNSIGNED_BYTE;
	ts.wrap = GL_CLAMP_TO_EDGE;
	ts.width = gx_EngineSettings.rendererWidth;
	ts.height = gx_EngineSettings.rendererHeight;

	diffuseTexture = GXCreateTexture ( ts );
	outTexture = GXCreateTexture ( ts );

	ts.internalFormat = GL_RGBA16F;
	ts.type = GL_HALF_FLOAT;

	emitTexture = GXCreateTexture ( ts );
	lightUpTexture = GXCreateTexture ( ts );
	hdrOutTexture = GXCreateTexture ( ts );

	ts.format = GL_RGB;
	ts.internalFormat = GL_RGB8;
	ts.type = GL_UNSIGNED_BYTE;

	specColorTexture = GXCreateTexture ( ts );
	specPowMetalIntensTexture = GXCreateTexture ( ts );

	ts.format = GL_RGB;
	ts.internalFormat = GL_RGB16;
	ts.type = GL_UNSIGNED_SHORT;

	normalTexture = GXCreateTexture ( ts );

	ts.format = GL_DEPTH_STENCIL;
	ts.internalFormat = GL_DEPTH24_STENCIL8;
	ts.type = GL_UNSIGNED_INT_24_8;

	depthTexture = GXCreateTexture ( ts );

	ts.format = GL_RED_INTEGER;
	ts.internalFormat = GL_R32UI;
	ts.type = GL_UNSIGNED_INT;

	objectTexture = GXCreateTexture ( ts );

	glGenFramebuffers ( 1, &fbo );

	ts.width = resampleX = gx_EngineSettings.potWidth;
	ts.height = resampleY = gx_EngineSettings.potHeight;
	ts.data = 0;
	ts.format = GL_RGB;
	ts.internalFormat = GL_RGB16F;
	ts.type = GL_HALF_FLOAT;
	ts.wrap = GL_CLAMP_TO_EDGE;
	resampleTexture[ 0 ] = GXCreateTexture ( ts );
	resampleTexture[ 1 ] = GXCreateTexture ( ts );

	activeResampleTexture = 0;

	maxMipmapLevel = 0;
	GXUShort testWidth = gx_EngineSettings.potWidth;
	while ( testWidth != 0 )
	{
		maxMipmapLevel++;
		testWidth >>= 1;
	}

	maxMipmapLevel--;

	glBindTexture ( GL_TEXTURE_2D, resampleTexture[ 0 ] );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
	glGenerateMipmap ( GL_TEXTURE_2D );

	glBindTexture ( GL_TEXTURE_2D, resampleTexture[ 1 ] );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST );
	glGenerateMipmap ( GL_TEXTURE_2D );
	glBindTexture ( GL_TEXTURE_2D, 0 );

	glGenFramebuffers ( 1, &resampleFBO );
	glBindFramebuffer ( GL_FRAMEBUFFER, resampleFBO );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resampleTexture[ 0 ], 0 );
}

GXVoid TSGBuffer::InitShaders ()
{
	GXLoadMTR ( L"../Materials/Thesis/GBuffer.mtr", matInfo );
	GXGetShaderProgram ( matInfo.shaders[ 0 ] );
	GXGetShaderProgram ( matInfo.shaders[ 1 ] );
	GXGetShaderProgram ( matInfo.shaders[ 2 ] );
	GXGetShaderProgram ( matInfo.shaders[ 3 ] );
	GXGetShaderProgram ( matInfo.shaders[ 4 ] );
	GXGetShaderProgram ( matInfo.shaders[ 5 ] );
	GXGetShaderProgram ( matInfo.shaders[ 6 ] );
	GXGetShaderProgram ( matInfo.shaders[ 7 ] );
	GXGetShaderProgram ( matInfo.shaders[ 8 ] );

	glUseProgram ( matInfo.shaders[ 0 ].uiId );

	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 0 ].uiId, "diffuseSampler" ), 0 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 0 ].uiId, "lightUpSampler" ), 1 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 0 ].uiId, "emitSampler" ), 2 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 0 ].uiId, "specColorSampler" ), 3 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 0 ].uiId, "specPowMetalIntensSampler" ), 4 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 0 ].uiId, "ssaoSampler" ), 5 );

	glUseProgram ( matInfo.shaders[ 1 ].uiId );

	bulpUniforms.viewBulpPosition = GXGetUniformLocation ( matInfo.shaders[ 1 ].uiId, "viewBulpPosition" );
	bulpUniforms.invQuadMaxDistance = GXGetUniformLocation ( matInfo.shaders[ 1 ].uiId, "invQuadMaxDistance" );
	bulpUniforms.inv_proj_mat = GXGetUniformLocation ( matInfo.shaders[ 1 ].uiId, "inv_proj_mat" );
	bulpUniforms.hdrColorAndIntensity = GXGetUniformLocation ( matInfo.shaders[ 1 ].uiId, "hdrColorAndIntensity" );
	bulpUniforms.mod_view_proj_mat = GXGetUniformLocation ( matInfo.shaders[ 1 ].uiId, "mod_view_proj_mat" );
	bulpUniforms.invScreenResolution = GXGetUniformLocation ( matInfo.shaders[ 1 ].uiId, "invScreenResolution" );

	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 1 ].uiId, "normalTexture" ), 0 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 1 ].uiId, "depthTexture" ), 1 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 1 ].uiId, "specPowIntensTexture" ), 2 );

	glUseProgram ( matInfo.shaders[ 2 ].uiId );

	tone_lerpFactorLocation = GXGetUniformLocation ( matInfo.shaders[ 2 ].uiId, "lerpFactor" );

	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 2 ].uiId, "hdrTexture" ), 0 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 2 ].uiId, "bloomTexture" ), 1 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 2 ].uiId, "currentLumTexture" ), 2 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 2 ].uiId, "oldLumTexture" ), 3 );
	
	glUseProgram ( matInfo.shaders[ 3 ].uiId );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 3 ].uiId, "inTexture" ), 0 );

	glUseProgram ( matInfo.shaders[ 4 ].uiId );

	spotShadowUniforms.viewSpotLampPosition = GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "viewSpotLampPosition" );
	spotShadowUniforms.viewSpotLampDirection = GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "viewSpotLampDirection" );
	spotShadowUniforms.invQuadMaxDistance = GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "invQuadMaxDistance" );
	spotShadowUniforms.cutoffCosine = GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "cutoffCosine" );
	spotShadowUniforms.inv_proj_mat = GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "inv_proj_mat" );
	spotShadowUniforms.hdrColorAndIntensity = GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "hdrColorAndIntensity" );
	spotShadowUniforms.inv_view_mat = GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "inv_view_mat" );
	spotShadowUniforms.lamp_view_proj_mat = GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "lamp_view_proj_mat" );
	spotShadowUniforms.mod_view_proj_mat = GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "mod_view_proj_mat" );
	spotShadowUniforms.invScreenResolution = GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "invScreenResolution" );

	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "normalTexture" ), 0 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "depthTexture" ), 1 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "specPowIntensTexture" ), 2 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 4 ].uiId, "shadowmapTexture" ), 3 );

	glUseProgram ( matInfo.shaders[ 5 ].uiId );

	spotUniforms.viewSpotLampPosition = GXGetUniformLocation ( matInfo.shaders[ 5 ].uiId, "viewSpotLampPosition" );
	spotUniforms.viewSpotLampDirection = GXGetUniformLocation ( matInfo.shaders[ 5 ].uiId, "viewSpotLampDirection" );
	spotUniforms.invQuadMaxDistance = GXGetUniformLocation ( matInfo.shaders[ 5 ].uiId, "invQuadMaxDistance" );
	spotUniforms.cutoffCosine = GXGetUniformLocation ( matInfo.shaders[ 5 ].uiId, "cutoffCosine" );
	spotUniforms.inv_proj_mat = GXGetUniformLocation ( matInfo.shaders[ 5 ].uiId, "inv_proj_mat" );
	spotUniforms.hdrColorAndIntensity = GXGetUniformLocation ( matInfo.shaders[ 5 ].uiId, "hdrColorAndIntensity" );
	spotUniforms.mod_view_proj_mat = GXGetUniformLocation ( matInfo.shaders[ 5 ].uiId, "mod_view_proj_mat" );
	spotUniforms.invScreenResolution = GXGetUniformLocation ( matInfo.shaders[ 5 ].uiId, "invScreenResolution" );

	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 5 ].uiId, "normalTexture" ), 0 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 5 ].uiId, "depthTexture" ), 1 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 5 ].uiId, "specPowIntensTexture" ), 2 );

	light_vol_mod_view_proj_matLocation = GXGetUniformLocation ( matInfo.shaders[ 6 ].uiId, "mod_view_proj_mat" );

	glUseProgram ( matInfo.shaders[ 7 ].uiId );

	directedUniforms.viewLightDirection = GXGetUniformLocation ( matInfo.shaders[ 7 ].uiId, "viewLightDirection" );
	directedUniforms.inv_proj_mat = GXGetUniformLocation ( matInfo.shaders[ 7 ].uiId, "inv_proj_mat" );
	directedUniforms.hdrColorAndIntensity = GXGetUniformLocation ( matInfo.shaders[ 7 ].uiId, "hdrColorAndIntensity" );
	directedUniforms.hdrAmbientColorAndIntensity = GXGetUniformLocation ( matInfo.shaders[ 7 ].uiId, "hdrAmbientColorAndIntensity" );

	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 7 ].uiId, "normalTexture" ), 0 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 7 ].uiId, "depthTexture" ), 1 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 7 ].uiId, "specPowIntensTexture" ), 2 );

	glUseProgram ( matInfo.shaders[ 8 ].uiId );

	directedShadowUniforms.hdrAmbientColorAndIntensity = GXGetUniformLocation ( matInfo.shaders[ 8 ].uiId, "hdrAmbientColorAndIntensity" );
	directedShadowUniforms.hdrColorAndIntensity = GXGetUniformLocation ( matInfo.shaders[ 8 ].uiId, "hdrColorAndIntensity" );
	directedShadowUniforms.inv_proj_mat = GXGetUniformLocation ( matInfo.shaders[ 8 ].uiId, "inv_proj_mat" );
	directedShadowUniforms.inv_view_mat = GXGetUniformLocation ( matInfo.shaders[ 8 ].uiId, "inv_view_mat" );
	directedShadowUniforms.light_view_proj_crop_bias_mat = GXGetUniformLocation ( matInfo.shaders[ 8 ].uiId, "light_view_proj_crop_bias_mat" );
	directedShadowUniforms.viewFrustumSplits = GXGetUniformLocation ( matInfo.shaders[ 8 ].uiId, "viewFrustumSplits" );
	directedShadowUniforms.viewLightDirection = GXGetUniformLocation ( matInfo.shaders[ 8 ].uiId, "viewLightDirection" );

	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 8 ].uiId, "normalSampler" ), 0 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 8 ].uiId, "depthSampler" ), 1 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 8 ].uiId, "specPowIntensSampler" ), 2 );
	glUniform1i ( GXGetUniformLocation ( matInfo.shaders[ 8 ].uiId, "csmSampler" ), 3 );

 	glUseProgram ( 0 );
}

GXVoid TSGBuffer::LightByBulp ( TSBulp* bulp )
{
	glUseProgram ( matInfo.shaders[ 1 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, normalTexture );

	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, depthTexture );

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, specPowMetalIntensTexture );

	glUniformMatrix4fv ( bulpUniforms.inv_proj_mat, 1, GL_FALSE, observer_inv_proj_mat.A );

	GXVec3 worldBulpPosition;
	bulp->GetLocation ( worldBulpPosition );

	GXVec3 viewBulpPosition;
	GXMulVec3Mat4AsPoint ( viewBulpPosition, worldBulpPosition, *observer_view_mat );
	glUniform3fv ( bulpUniforms.viewBulpPosition, 1, viewBulpPosition.v );

	GXFloat invQuadMaxDistance = 1.0f / bulp->GetInfluenceDistance ();
	invQuadMaxDistance *= invQuadMaxDistance;
	glUniform1fv ( bulpUniforms.invQuadMaxDistance, 1, &invQuadMaxDistance );

	GXVec4 bulpHDRColorAndIntensity;
	bulp->GetHDRColorAndIntensity ( bulpHDRColorAndIntensity );
	glUniform4fv ( bulpUniforms.hdrColorAndIntensity, 1, bulpHDRColorAndIntensity.v );
	
	GLuint vao;
	GXInt numVertices;
	GLenum topology;
	bulp->GetLightVolume ( vao, topology, numVertices );

	const GXMat4& mod_mat = bulp->GetModelMatrix ();
	const GXMat4* view_proj_mat = observerCamera->GetViewProjectionMatrixPtr ();

	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUniformMatrix4fv ( bulpUniforms.mod_view_proj_mat, 1, GL_FALSE, mod_view_proj_mat.A );

	glUniform2fv ( bulpUniforms.invScreenResolution, 1, invScreenResolution._v );
	
	glBindVertexArray ( vao );
	glDrawArrays ( topology, 0, numVertices );
}

GXVoid TSGBuffer::LightBySpotlight ( TSSpotlight* spot )
{
	if ( spot->IsCastedShadows () )
		DoSpotWithShadows ( spot );
	else
		DoSpotWithoutShadows ( spot );
}

GXVoid TSGBuffer::LightByDirectedLight ( TSDirectedLight* light )
{
	if ( light->IsCastedShadows () )
		DoDirectedLightWithShadows ( light );
	else
		DoDirectedLightWithoutShadows ( light );
}

GXVoid TSGBuffer::MarkLightVolume ( TSLightEmitter* emitter )
{
	GLuint vao;
	GXInt numVertexes;
	GLenum topology;
	emitter->GetLightVolume ( vao, topology, numVertexes );
	
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0 );
	glDrawBuffer ( GL_NONE );
	
	glDisable ( GL_CULL_FACE );
	glDisable ( GL_BLEND );
	glEnable ( GL_DEPTH_TEST );

	glStencilMask ( 0xFF );
	glClearStencil ( 0 );
	glClear ( GL_STENCIL_BUFFER_BIT );

	glStencilFunc ( GL_ALWAYS, 0, 0xFF );
	glStencilOpSeparate ( GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP );
	glStencilOpSeparate ( GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP );
	
	const GXMat4& mod_mat = emitter->GetModelMatrix ();
	const GXMat4* view_proj_mat = observerCamera->GetViewProjectionMatrixPtr ();

	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUseProgram ( matInfo.shaders[ 6 ].uiId );
	glUniformMatrix4fv ( light_vol_mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );
	
	glBindVertexArray ( vao );
	glDrawArrays ( topology, 0, numVertexes );
}

GXVoid TSGBuffer::DoSpotWithShadows ( TSSpotlight* spot )
{
	GLuint shadowmap;

	ts_light_Shadowmap->StartShadowmapPass ();
	shadowmap = ts_light_Shadowmap->GetShadowmapFrom ( spot );
	ts_light_Shadowmap->FinishShadowmapPass ();

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );

	static const GLenum buffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buffer );

	glViewport ( 0, 0, gx_EngineSettings.rendererWidth, gx_EngineSettings.rendererHeight );
	glUseProgram ( matInfo.shaders[ 4 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, normalTexture );

	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, depthTexture );

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, specPowMetalIntensTexture );

	glActiveTexture ( GL_TEXTURE3 );
	glBindTexture ( GL_TEXTURE_2D, shadowmap );

	glUniformMatrix4fv ( spotShadowUniforms.inv_proj_mat, 1, GL_FALSE, observer_inv_proj_mat.A );

	GXVec3 worldSpotLampPosition;
	spot->GetLocation ( worldSpotLampPosition );

	GXVec3 viewSpotLampPosition;
	GXMulVec3Mat4AsPoint ( viewSpotLampPosition, worldSpotLampPosition, *observer_view_mat );
	glUniform3fv ( spotShadowUniforms.viewSpotLampPosition, 1, viewSpotLampPosition.v );

	GXMat4 rot;
	spot->GetRotation ( rot );
	GXVec3 viewSpotLampDirection;
	GXMulVec3Mat4AsNormal ( viewSpotLampDirection, rot.zv, *observer_view_mat );
	glUniform3fv ( spotShadowUniforms.viewSpotLampDirection, 1, viewSpotLampDirection.v );

	GXFloat invQuadMaxDistance = 1.0f / spot->GetInfluenceDistance ();
	invQuadMaxDistance *= invQuadMaxDistance;
	glUniform1fv ( spotShadowUniforms.invQuadMaxDistance, 1, &invQuadMaxDistance );

	GXFloat cutoffCosine = cosf ( spot->GetConeAngle () * 0.5f );
	glUniform1fv ( spotShadowUniforms.cutoffCosine, 1, &cutoffCosine );

	GXVec4 hdrColorAndIntensity;
	spot->GetHDRColorAndIntensity ( hdrColorAndIntensity );
	glUniform4fv ( spotShadowUniforms.hdrColorAndIntensity, 1, hdrColorAndIntensity.v );

	const GXMat4& spot_view_proj_mat = spot->GetViewProjectionMatrix ();

	GXMat4 bias;
	GXSetMat4Scale ( bias, 0.5f, 0.5f, 0.5f );
	bias.m41 = bias.m42 = bias.m43 = 0.5f;

	GXMat4 shadowmap_mat;
	GXMulMat4Mat4 ( shadowmap_mat, spot_view_proj_mat, bias );

	glUniformMatrix4fv ( spotShadowUniforms.lamp_view_proj_mat, 1, GL_FALSE, shadowmap_mat.A );

	const GXMat4* inv_view_mat = gx_ActiveCamera->GetModelMatrixPtr ();
	glUniformMatrix4fv ( spotShadowUniforms.inv_view_mat, 1, GL_FALSE, inv_view_mat->A );
	
	GLuint vao;
	GXInt numVertices;
	GLenum topology;
	spot->GetLightVolume ( vao, topology, numVertices );

	const GXMat4& mod_mat = spot->GetModelMatrix ();
	const GXMat4* view_proj_mat = observerCamera->GetViewProjectionMatrixPtr ();

	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUniformMatrix4fv ( spotShadowUniforms.mod_view_proj_mat, 1, GL_FALSE, mod_view_proj_mat.A );

	glUniform2fv ( spotShadowUniforms.invScreenResolution, 1, invScreenResolution._v );
	
	glBindVertexArray ( vao );
	glDrawArrays ( topology, 0, numVertices );
}

GXVoid TSGBuffer::DoSpotWithoutShadows ( TSSpotlight* spot )
{
	glUseProgram ( matInfo.shaders[ 5 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, normalTexture );

	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, depthTexture );

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, specPowMetalIntensTexture );

	glUniformMatrix4fv ( spotUniforms.inv_proj_mat, 1, GL_FALSE, observer_inv_proj_mat.A );

	GXVec3 worldSpotLampPosition;
	spot->GetLocation ( worldSpotLampPosition );

	GXVec3 viewSpotLampPosition;
	GXMulVec3Mat4AsPoint ( viewSpotLampPosition, worldSpotLampPosition, *observer_view_mat );
	glUniform3fv ( spotUniforms.viewSpotLampPosition, 1, viewSpotLampPosition.v );

	GXMat4 rot;
	spot->GetRotation ( rot );
	GXVec3 viewSpotLampDirection;
	GXMulVec3Mat4AsNormal ( viewSpotLampDirection, rot.zv, *observer_view_mat );
	glUniform3fv ( spotUniforms.viewSpotLampDirection, 1, viewSpotLampDirection.v );

	GXFloat invQuadMaxDistance = 1.0f / spot->GetInfluenceDistance ();
	invQuadMaxDistance *= invQuadMaxDistance;
	glUniform1fv ( spotUniforms.invQuadMaxDistance, 1, &invQuadMaxDistance );

	GXFloat cutoffCosine = cosf ( spot->GetConeAngle () * 0.5f );
	glUniform1fv ( spotUniforms.cutoffCosine, 1, &cutoffCosine );

	GXVec4 hdrColorAndIntensity;
	spot->GetHDRColorAndIntensity ( hdrColorAndIntensity );
	glUniform4fv ( spotUniforms.hdrColorAndIntensity, 1, hdrColorAndIntensity.v );
	
	GLuint vao;
	GXInt numVertices;
	GLenum topology;
	spot->GetLightVolume ( vao, topology, numVertices );

	const GXMat4& mod_mat = spot->GetModelMatrix ();
	const GXMat4* view_proj_mat = observerCamera->GetViewProjectionMatrixPtr ();

	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUniformMatrix4fv ( spotUniforms.mod_view_proj_mat, 1, GL_FALSE, mod_view_proj_mat.A );

	glUniform2fv ( spotUniforms.invScreenResolution, 1, invScreenResolution._v );
	
	glBindVertexArray ( vao );
	glDrawArrays ( topology, 0, numVertices );
}

GXVoid TSGBuffer::DoDirectedLightWithShadows ( TSDirectedLight* light )
{
	GXCameraPerspective* cam = (GXCameraPerspective*)gx_ActiveCamera;
	glDepthMask ( GL_TRUE );
	glEnable ( GL_DEPTH_TEST );
	//glDisable ( GL_CULL_FACE );

	GLuint csmTexture;
	const GXMat4* lightVPCB;
	const GXFloat* splits;
	ts_light_CascadeShadowmap->MakeShadowmaps ( &lightVPCB, &splits, csmTexture, light, cam );

	//glEnable ( GL_CULL_FACE );
	glDisable ( GL_DEPTH_TEST );
	glDepthMask ( GL_FALSE );

	glUseProgram ( matInfo.shaders[ 8 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, normalTexture );

	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, depthTexture );

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, specPowMetalIntensTexture );

	glActiveTexture ( GL_TEXTURE3 );
	glBindTexture ( GL_TEXTURE_2D_ARRAY, csmTexture );

	glUniform1fv ( directedShadowUniforms.viewFrustumSplits, 4, splits );

	glUniformMatrix4fv ( directedShadowUniforms.light_view_proj_crop_bias_mat, 4, GL_FALSE, lightVPCB->A );
	glUniformMatrix4fv ( directedShadowUniforms.inv_proj_mat, 1, GL_FALSE, observer_inv_proj_mat.A );

	GXMat4 rot_mat;
	light->GetRotation ( rot_mat );
	GXVec3 lightViewDirection;
	GXMulVec3Mat4AsNormal ( lightViewDirection, rot_mat.zv, *observer_view_mat );
	lightViewDirection.x = -lightViewDirection.x;
	lightViewDirection.y = -lightViewDirection.y;
	lightViewDirection.z = -lightViewDirection.z;
	glUniform3fv ( directedShadowUniforms.viewLightDirection, 1, lightViewDirection.v );

	GXVec4 hdrParams;
	light->GetHDRColorAndIntensity ( hdrParams );
	glUniform4fv ( directedShadowUniforms.hdrColorAndIntensity, 1, hdrParams.v );

	light->GetHDRAmbientColorAndIntensity ( hdrParams );
	glUniform4fv ( directedShadowUniforms.hdrAmbientColorAndIntensity, 1, hdrParams.v );

	const GXMat4& inv_view_mat = cam->GetModelMatrix ();
	glUniformMatrix4fv ( directedShadowUniforms.inv_view_mat, 1, GX_FALSE, inv_view_mat.A );
	
	glBindVertexArray ( screenQuadVAO.vao );
	glDrawArrays ( GL_TRIANGLES, 0, screenQuadVAO.numVertices );
}

GXVoid TSGBuffer::DoDirectedLightWithoutShadows ( TSDirectedLight* light )
{
	glUseProgram ( matInfo.shaders[ 7 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, normalTexture );

	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, depthTexture );

	glActiveTexture ( GL_TEXTURE2 );
	glBindTexture ( GL_TEXTURE_2D, specPowMetalIntensTexture );

	glUniformMatrix4fv ( directedUniforms.inv_proj_mat, 1, GL_FALSE, observer_inv_proj_mat.A );

	GXMat4 rot_mat;
	light->GetRotation ( rot_mat );
	GXVec3 lightViewDirection;
	GXMulVec3Mat4AsNormal ( lightViewDirection, rot_mat.zv, *observer_view_mat );
	lightViewDirection.x = -lightViewDirection.x;
	lightViewDirection.y = -lightViewDirection.y;
	lightViewDirection.z = -lightViewDirection.z;
	glUniform3fv ( directedUniforms.viewLightDirection, 1, lightViewDirection.v );

	GXVec4 hdrParams;
	light->GetHDRColorAndIntensity ( hdrParams );
	glUniform4fv ( directedUniforms.hdrColorAndIntensity, 1, hdrParams.v );

	light->GetHDRAmbientColorAndIntensity ( hdrParams );
	glUniform4fv ( directedUniforms.hdrAmbientColorAndIntensity, 1, hdrParams.v );
	
	glBindVertexArray ( screenQuadVAO.vao );
	glDrawArrays ( GL_TRIANGLES, 0, screenQuadVAO.numVertices );
}