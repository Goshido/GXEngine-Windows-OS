//version 1.1

#ifndef TS_LAND_CRUISER_PHYSICS_OBJECT
#define TS_LAND_CRUISER_PHYSICS_OBJECT


#include "TSPhysicsObject.h"


class TSLandCruiserPhysicsObject : public TSPhysicsObject
{
	private:
		PxRigidDynamic*		actor;
		GXBool				isDelete;

		PxTransform			transform;

	public:
		TSLandCruiserPhysicsObject ();
		virtual GXVoid Delete ();

		virtual GXVoid OnPhysics ( GXFloat deltaTime );

		GXVoid SetOrigin ( const GXVec3 &location, const GXQuat &rotation );

	protected:
		virtual ~TSLandCruiserPhysicsObject ();
};


#endif //TS_LAND_CRUISER_PHYSICS_OBJECT