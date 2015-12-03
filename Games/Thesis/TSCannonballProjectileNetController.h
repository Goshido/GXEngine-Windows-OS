//version 1.0

#ifndef TS_CANNONBALL_PROJECTILE_NET_CONTROLLER
#define TS_CANNONBALL_PROJECTILE_NET_CONTROLLER


#include "TSNetController.h"
#include "TSCannonballProjectile.h"


class TSCannonballProjectileNetController : public TSNetController
{
	private:
		TSCannonballProjectile*		projectile;

	public:
		TSCannonballProjectileNetController ( GXUInt clientID, GXUInt objectID, const GXVec3 &location );
		virtual ~TSCannonballProjectileNetController ();

		virtual GXVoid OnReceive ( const GXVoid* data, GXUInt size );

	private:
		GXVoid SetOrigin ( const GXVec3 &location );
};


#endif //TS_CANNONBALL_PROJECTILE_NET_CONTROLLER