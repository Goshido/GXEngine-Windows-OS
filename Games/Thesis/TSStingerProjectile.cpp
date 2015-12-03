//version 1.3

#include "TSStingerProjectile.h"
#include "TSGlobals.h"
#include <GXEngine/GXOGGSoundProvider.h>
#include <GXEngine/GXSoundStorage.h>


GXOGGSoundTrack* ts_stingerExhaustSoundTrack = 0;
GXOGGSoundTrack* ts_stingerExplosionSoundTrack = 0;


TSStingerProjectile::TSStingerProjectile ( const GXVec3 &location, const GXMat4 &rotation )
{
	ts_renderObjectMutex->Lock ();

	startLocation = location;
	startRotation = rotation;

	stinger = new TSMesh ( L"../3D Models/Thesis/Missile.obj", L"../3D Models/Thesis/Cache/Missile.cache", L"../Materials/Thesis/Missile.mtr" );
	stinger->SetLocation ( startLocation );
	stinger->SetRotation ( startRotation );

	GXMat4 offset;
	GXSetMat4Identity ( offset );
	GXConvert3DSMaxToGXEngine ( offset.wv, 0.0f, 1.655f, 0.0f );
	burn = new GXAttachment ( L"Exhaust", stinger, offset );

	InitExhaust ();
	InitExplosion ();
	InitSmoke ();

	isExplosed = GX_FALSE;
	deleteTimer = 6.0f;

	light = 0;

	ts_renderObjectMutex->Release ();
}

GXVoid TSStingerProjectile::OnUpdate ( GXFloat deltaTime )
{
	GXVec3 location;
	GXMat4 rotation;

	if ( isExplosed )
	{
		deleteTimer -= deltaTime;

		explosionParticles->Update ( deltaTime );
		exhaustParticles->Update ( deltaTime );
		smokeParticles->Update ( deltaTime );	

		light->SetIntensity ( 10.0f );
		light->SetInfluenceDistance ( 150.0f );

		return;
	}

	exhaustFlashTimer -= deltaTime;
	if ( exhaustFlashTimer <= 0.0f )
	{
		exhaustFlashSize[ 0 ] = exhaustFlashSize[ 1 ];
		exhaustFlashSize[ 1 ] = 0.2f + GXRandomNormalize ();
		exhaustFlashTimer = exhaustFlashTime;
	}

	GXFloat size = exhaustFlashSize[ 0 ] + ( exhaustFlashSize[ 1 ] - exhaustFlashSize[ 0 ] ) * ( 1.0f - exhaustFlashTimer * exhaustFlashInvTime );
	exhaustFlash->SetScale ( size, size, size );

	burn->GetTransform ( location, rotation );

	exhaustParticles->SetLocation ( location );
	exhaustParticles->SetRotation ( rotation );
	exhaustParticles->Update ( deltaTime );

	exhaustFlash->SetLocation ( location );
	exhaustFlash->SetRotation ( rotation );

	exhaustSoundEmitter->SetLocation ( location );

	if ( light )
	{
		light->SetLocation ( location );
		light->SetInfluenceDistance ( ( size + 0.1f ) * 20.0f );
	}
}

GXVoid TSStingerProjectile::OnDraw ( eTSGBufferPass pass )
{
	if ( deleteTimer < 0.0f )
	{
		delete this;
		return;
	}

	if ( !light )
	{
		light = new TSBulp ();

		GXVec3 loc;
		GXMat4 rot;
		burn->GetTransform ( loc, rot );
		light->SetLocation ( loc );
		light->SetColor ( 0.972f, 1.0f, 0.557f );
		light->SetIntensity ( 3.0f );
		light->SetInfluenceDistance ( 20.0f );
	}

	switch ( pass )
	{
		case TS_COMMON:
			if ( !isExplosed )
				stinger->Draw ();
		break;

		case TS_ALBEDO:
			if ( deleteTimer >= 5.95f )
				ts_GBuffer->LightBy ( light );
		break;

		case TS_HUD_DEPTH_DEPENDENT:
			exhaustParticles->Draw ();

			if ( isExplosed )
			{
				explosionParticles->Draw ();
				smokeParticles->Draw ();

				if ( deleteTimer > 5.95f )
					explosionFlash->Draw ();
			}
			else
			{
				exhaustFlash->Draw ();
			}
		break;
	}
}

GXVoid TSStingerProjectile::GetLocation ( GXVec3 &location )
{
	stinger->GetLocation ( location );
}

GXVoid TSStingerProjectile::GetOrigin ( GXVec3 &location, GXQuat &rotation )
{
	stinger->GetLocation ( location );
	stinger->GetRotation ( rotation );
}

GXVoid TSStingerProjectile::SetOrigin ( const GXVec3 &location, const GXMat4 &rotation )
{
	stinger->SetLocation ( location );
	stinger->SetRotation ( rotation );
}

GXVoid TSStingerProjectile::Explose ()
{
	isExplosed = GX_TRUE;

	GXVec3 location;
	stinger->GetLocation ( location );
	explosionFlash->SetLocation ( location );

	explosionParticles->SetLocation ( location );
	smokeParticles->SetLocation ( location );

	exhaustParticles->EnableEmission ( GX_FALSE );
	exhaustSoundEmitter->Stop ();

	explosionSoundEmitter->SetLocation ( location );
	explosionSoundEmitter->Play ();
}

TSStingerProjectile::~TSStingerProjectile ()
{
	delete exhaustSoundEmitter;
	delete burn;

	explosionParticles->Delete ();
	explosionParticles->Draw ();

	exhaustParticles->Delete ();
	exhaustParticles->Draw ();

	smokeParticles->Delete ();
	smokeParticles->Draw ();

	explosionFlash->Delete ();
	explosionFlash->Draw ();

	stinger->Delete ();
	stinger->Draw ();

	delete light;
}

GXVoid TSStingerProjectile::InitExhaust ()
{
	GXIdealParticleSystemParams ps;
	ps.deviation = 1.0f;
	ps.acceleration = GXVec3 ( 0.0f, 0.3f, 0.0f );
	ps.delayTime = 0.005f;
	ps.lifeTime = 1.0f;
	ps.maxParticles = 200;
	ps.maxVelocity = 50.0f;
	ps.size = 1.0f;

	GXVec3* distribution = (GXVec3*)malloc ( ps.maxParticles * sizeof ( GXVec3 ) );
	GXVec3* positions = (GXVec3*)malloc ( ps.maxParticles * sizeof ( GXVec3 ) );
	GXVec3* velocities = (GXVec3*)malloc ( ps.maxParticles * sizeof ( GXVec3 ) );
	GXFloat* startLifeTime = (GXFloat*)malloc ( ps.maxParticles * sizeof ( GXFloat ) );

	GXFloat delay = 0.01f;

	for ( GXUInt i = 0; i < ps.maxParticles; i++ )
	{
		distribution[ i ].x = GXRandomNormalize () * 2.0f - 1.0f;
		distribution[ i ].y = GXRandomNormalize () * 2.0f - 1.0f;
		distribution[ i ].z = -50.0f;

		GXNormalizeVec3 ( distribution[ i ] );

		startLifeTime[ i ] = delay;
		delay -= ps.delayTime;
	}

	memset ( positions, 0, ps.maxParticles * sizeof ( GXVec3 ) );
	memset ( velocities, 0, ps.maxParticles * sizeof ( GXVec3 ) );

	ps.distribution = distribution;
	ps.startPositions = positions;
	ps.startVelocities = velocities;
	ps.startLifeTime = startLifeTime;

	exhaustParticles = new TSExhaustParticles ( ps );

	exhaustFlash = new TSSprite ( L"../Materials/Thesis/Exhaust_Flash.mtr" );

	if ( !ts_stingerExhaustSoundTrack )
	{
		ts_stingerExhaustSoundTrack = (GXOGGSoundTrack*)GXGetSoundTrack ( L"../Sounds/Thesis/SFX/Rocket_Exhaust_Loop.ogg" );
		ts_stingerExhaustSoundTrack->AddRef ();
	}

	exhaustSoundEmitter = new GXSoundEmitter ( ts_stingerExhaustSoundTrack, GX_TRUE, GX_FALSE, GX_FALSE );
	exhaustSoundEmitter->SetRange ( 5.0f, 100.0f );

	ts_EffectChannel->AddEmitter ( exhaustSoundEmitter );

	exhaustSoundEmitter->Play ();

	exhaustFlashSize[ 0 ] = 1.0f;
	exhaustFlashSize[ 1 ] = 0.2f + GXRandomNormalize ();
	exhaustFlashTime = 0.12f;
	exhaustFlashTimer = exhaustFlashTime;
	exhaustFlashInvTime = 1.0f / exhaustFlashTime;
}

GXVoid TSStingerProjectile::InitExplosion ()
{
	GXIdealParticleSystemParams ps;
	ps.deviation = 1.0f;
	ps.acceleration = GXVec3 ( 0.0f, -0.1f, 0.0f );
	ps.delayTime = 100.0f;
	ps.lifeTime = 1.0f;
	ps.maxParticles = 60;
	ps.maxVelocity = 160.0f;
	ps.size = 1.0f;

	GXVec3* distribution = (GXVec3*)malloc ( ps.maxParticles * sizeof ( GXVec3 ) );
	GXVec3* positions = (GXVec3*)malloc ( ps.maxParticles * sizeof ( GXVec3 ) );
	GXVec3* velocities = (GXVec3*)malloc ( ps.maxParticles * sizeof ( GXVec3 ) );
	GXFloat* startLifeTime = (GXFloat*)malloc ( ps.maxParticles * sizeof ( GXFloat ) );

	for ( GXUInt i = 0; i < ps.maxParticles; i++ )
	{
		distribution[ i ].x = GXRandomNormalize () * 2.0f - 1.0f;
		distribution[ i ].y = GXRandomNormalize () * 2.0f - 1.0f;
		distribution[ i ].z = GXRandomNormalize () * 2.0f - 1.0f;

		GXNormalizeVec3 ( distribution[ i ] );

		startLifeTime[ i ] = -0.15f;
	}
	
	memset ( positions, 0, ps.maxParticles * sizeof ( GXVec3 ) );
	memset ( velocities, 0, ps.maxParticles * sizeof ( GXVec3 ) );

	ps.distribution = distribution;
	ps.startPositions = positions;
	ps.startVelocities = velocities;
	ps.startLifeTime = startLifeTime;

	explosionParticles = new TSExplosionParticles ( ps );

	explosionFlash = new TSBillboard ( L"../Materials/Thesis/Explosion_Flash.mtr" );
	explosionFlash->SetScale ( 15.0f, 15.0f, 15.0f );

	if ( !ts_stingerExplosionSoundTrack )
	{
		ts_stingerExplosionSoundTrack = (GXOGGSoundTrack*)GXGetSoundTrack ( L"../Sounds/Thesis/SFX/Explosion01.ogg" );
		ts_stingerExplosionSoundTrack->AddRef ();
	}

	explosionSoundEmitter = new GXSoundEmitter ( ts_stingerExplosionSoundTrack, GX_FALSE, GX_FALSE, GX_FALSE );
	explosionSoundEmitter->SetRange ( 100.0f, 350.0f );

	ts_EffectChannel->AddEmitter ( explosionSoundEmitter );
}

GXVoid TSStingerProjectile::InitSmoke ()
{
	GXIdealParticleSystemParams ps;
	ps.deviation = 1.0f;
	ps.acceleration = GXVec3 ( 0.0f, 3.0f, 0.0f );
	ps.delayTime = 100.0f;
	ps.lifeTime = 6.0f;
	ps.maxParticles = 100;
	ps.maxVelocity = 2.0f;
	ps.size = 3.0f;

	GXVec3* distribution = (GXVec3*)malloc ( ps.maxParticles * sizeof ( GXVec3 ) );
	GXVec3* positions = (GXVec3*)malloc ( ps.maxParticles * sizeof ( GXVec3 ) );
	GXVec3* velocities = (GXVec3*)malloc ( ps.maxParticles * sizeof ( GXVec3 ) );
	GXFloat* startLifeTime = (GXFloat*)malloc ( ps.maxParticles * sizeof ( GXFloat ) );

	for ( GXUInt i = 0; i < ps.maxParticles; i++ )
	{
		distribution[ i ].x = GXRandomNormalize () * 2.0f - 1.0f;
		distribution[ i ].y = GXRandomNormalize () * 2.0f - 1.0f;
		distribution[ i ].z = GXRandomNormalize () * 2.0f - 1.0f + 0.5f;

		GXNormalizeVec3 ( distribution[ i ] );

		startLifeTime[ i ] = -0.15f;
	}

	memset ( positions, 0, ps.maxParticles * sizeof ( GXVec3 ) );
	memset ( velocities, 0, ps.maxParticles * sizeof ( GXVec3 ) );

	ps.distribution = distribution;
	ps.startPositions = positions;
	ps.startVelocities = velocities;
	ps.startLifeTime = startLifeTime;

	smokeParticles = new TSSmokeParticles ( ps, L"../Materials/Thesis/Smoke_Particles.mtr" );
	smokeParticles->SetRotation ( -GX_MATH_HALFPI, 0.0f, 0.0f );
}