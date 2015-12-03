//version 1.1

#include "TSCannonballProjectileAIController.h"
#include <GXEngine/GXGlobals.h>
#include <GXEngine/GXPlayerBoss.h>
#include "TSClient.h"
#include "TSNetProtocol.h"


class TSCannonballProjectileHitReport : public GXHitReportBase
{
	private:
		TSCannonballProjectileAIController*	owner;

	public:
		TSCannonballProjectileHitReport ( TSCannonballProjectileAIController* owner );
		~TSCannonballProjectileHitReport ();

		virtual GXVoid onShapeHit ( const PxControllerShapeHit& hit );
		virtual GXVoid onControllerHit ( const PxControllersHit& hit );
		virtual GXVoid onObstacleHit ( const PxControllerObstacleHit& hit );
};


TSCannonballProjectileHitReport::TSCannonballProjectileHitReport ( TSCannonballProjectileAIController* owner )
{
	this->owner = owner;
}

TSCannonballProjectileHitReport::~TSCannonballProjectileHitReport ()
{
	//NOTHING
}

GXVoid TSCannonballProjectileHitReport::onShapeHit ( const PxControllerShapeHit& hit )
{
	owner->OnHit ( hit );
}

GXVoid TSCannonballProjectileHitReport::onControllerHit ( const PxControllersHit& hit )
{
	owner->OnHit ( hit );
}

GXVoid TSCannonballProjectileHitReport::onObstacleHit ( const PxControllerObstacleHit& hit )
{
	owner->OnHit ( hit );
}

//====================================================================================================

TSCannonballProjectileAIController::TSCannonballProjectileAIController ( const GXVec3 &location, const GXMat4 &rotation )
{
	cannonball = new TSCannonballProjectile ( location, rotation );
	
	damage = 30;
	isFly = GX_TRUE;
	velocity = 1.2f;
	lifeTime = 10.0f;

	direction = rotation.zv;

	GXPhysics* physics = gx_Core->GetPhysics ();
	PxMaterial* mat = physics->CreateMaterial ( 100.0f, 500.0f, 0.1f );
	hitReport = new TSCannonballProjectileHitReport ( this );

	PxCapsuleControllerDesc desc;
	desc.height = 0.3f;
	desc.material = mat;
	desc.callback = hitReport;
	desc.behaviorCallback = 0;
	desc.climbingMode = PxCapsuleClimbingMode::eEASY;
	desc.contactOffset = 0.02f;
	desc.density = 10.0f;
	desc.maxJumpHeight = 3.0f;
	desc.radius = 0.112f;
	desc.position = PxExtendedVec3 ( location.x, location.y, location.z );
	desc.userData = 0;
	controller = physics->CreateCharacterController ( desc );

	ReplicateMe ( location );
}

TSCannonballProjectileAIController::~TSCannonballProjectileAIController ()
{
	cannonball->Delete ();
	controller->release ();
	delete hitReport;
}

GXVoid TSCannonballProjectileAIController::OnUpdate ( GXFloat deltaTime )
{
	lifeTime -= deltaTime;
	if ( lifeTime <= 0.0f || !isFly )
	{
		ReplicateExplosed ();
		cannonball->Explose ();
		delete this;
		return;
	}

	PxVec3 vel ( direction.x * velocity, direction.y * velocity, direction.z * velocity );
	PxControllerFilters f;
	controller->move ( vel, 0.03f, deltaTime, f );

	PxExtendedVec3 p = controller->getPosition ();
	GXVec3 location ( (GXFloat)p.x, (GXFloat)p.y, (GXFloat)p.z );
	GXMat4 rot;
	GXSetMat4Identity ( rot );
	cannonball->SetOrigin ( location, rot );

	ReplicateOrigin ();
}

GXVoid TSCannonballProjectileAIController::OnReplicate ()
{
	GXVec3 loc;
	cannonball->GetLocation ( loc );

	ReplicateMe ( loc );
}

GXVoid TSCannonballProjectileAIController::OnHit ( const PxControllerShapeHit &hit )
{
	isFly = GX_FALSE;
}

GXVoid TSCannonballProjectileAIController::OnHit ( const PxControllersHit &hit )
{
	isFly = GX_FALSE;
}

GXVoid TSCannonballProjectileAIController::OnHit ( const PxControllerObstacleHit &hit )
{
	isFly = GX_FALSE;
}

GXVoid TSCannonballProjectileAIController::ReplicateMe ( const GXVec3 &location )
{
	if ( ts_ClientID == TS_NET_INVALID_ID ) return;

	GXUInt size = sizeof ( TSNetObjectSpawnFrameData ) + sizeof ( TSNetCannonballProjectileOriginFrameData );
	GXChar* frame = (GXChar*)malloc ( size );

	TSNetObjectSpawnFrameData* spawnData = (TSNetObjectSpawnFrameData*)frame;
	spawnData->controllerID = TS_NET_CANNONBALL_PROJECTILE_CONTROLLER_ID;

	TSNetCannonballProjectileOriginFrameData* originData = (TSNetCannonballProjectileOriginFrameData*)( frame + sizeof ( TSNetObjectSpawnFrameData ) );
	originData->location = location;
	
	TSClientSendTCP ( (GXUInt)this, TS_NET_OBJECT_SPAWN, frame, size );

	free ( frame );
}

GXVoid TSCannonballProjectileAIController::ReplicateOrigin ()
{
	if ( ts_ClientID == TS_NET_INVALID_ID ) return;

	TSNetCannonballProjectileOriginFrameData originData;
	cannonball->GetLocation ( originData.location );

	TSClientSendUDP ( (GXUInt)this, TS_NET_OBJECT_CANNONBALL_PROJECTILE_ORIGIN, &originData, sizeof ( TSNetCannonballProjectileOriginFrameData ) );
}

GXVoid TSCannonballProjectileAIController::ReplicateExplosed ()
{
	if ( ts_ClientID == TS_NET_INVALID_ID ) return;

	TSClientSendTCP ( (GXUInt)this, TS_NET_OBJECT_CANNONBALL_PROJECTILE_EXPLOSED, 0, 0 );
}