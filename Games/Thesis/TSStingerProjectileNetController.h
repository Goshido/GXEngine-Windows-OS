//version 1.0

#ifndef TS_STINGER_PROJECTILE_NET_CONTROLLER
#define TS_STINGER_PROJECTILE_NET_CONTROLLER


#include "TSNetController.h"
#include "TSStingerProjectile.h"


class TSStingerProjectileNetController : public TSNetController
{
	private:
		TSStingerProjectile*		projectile;

	public:
		TSStingerProjectileNetController ( GXUInt clientID, GXUInt objectID, const GXVec3 &location, const GXQuat &rotation );
		virtual ~TSStingerProjectileNetController ();

		virtual GXVoid OnReceive ( const GXVoid* data, GXUInt size );

	private:
		GXVoid SetOrigin ( const GXVec3 &location, const GXQuat &rotation );
};


#endif //TS_STINGER_PROJECTILE_NET_CONTROLLER