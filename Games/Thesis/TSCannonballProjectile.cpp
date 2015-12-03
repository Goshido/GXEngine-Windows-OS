//version 1.0

#include "TSCannonballProjectile.h"
#include "TSGlobals.h"
#include <GXEngine/GXOGGSoundProvider.h>
#include <GXEngine/GXSoundStorage.h>


GXOGGSoundTrack* ts_cannonballExplosionSoundTrack = 0;
GXOGGSoundTrack* ts_cannonballLaunchSoundTrack = 0;


TSCannonballProjectile::TSCannonballProjectile ( const GXVec3 &location, const GXMat4 &rotation )
{
	ts_renderObjectMutex->Lock ();

	cannonball = new TSBillboard ( L"../Materials/Thesis/Cannonball.mtr" );
	cannonball->SetScale ( 0.9f, 0.9f, 0.9f );
	cannonball->SetLocation ( startLocation );
	cannonball->SetRotation ( startRotation );

	explosionFlash = new TSBillboard ( L"../Materials/Thesis/Explosion_Flash.mtr" );
	explosionFlash->SetScale ( 15.0f, 15.0f, 15.0f );

	startLocation = location;
	startRotation = rotation;

	InitExplosion ();
	InitSmoke ();

	if ( !ts_cannonballLaunchSoundTrack )
	{
		ts_cannonballLaunchSoundTrack = (GXOGGSoundTrack*)GXGetSoundTrack ( L"../Sounds/Thesis/SFX/Cannon01.ogg" );
		ts_cannonballLaunchSoundTrack->AddRef ();
	}

	launchSoundEmitter = new GXSoundEmitter ( ts_cannonballLaunchSoundTrack, GX_FALSE, GX_FALSE, GX_FALSE );
	launchSoundEmitter->SetRange ( 50.0f, 200.0f );
	launchSoundEmitter->SetLocation ( startLocation );

	ts_EffectChannel->AddEmitter ( launchSoundEmitter );

	launchSoundEmitter->Play ();

	isExplosed = GX_FALSE;
	deleteTimer = 6.0f;

	light = 0;

	ts_renderObjectMutex->Release ();
}

GXVoid TSCannonballProjectile::OnUpdate ( GXFloat deltaTime )
{
	GXVec3 location;
	GXMat4 rotation;

	if ( isExplosed )
	{
		deleteTimer -= deltaTime;

		explosionParticles->Update ( deltaTime );
		smokeParticles->Update ( deltaTime );

		light->SetInfluenceDistance ( 150.0f );
		light->SetIntensity ( 10.0f );

		return;
	}

	if ( light )
	{
		GXVec3 loc;
		cannonball->GetLocation ( loc );
		light->SetLocation ( loc );
	}
}

GXVoid TSCannonballProjectile::OnDraw ( eTSGBufferPass pass )
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
		cannonball->GetLocation ( loc );
		light->SetLocation ( loc );
		light->SetColor ( 0.15f, 0.1f, 1.0f );
		light->SetIntensity ( 4.0f );
		light->SetInfluenceDistance ( 20.0f );
	}

	switch ( pass )
	{
		case TS_COMMON:

		break;

		case TS_ALBEDO:
		{
			if ( deleteTimer > 5.95f )
				ts_GBuffer->LightBy ( light );
		}

		case TS_HUD_DEPTH_DEPENDENT:
		{
			if ( isExplosed )
			{
				explosionParticles->Draw ();
				smokeParticles->Draw ();

				if ( deleteTimer > 5.95f )
					explosionFlash->Draw ();
			}
			else
				cannonball->Draw ();
		}
		break;

		default:
		break;
	}
}

GXVoid TSCannonballProjectile::GetLocation ( GXVec3 &location )
{
	cannonball->GetLocation ( location );
}

GXVoid TSCannonballProjectile::SetOrigin ( const GXVec3 &location, const GXMat4 &rotation )
{
	cannonball->SetLocation ( location );
	cannonball->SetRotation ( rotation );
}

GXVoid TSCannonballProjectile::Explose ()
{
	isExplosed = GX_TRUE;

	GXVec3 location;
	cannonball->GetLocation ( location );
	explosionFlash->SetLocation ( location );

	explosionParticles->SetLocation ( location );
	smokeParticles->SetLocation ( location );

	explosionSoundEmitter->SetLocation ( location );
	explosionSoundEmitter->Play ();
}

TSCannonballProjectile::~TSCannonballProjectile ()
{
	GXSafeDelete ( explosionSoundEmitter );
	GXSafeDelete ( launchSoundEmitter );

	explosionParticles->Delete ();
	explosionParticles->Draw ();

	smokeParticles->Delete ();
	smokeParticles->Draw ();

	explosionFlash->Delete ();
	explosionFlash->Draw ();

	cannonball->Delete ();
	cannonball->Draw ();

	delete light;
}

GXVoid TSCannonballProjectile::InitExplosion ()
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

	if ( !ts_cannonballExplosionSoundTrack )
	{
		ts_cannonballExplosionSoundTrack = (GXOGGSoundTrack*)GXGetSoundTrack ( L"../Sounds/Thesis/SFX/Explosion01.ogg" );
		ts_cannonballExplosionSoundTrack->AddRef ();
	}

	explosionSoundEmitter = new GXSoundEmitter ( ts_cannonballExplosionSoundTrack, GX_FALSE, GX_FALSE, GX_FALSE );
	explosionSoundEmitter->SetRange ( 100.0f, 350.0f );

	ts_EffectChannel->AddEmitter ( explosionSoundEmitter );
}

GXVoid TSCannonballProjectile::InitSmoke ()
{
	GXIdealParticleSystemParams ps;
	ps.deviation = 1.0f;
	ps.acceleration = GXVec3 ( 0.0f, 3.0f, 0.0f );
	ps.delayTime = 100.0f;
	ps.lifeTime = 6.0f;
	ps.maxParticles = 500;
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