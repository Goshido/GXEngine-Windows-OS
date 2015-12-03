//version 1.1

#include "TSCannonAIController.h"
#include "TSCannonballProjectileAIController.h"
#include "TSClient.h"
#include "TSNetProtocol.h"


TSCannonAIController::TSCannonAIController ():
TSWeaponAIController ( L"Red" )
{
	ammo = 50;
	maxAmmo = 60;		
	fireTimer = 0.0f;
	fireDelay = 2.0f;
	damage = 35;

	cannon = new TSCannon ();
	ReplicateMe ();
}

TSCannonAIController::~TSCannonAIController ()
{
	cannon->Delete ();
	ReplicateDelete ();
}

GXVoid TSCannonAIController::OnUpdate ( GXFloat deltaTime )
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

				cannon->GetMuzzleFlashOrigin ( loc, rot );
				new TSCannonballProjectileAIController ( loc, rot );
				cannon->Fire ();
				ReplicateFire ();

				ammo--;
			}

			fireTimer = fireDelay;
		}
	}

	GXVec3 loc;
	GXMat4 rot;

	attachment->GetTransform ( loc, rot );

	GXVec3 target_loc;
	GXMat4 target_rot;
	target->GetBodyOrigin ( target_loc, target_rot );

	GXVec3 tempForward;
	tempForward.x = target_loc.x - loc.x;
	tempForward.y = target_loc.y - loc.y;
	tempForward.z = target_loc.z - loc.z;
	GXNormalizeVec3 ( tempForward );

	GXMat4 rotation;
	GXSetMat4Identity ( rotation );
	rotation.yv = rot.yv;

	GXCrossVec3Vec3 ( rotation.xv, rotation.yv, tempForward );
	GXCrossVec3Vec3 ( rotation.zv, rotation.xv, rotation.yv );

	cannon->SetOrigin ( loc, rotation );

	ReplicateOrigin ();
}

GXVoid TSCannonAIController::OnReplicate ()
{
	ReplicateMe ();
}

GXVoid TSCannonAIController::ReplicateMe ()
{
	if ( ts_ClientID == 0xFFFFFFFF ) return;

	GXUInt size = sizeof ( TSNetObjectSpawnFrameData ) + sizeof ( TSNetCannonOriginFrameData );
	GXChar* frame = (GXChar*)malloc ( size );

	TSNetObjectSpawnFrameData* spawnData = (TSNetObjectSpawnFrameData*)frame;
	spawnData->controllerID = TS_NET_CANNON_CONTROLLER_ID;

	TSNetCannonOriginFrameData* originData = (TSNetCannonOriginFrameData*)( frame + sizeof ( TSNetObjectSpawnFrameData ) );
	cannon->GetOrigin ( originData->location, originData->rotation );
	
	TSClientSendTCP ( (GXUInt)this, TS_NET_OBJECT_SPAWN, frame, size );

	free ( frame );
}

GXVoid TSCannonAIController::ReplicateOrigin ()
{
	if ( ts_ClientID == 0xFFFFFFFF ) return;

	TSNetCannonOriginFrameData originData;
	cannon->GetOrigin ( originData.location, originData.rotation );

	TSClientSendUDP ( (GXUInt)this, TS_NET_OBJECT_CANNON_ORIGIN, &originData, sizeof ( TSNetCannonOriginFrameData ) );
}

GXVoid TSCannonAIController::ReplicateFire ()
{
	if ( ts_ClientID == 0xFFFFFFFF ) return;

	TSClientSendTCP ( (GXUInt)this, TS_NET_OBJECT_CANNON_FIRE, 0, 0 );
}

GXVoid TSCannonAIController::ReplicateDelete ()
{
	if ( ts_ClientID == 0xFFFFFFFF ) return;

	TSClientSendTCP ( (GXUInt)this, TS_NET_OBJECT_DESTROY, 0, 0 );
}