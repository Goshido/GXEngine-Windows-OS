//version 1.1

#ifndef GX_PHYSICS_BOSS
#define GX_PHYSICS_BOSS


#include "GXGlobals.h"
#include "GXMesh.h"
#include "GXPhysics.h"


class GXPhysicsBoss : public GXPhysicsActorState
{
	protected:
		GXVec3					location;
		GXVec3					rotation;

		GXMesh* mesh;
		PxRigidDynamic* actor;

	public:
		GXPhysicsBoss ();

		virtual GXVoid SetPivotOrigin ( const GXVec3 &location, const GXQuat &rotation );
		virtual GXVoid SetShapeOrigin ( GXUShort shapeID, const GXVec3 &location, const GXQuat &rotation );

		GXVoid SetRigidDynamicActor ( PxRigidDynamic* actor );
		GXVoid SetMesh ( GXMesh* mesh );
		PxRigidDynamic* GetRigidDynamicActor ();
		GXMesh* GetMesh ();

	protected:
		GXVoid SetRotation ( GXQuat qt );
		GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );	
		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
};


#endif	//GX_PHYSICS_BOSS