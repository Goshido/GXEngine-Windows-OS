//version 1.3

#include "TSBloomEffect.h"


TSBloomEffect::TSBloomEffect ( GXUShort width, GXUShort height, GLuint hdrTexture )
{
	this->hdrTexture = hdrTexture;
	averageLumTexture = 0;

	glGenFramebuffers ( 1, &fbo );

	GXGLTextureStruct ts;
	ts.width = width >> 2;
	ts.height = height >> 2;
	ts.data = 0;
	ts.format = GL_RGB;
	ts.internalFormat = GL_RGB16F;
	ts.type = GL_HALF_FLOAT;
	ts.wrap = GL_CLAMP_TO_EDGE;

	blurXTexture = GXCreateTexture ( ts );
	blurYTexture = GXCreateTexture ( ts );

	resolutionX = ts.width;
	resolutionY = ts.height;

	invResolution.x = 1.0f / (GXFloat)width;
	invResolution.y = 1.0f / (GXFloat)height;

	Load3DModel ();
	InitUniforms ();
}

TSBloomEffect::~TSBloomEffect ()
{
	glBindFramebuffer ( GL_FRAMEBUFFER, 0 );
	glBindTexture ( GL_TEXTURE_2D, 0 );
	glDeleteTextures ( 1, &blurXTexture );
	glDeleteTextures ( 1, &blurYTexture );
	glDeleteFramebuffers ( 1, &fbo );

	GXRemoveVAO ( vaoInfo ); 

	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
	GXRemoveShaderProgram ( mat_info.shaders[ 1 ] );
	GXRemoveShaderProgram ( mat_info.shaders[ 2 ] );
}


GXVoid TSBloomEffect::Draw ()
{
	glViewport ( 0, 0, resolutionX, resolutionY );

	glBindFramebuffer ( GL_FRAMEBUFFER, fbo );
	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurYTexture, 0 );

	static const GLenum buf = GL_COLOR_ATTACHMENT0;
	glDrawBuffers ( 1, &buf );

	glUseProgram ( mat_info.shaders[ 2 ].uiId );

	glUniform2fv ( stepXYLocation, 1, invResolution._v );
	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, hdrTexture );
	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, averageLumTexture );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );

	
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurXTexture, 0 );
	glUniform1fv  ( blurXLocation, 1, &invResolution.x );
	glBindTexture ( GL_TEXTURE_2D, blurYTexture );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );


	glUseProgram ( mat_info.shaders[ 1 ].uiId );

	glFramebufferTexture2D ( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurYTexture, 0 );
	glBindTexture ( GL_TEXTURE_2D, blurXTexture );
	glUniform1fv  ( blurYLocation, 1, &invResolution.y );

	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
}

GXVoid TSBloomEffect::SetAverageTexture ( GLuint texture )
{
	averageLumTexture = texture;
}

GLuint TSBloomEffect::GetBloomTexture ()
{
	return blurYTexture;
}

GXVoid TSBloomEffect::Load3DModel ()
{
	GXLoadMTR ( L"../Materials/Thesis/Bloom_Effect.mtr", mat_info );
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/System/ScreenQuad.obj", L"../3D Models/System/Cache/ScreenQuad.cache" );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
	GXGetShaderProgram ( mat_info.shaders[ 1 ] );
	GXGetShaderProgram ( mat_info.shaders[ 2 ] );
}

GXVoid TSBloomEffect::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );
	blurXLocation = glGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "blurX" );
	glUniform1i ( glGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "hdrTexture" ), 0 );

	glUseProgram ( mat_info.shaders[ 1 ].uiId );
	blurYLocation = glGetUniformLocation ( mat_info.shaders[ 1 ].uiId, "blurY" );
	glUniform1i ( glGetUniformLocation ( mat_info.shaders[ 1 ].uiId, "hdrTexture" ), 0 );

	glUseProgram ( mat_info.shaders[ 2 ].uiId );
	stepXYLocation = glGetUniformLocation ( mat_info.shaders[ 2 ].uiId, "stepXY" );
	glUniform1i ( glGetUniformLocation ( mat_info.shaders[ 2 ].uiId, "hdrTexture" ), 0 );
	glUniform1i ( glGetUniformLocation ( mat_info.shaders[ 2 ].uiId, "averageLumTexture" ), 1 );

	glUseProgram ( 0 );
}