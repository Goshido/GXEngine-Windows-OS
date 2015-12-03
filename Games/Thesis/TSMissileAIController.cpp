//version 1.0

#include "TSMissileAIController.h"
#include "TSStingerProjectileAIController.h"
#include "TSClient.h"
#include "TSNetProtocol.h"


TSMissileAIController::TSMissileAIController ():
TSWeaponAIController ( L"Red" )
{
	ammo = 50;
	maxAmmo = 60;		
	fireTimer = 0.0f;
	fireDelay = 1.0f;
	damage = 15;

	missile = new TSMissile ();
	ReplicateMe ();
}

TSMissileAIController::~TSMissileAIController ()
{
	missile->Delete ();
	ReplicateDelete ();
}

GXVoid TSMissileAIController::OnUpdate ( GXFloat deltaTime )
{
	if ( fireTimer > 0.0f )
		fireTimer -= deltaTime;

	if ( isFire )
	{
		if ( fireTimer <= 0.0f )
		{
			if ( ammo > 0 )
			{
				GXVec3 loc;
				GXMat4 rot;

				missile->GetFireOrigin ( loc, rot );
				new TSStingerProjectileAIController ( target, loc, rot );
				ammo--;
			}

			fireTimer = fireDelay;
		}
	}

	GXVec3 loc;
	GXMat4 rot;

	attachment->GetTransform ( loc, rot );
	missile->SetOrigin ( loc, rot );

	ReplicateOrigin ();
}

GXVoid TSMissileAIController::OnReplicate ()
{
	ReplicateMe ();
}

GXVoid TSMissileAIController::ReplicateMe ()
{
	if ( ts_ClientID == 0xFFFFFFFF ) return;

	GXUInt size = sizeof ( TSNetObjectSpawnFrameData ) + sizeof ( TSNetMissileOriginFrameData );
	GXChar* frame = (GXChar*)malloc ( size );

	TSNetObjectSpawnFrameData* spawnData = (TSNetObjectSpawnFrameData*)frame;
	spawnData->controllerID = TS_NET_MISSILE_CONTROLLER_ID;

	TSNetMissileOriginFrameData* originData = (TSNetMissileOriginFrameData*)( frame + sizeof ( TSNetObjectSpawnFrameData ) );
	missile->GetOrigin ( originData->location, originData->rotation );
	
	TSClientSendTCP ( (GXUInt)this, TS_NET_OBJECT_SPAWN, frame, size );

	free ( frame );
}

GXVoid TSMissileAIController::ReplicateOrigin ()
{
	if ( ts_ClientID == 0xFFFFFFFF ) return;

	TSNetMissileOriginFrameData originData;
	missile->GetOrigin ( originData.location, originData.rotation );

	TSClientSendUDP ( (GXUInt)this, TS_NET_OBJECT_MISSILE_ORIGIN, &originData, sizeof ( TSNetMissileOriginFrameData ) );
}

GXVoid TSMissileAIController::ReplicateDelete ()
{
	if ( ts_ClientID == 0xFFFFFFFF ) return;

	TSClientSendTCP ( (GXUInt)this, TS_NET_OBJECT_DESTROY, 0, 0 );
}