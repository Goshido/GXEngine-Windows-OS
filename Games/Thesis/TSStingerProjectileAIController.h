//version 1.1

#ifndef TS_STINGER_PROJECTILE_AI_CONTROLLER
#define TS_STINGER_PROJECTILE_AI_CONTROLLER


#include "TSAIController.h"
#include <GXEngineDLL/GXPhysX.h>
#include "TSCar.h"
#include "TSStingerProjectile.h"


class TSStingerProjectileHitReport;

class TSStingerProjectileAIController : public TSAIController
{
	private:
		PxController*					controller;
		TSCar*							target;

		GXFloat							velocity;
		GXFloat							lifeTime;

		TSStingerProjectile*			stinger;
		GXInt							damage;
		GXBool							isFly;

		TSStingerProjectileHitReport*	hitReport;

	public:
		TSStingerProjectileAIController ( TSCar* target, const GXVec3 &location, const GXMat4 &rotation );
		virtual ~TSStingerProjectileAIController ();

		virtual GXVoid OnUpdate ( GXFloat deltaTime );
		virtual GXVoid OnReplicate ();

		GXVoid OnHit ( const PxControllerShapeHit &hit );
		GXVoid OnHit ( const PxControllersHit &hit );
		GXVoid OnHit ( const PxControllerObstacleHit &hit );

	private:
		GXVoid ReplicateMe ( const GXVec3 &location, const GXMat4 &rotation );
		GXVoid ReplicateOrigin ();
		GXVoid ReplicateExplosed ();
};


#endif //TS_STINGER_PROJECTILE_AI_CONTROLLER