//version 1.2

#include "TSCannon.h"


TSCannon::TSCannon ()
{
	cannon = new TSMesh ( L"../3D Models/Thesis/Cannon.obj", L"../3D Models/Thesis/Cache/Cannon.cache", L"../Materials/Thesis/Cannon.mtr" );

	muzzleFlash01 = new TSSprite ( L"../Materials/Thesis/Exhaust_Flash.mtr" );
	muzzleFlash01->SetScale ( 1.0f, 1.0f, 1.0f );
	muzzleFlash01->SetTwoFaced ( GX_TRUE );

	muzzleFlash02 = new TSBillboard ( L"../Materials/Thesis/Muzzle_Flash01.mtr" );
	muzzleFlash02->SetScale ( 1.0f, 3.0f, 1.0f );
	muzzleFlash02->LockForward ();

	GXMat4 offset;
	GXSetMat4Identity ( offset );
	GXConvert3DSMaxToGXEngine ( offset.wv, 0.0f, -2.263f, 0.526f );
	flashAttachment = new GXAttachment ( L"Muzzle Flash", cannon, offset );

	GXConvert3DSMaxToGXEngine ( offset.wv, 0.0f, -1.099f, 0.526f );
	smokeAttachment = new GXAttachment ( L"Launch", cannon, offset );

	GXIdealParticleSystemParams ps;
	ps.size = 1.5f;
	ps.delayTime = 0.03f;
	ps.lifeTime = 1.0f;
	ps.maxParticles = 50;
	ps.acceleration = GXVec3 ( 0.0f, 3.0f, 0.0f );
	ps.deviation = 1.0f;
	ps.maxVelocity = 4.0f;
	
	GXVec3* distribution = (GXVec3*)malloc ( ps.maxParticles * sizeof ( GXVec3 ) );
	GXVec3* positions = (GXVec3*)malloc ( ps.maxParticles * sizeof ( GXVec3 ) );
	GXVec3* velocities = (GXVec3*)malloc ( ps.maxParticles * sizeof ( GXVec3 ) );
	GXFloat* startLifeTime = (GXFloat*)malloc ( ps.maxParticles * sizeof ( GXFloat ) );

	GXVec3 v1 ( 0.0f, 1.0f, 0.0f );

	GXFloat tx = cosf ( GXDegToRad ( 240.0f ) );
	GXFloat ty = sinf ( GXDegToRad ( 240.0f ) );
	GXVec3 v2 ( tx, ty, 0.0f );

	tx = cosf ( GXDegToRad ( 330.0f ) );
	ty = sinf ( GXDegToRad ( 330.0f ) );
	GXVec3 v3 ( tx, ty, 0.0f );

	GXUChar beam = 0;
	GXFloat l = -0.3f;

	for ( GXUInt i = 0; i < ps.maxParticles; i++ )
	{
		switch ( beam )
		{
			case 0:
				distribution[ i ] = v1;
			break;

			case 1:
				distribution[ i ] = v2;
			break;

			case 2:
				distribution[ i ] = v3;
			break;
		}

		startLifeTime[ i ] = l;
		

		if ( beam == 2 )
		{
			beam = 0;
			l -= ps.delayTime;
		}
		else
			beam++;
	}

	memset ( positions, 0, ps.maxParticles * sizeof ( GXVec3 ) );
	memset ( velocities, 0, ps.maxParticles * sizeof ( GXVec3 ) );

	ps.distribution = distribution;
	ps.startLifeTime = startLifeTime;
	ps.startPositions = positions;
	ps.startVelocities = velocities;

	launchParticles = new TSSmokeParticles ( ps, L"../Materials/Thesis/White_Smoke_Particles.mtr" );

	delay = 2.0f;
	timer = 0.0f;

	isDelete = GX_FALSE;
}

GXVoid TSCannon::Delete ()
{
	isDelete = GX_TRUE;
}

GXVoid TSCannon::OnDraw ( eTSGBufferPass pass )
{
	if ( isDelete )
	{
		delete this;
		return;
	}

	switch  ( pass )
	{
		case TS_COMMON:
			cannon->Draw ();
		break;

		case TS_HUD_DEPTH_DEPENDENT:
		{
			if ( timer > 1.952f )
			{
				muzzleFlash01->Draw ();
				muzzleFlash02->Draw ();
			}

			if ( timer > 0.1f )
			{
				launchParticles->Draw ();
				return;
			}

			launchParticles->Reset ();
		}
		break;		

		default:
		break;
	}
}

GXVoid TSCannon::OnUpdate ( GXFloat deltaTime )
{
	if ( timer <= 0.0f ) return;
	{
		timer -= deltaTime;

		if ( !launchParticles ) return;

		if ( timer > 1.0f )
			launchParticles->Update ( deltaTime );
		else
			launchParticles->EnableEmission ( GX_FALSE );
	}
}

GXVoid TSCannon::SetOrigin ( const GXVec3 &location, const GXMat4 &rotation )
{
	cannon->SetLocation ( location );
	cannon->SetRotation ( rotation );

	GXVec3 loc;
	GXMat4 rot;

	flashAttachment->GetTransform ( loc, rot );

	muzzleFlash01->SetLocation ( loc );
	muzzleFlash01->SetRotation ( rot );

	muzzleFlash02->SetLocation ( loc );
	muzzleFlash02->SetRotation ( rot );

	smokeAttachment->GetTransform ( loc, rot );
	launchParticles->SetLocation ( loc );
	launchParticles->SetRotation ( rot );
}

GXVoid TSCannon::GetOrigin ( GXVec3 &location, GXQuat &rotation )
{
	cannon->GetLocation ( location );
	cannon->GetRotation ( rotation );
}

GXVoid TSCannon::Fire ()
{
	timer = delay;

	launchParticles->EnableEmission ( GX_TRUE );
}

GXVoid TSCannon::GetMuzzleFlashOrigin ( GXVec3 &location, GXMat4 &rotation )
{
	flashAttachment->GetTransform ( location, rotation );
}


TSCannon::~TSCannon ()
{
	cannon->Delete ();
	cannon->Draw ();

	muzzleFlash01->Delete ();
	muzzleFlash01->Draw ();

	muzzleFlash02->Delete ();
	muzzleFlash02->Draw ();

	launchParticles->Delete ();
	launchParticles->Draw ();

	GXSafeDelete ( smokeAttachment );
	GXSafeDelete ( flashAttachment );
}