//version 1.0

#include "TSSmokeParticles.h"
#include <GXEngine/GXGlobals.h>


TSSmokeParticles::TSSmokeParticles ( GXIdealParticleSystemParams &params, const GXWChar* material ) :
GXIdealParticles ( params )
{
	inertiaTimer = params.lifeTime;

	GXUInt size = sizeof ( GXWChar ) * ( wcslen ( material ) + 1 );
	this->material = (GXWChar*)malloc ( size );
	memcpy ( this->material, material, size );
}

GXVoid TSSmokeParticles::Draw ()
{
	if ( distribution == 0 )
		InitGraphicalResources ();

	if ( isDelete )
	{
		delete this;
		return;
	}

	if ( !isEmit && inertiaTimer < 0.0f ) return;

	GXIdealParticles::Draw ();

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	GXFloat deviation = params.deviation/* * GXRandomNormalize ()*/;

	glUniform1fv ( deltaTimeLocation, 1, &deltaTime );
	glUniform1fv ( delayTimeLocation, 1, &params.delayTime );
	glUniform1fv ( maxLifeTimeLocation, 1, &params.lifeTime );
	glUniform3fv ( accelerationLocation, 1, params.acceleration.v );
	glUniform1fv ( maxVelocityLocation, 1, &params.maxVelocity );
	glUniform3fv ( startPositionLocation, 1, trans_mat.wv.v );
	glUniformMatrix4fv ( local_rot_matLocation, 1, GL_FALSE, rot_mat.A );
	glUniform1fv ( deviationLocation, 1, &deviation );

	GXFloat stopEmit = isEmit ? 1.0f : -1.0f;
	glUniform1fv ( stopEmitLocation, 1, &stopEmit );

	glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, GX_IDEAL_PARTICLES_POSITION, position[ writeBuffer ] );
	glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, GX_IDEAL_PARTICLES_VELOCITY, velocity[ writeBuffer ] );
	glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, GX_IDEAL_PARTICLES_LIFETIME, lifetime[ writeBuffer ] );

	glEnable ( GL_RASTERIZER_DISCARD );

	glBeginTransformFeedback ( GL_POINTS );

	glBindVertexArray ( vao[ writeBuffer ? 0 : 1 ] );
	glDrawArrays ( GL_POINTS, 0, params.maxParticles );

	glEndTransformFeedback ();

	glUseProgram ( mat_info.shaders[ 1 ].uiId );

	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();
	GXMat4 mat;
	gx_ActiveCamera->GetRotation ( mat );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 0 ].uiId );

	glUniformMatrix4fv ( view_proj_matLocation, 1, GL_FALSE, view_proj_mat->A );
	glUniformMatrix4fv ( mod_matLocation, 1, GL_FALSE, mat.A );
	glUniform1fv ( invLifeTimeLocation, 1, &invLifeTime );
	glUniform1fv ( particleSizeLocation, 1, &params.size );

	glDisable ( GL_RASTERIZER_DISCARD );
	glDepthMask ( GL_FALSE );
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glBindVertexArray ( vao[ writeBuffer ] );
	glDrawArrays ( GL_POINTS, 0, params.maxParticles );

	glDepthMask ( GL_TRUE );
	glDisable ( GL_BLEND );

	SwapTransformFeedbackBuffers ();
}

GXVoid TSSmokeParticles::Update ( GXFloat deltaTime )
{
	this->deltaTime = deltaTime;

	if ( !isEmit )
		inertiaTimer -= params.delayTime;
}

GXVoid TSSmokeParticles::EnableEmission ( GXBool emission )
{
	isEmit = emission;

	if ( !emission )
		inertiaTimer = params.lifeTime;
}

TSSmokeParticles::~TSSmokeParticles ()
{
	GXSafeFree ( material );

	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
	GXRemoveShaderProgram ( mat_info.shaders[ 1 ] );
	GXRemoveTexture ( mat_info.textures[ 0 ] );
}

GXVoid TSSmokeParticles::Load3DModel ()
{
	GXLoadMTR ( material, mat_info );

	GXChar* varyings[] =
	{
		"outPosition",
		"outVelocity",
		"outLifetime"
	};

	GXGetTransformFeedbackShaderProgram ( mat_info.shaders[ 0 ], 3, (const GXChar**)varyings, GL_SEPARATE_ATTRIBS );

	GXGetShaderProgram ( mat_info.shaders[ 1 ] );

	GXGetTexture ( mat_info.textures[ 0 ] );
}

GXVoid TSSmokeParticles::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	deltaTimeLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "deltaTime" );
	delayTimeLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "delayTime" );
	maxLifeTimeLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "maxLifeTime" );
	accelerationLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "acceleration" );
	maxVelocityLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "maxVelocity" );
	startPositionLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "startPosition" );
	local_rot_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "local_rot_mat" );
	stopEmitLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "stopEmit" );
	deviationLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "deviation" );

	glUseProgram ( mat_info.shaders[ 1 ].uiId );

	invLifeTimeLocation = GXGetUniformLocation ( mat_info.shaders[ 1 ].uiId, "invLifeTime" );
	particleSizeLocation = GXGetUniformLocation ( mat_info.shaders[ 1 ].uiId, "particleSize" );
	mod_matLocation = GXGetUniformLocation ( mat_info.shaders[ 1 ].uiId, "mod_mat" );
	view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[ 1 ].uiId, "view_proj_mat" );

	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 1 ].uiId, "particleTexture" ), 0 );

	glUseProgram ( 0 );
}

GXVoid TSSmokeParticles::InitGraphicalResources ()
{
	GXIdealParticles::InitGraphicalResources ();
	
	Load3DModel ();
	InitUniforms ();

	GXSafeFree ( material );
}