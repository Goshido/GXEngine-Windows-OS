//version 1.0

#ifndef TS_LAND_CRUISER_NET_CONTROLLER
#define TS_LAND_CRUISER_NET_CONTROLLER


#include "TSNetController.h"
#include "TSLandCruiser.h"
#include "TSLandCruiserPhysicsObject.h"


class TSLandCruiserNetController : public TSNetController
{
	private:
		TSLandCruiser*					car;
		TSLandCruiserPhysicsObject*		actor;

	public:
		TSLandCruiserNetController ( GXUInt clientID, GXUInt objectID );
		virtual ~TSLandCruiserNetController ();

		virtual GXVoid OnReceive ( const GXVoid* data, GXUInt size );

		GXVoid SetBodyOrigin ( const GXVec3 &location, const GXQuat &rotation );
		GXVoid SetWheelOrigin ( GXUShort id, const GXVec3 &location, const GXQuat &rotation );		
};


#endif //TS_LAND_CRUISER_NET_CONTROLLER