//version 1.0

#ifndef TS_MISSILE_NET_CONTROLLER
#define TS_MISSILE_NET_CONTROLLER


#include "TSNetController.h"
#include "TSMissile.h"


class TSMissileNetController : public TSNetController
{
	private:
		TSMissile*		missile;

	public:
		TSMissileNetController ( GXUInt clientID, GXUInt objectID, const GXVec3 &location, const GXQuat &rotation );
		virtual ~TSMissileNetController ();

		virtual GXVoid OnReceive ( const GXVoid* data, GXUInt size );

		GXVoid SetOrigin ( const GXVec3 &location, const GXQuat &rotation );
};


#endif //TS_MISSILE_NET_CONTROLLER