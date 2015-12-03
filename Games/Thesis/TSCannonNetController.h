//version 1.0

#ifndef TS_CANNON_NET_CONTROLLER
#define TS_CANNON_NET_CONTROLLER


#include "TSNetController.h"
#include "TSCannon.h"


class TSCannonNetController : public TSNetController
{
	private:
		TSCannon*		cannon;

	public:
		TSCannonNetController ( GXUInt clientID, GXUInt objectID );
		virtual ~TSCannonNetController ();

		virtual GXVoid OnReceive ( const GXVoid* data, GXUInt size );

		GXVoid SetOrigin ( const GXVec3 &location, const GXQuat &rotation );
};


#endif //TS_CANNON_NET_CONTROLLER