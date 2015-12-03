//version 1.1

#include "TSStingerProjectileAIController.h"
#include <GXEngine/GXGlobals.h>
#include <GXEngine/GXPlayerBoss.h>
#include "TSClient.h"
#include "TSNetProtocol.h"


class TSStingerProjectileHitReport : public GXHitReportBase
{
	private:
		TSStingerProjectileAIController*	owner;

	public:
		TSStingerProjectileHitReport ( TSStingerProjectileAIController* owner );
		~TSStingerProjectileHitReport ();

		virtual GXVoid onShapeHit ( const PxControllerShapeHit& hit );
		virtual GXVoid onControllerHit ( const PxControllersHit& hit );
		virtual GXVoid onObstacleHit ( const PxControllerObstacleHit& hit );
};


TSStingerProjectileHitReport::TSStingerProjectileHitReport ( TSStingerProjectileAIController* owner )
{
	this->owner = owner;
}

TSStingerProjectileHitReport::~TSStingerProjectileHitReport ()
{
	//NOTHING
}

GXVoid TSStingerProjectileHitReport::onShapeHit ( const PxControllerShapeHit& hit )
{
	owner->OnHit ( hit );
}

GXVoid TSStingerProjectileHitReport::onControllerHit ( const PxControllersHit& hit )
{
	owner->OnHit ( hit );
}

GXVoid TSStingerProjectileHitReport::onObstacleHit ( const PxControllerObstacleHit& hit )
{
	owner->OnHit ( hit );
}

//====================================================================================================

TSStingerProjectileAIController::TSStingerProjectileAIController ( TSCar* target, const GXVec3 &location, const GXMat4 &rotation)
{
	this->target = target;
	stinger = new TSStingerProjectile ( location, rotation );
	
	damage = 30;
	isFly = GX_TRUE;
	velocity = 0.6f;
	lifeTime = 20.0f;

	stinger->SetOrigin ( location, rotation );

	GXPhysics* physics = gx_Core->GetPhysics ();
	PxMaterial* mat = physics->CreateMaterial ( 100.0f, 500.0f, 0.1f );
	hitReport = new TSStingerProjectileHitReport ( this );

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

	ReplicateMe ( location, rotation );
}

TSStingerProjectileAIController::~TSStingerProjectileAIController ()
{
	stinger->Delete ();
	controller->release ();
	delete hitReport;
}

GXVoid TSStingerProjectileAIController::OnUpdate ( GXFloat deltaTime )
{
	lifeTime -= deltaTime;
	if ( lifeTime <= 0.0f || !isFly )
	{
		ReplicateExplosed ();
		stinger->Explose ();
		delete this;
		return;
	}

	GXVec3 start;
	stinger->GetLocation ( start );

	GXVec3 finish;
	GXMat4 rotation;
	target->GetBodyOrigin ( finish, rotation );

	GXVec3 dir;
	dir.x = finish.x - start.x;
	dir.y = finish.y - start.y;
	dir.z = finish.z - start.z;
	GXNormalizeVec3 ( dir );

	rotation.zv = dir;

	GXVec3 up ( 0.0f, 1.0f, 0.0f );
	GXCrossVec3Vec3 ( rotation.xv, dir, up );
	GXCrossVec3Vec3 ( rotation.yv, rotation.zv, rotation.xv );

	PxVec3 vel ( dir.x * velocity, dir.y * velocity, dir.z * velocity );
	PxControllerFilters f;
	controller->move ( vel, 0.03f, deltaTime, f );

	PxExtendedVec3 p = controller->getPosition ();
	GXVec3 location ( (GXFloat)p.x, (GXFloat)p.y, (GXFloat)p.z );
	stinger->SetOrigin ( location, rotation );

	ReplicateOrigin ();
}

GXVoid TSStingerProjectileAIController::OnReplicate ()
{
	GXVec3 loc;
	GXQuat q;
	stinger->GetOrigin ( loc, q );

	GXVec3 tmp ( 0.0f, 0.0f, 0.0f );
	GXMat4 rot;
	rot.From ( q, tmp );

	ReplicateMe ( loc, rot );
}

GXVoid TSStingerProjectileAIController::OnHit ( const PxControllerShapeHit &hit )
{
	isFly = GX_FALSE;
}

GXVoid TSStingerProjectileAIController::OnHit ( const PxControllersHit &hit )
{
	isFly = GX_FALSE;
}

GXVoid TSStingerProjectileAIController::OnHit ( const PxControllerObstacleHit &hit )
{
	isFly = GX_FALSE;
}

GXVoid TSStingerProjectileAIController::ReplicateMe ( const GXVec3 &location, const GXMat4 &rotation )
{
	if ( ts_ClientID == TS_NET_INVALID_ID ) return;

	GXUInt size = sizeof ( TSNetObjectSpawnFrameData ) + sizeof ( TSNetStingerProjectileOriginFrameData );
	GXChar* frame = (GXChar*)malloc ( size );

	TSNetObjectSpawnFrameData* spawnData = (TSNetObjectSpawnFrameData*)frame;
	spawnData->controllerID = TS_NET_STINGER_PROJECTILE_CONTROLLER_ID;

	TSNetStingerProjectileOriginFrameData* originData = (TSNetStingerProjectileOriginFrameData*)( frame + sizeof ( TSNetObjectSpawnFrameData ) );
	originData->location = location;
	originData->rotation = GXCreateQuat ( rotation );
	
	TSClientSendTCP ( (GXUInt)this, TS_NET_OBJECT_SPAWN, frame, size );

	free ( frame );
}

GXVoid TSStingerProjectileAIController::ReplicateOrigin ()
{
	if ( ts_ClientID == TS_NET_INVALID_ID ) return;

	TSNetStingerProjectileOriginFrameData originData;
	stinger->GetOrigin ( originData.location, originData.rotation );

	TSClientSendUDP ( (GXUInt)this, TS_NET_OBJECT_STINGER_PROJECTILE_ORIGIN, &originData, sizeof ( TSNetStingerProjectileOriginFrameData ) );
}

GXVoid TSStingerProjectileAIController::ReplicateExplosed ()
{
	if ( ts_ClientID == TS_NET_INVALID_ID ) return;

	TSClientSendTCP ( (GXUInt)this, TS_NET_OBJECT_STINGER_PROJECTILE_EXPLOSED, 0, 0 );
}