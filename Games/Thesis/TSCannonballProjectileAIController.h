//version 1.1

#ifndef TS_CANNONBALL_PROJECTILE_AI_CONTROLLER
#define TS_CANNONBALL_PROJECTILE_AI_CONTROLLER


#include "TSAIController.h"
#include "TSCannonballProjectile.h"
#include <GXEngineDLL/GXPhysX.h>
#include "TSCar.h"


class TSCannonballProjectileHitReport;

class TSCannonballProjectileAIController : public TSAIController
{
	private:
		PxController*						controller;

		GXFloat								velocity;
		GXFloat								lifeTime;

		TSCannonballProjectile*				cannonball;
		GXInt								damage;
		GXBool								isFly;
		GXVec3								direction;

		TSCannonballProjectileHitReport*	hitReport;

	public:
		TSCannonballProjectileAIController ( const GXVec3 &location, const GXMat4 &rotation );
		virtual ~TSCannonballProjectileAIController ();

		virtual GXVoid OnUpdate ( GXFloat deltaTime );
		virtual GXVoid OnReplicate ();

		GXVoid OnHit ( const PxControllerShapeHit &hit );
		GXVoid OnHit ( const PxControllersHit &hit );
		GXVoid OnHit ( const PxControllerObstacleHit &hit );

	private:
		GXVoid ReplicateMe ( const GXVec3 &location  );
		GXVoid ReplicateOrigin ();
		GXVoid ReplicateExplosed ();
};


#endif //TS_CANNONBALL_PROJECTILE_AI_CONTROLLER