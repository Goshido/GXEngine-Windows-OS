//version 1.1

#include "TSLandCruiserPhysicsObject.h"
#include "TSGlobals.h"


TSLandCruiserPhysicsObject::TSLandCruiserPhysicsObject ()
{
	isDelete = GX_FALSE;
	actor = 0;
}

GXVoid TSLandCruiserPhysicsObject::Delete ()
{
	isDelete = GX_TRUE;
}

GXVoid TSLandCruiserPhysicsObject::OnPhysics ( GXFloat deltaTime )
{
	if ( isDelete )
	{
		delete this;
		return;
	}

	if ( !actor )
	{
		actor = ts_Physics->CreateRigidDynamic ( 0, PxTransform ( PxVec3 ( 0.0f, 0.0f, 0.0f ) ) );
		PxMaterial* mtr = ts_Physics->CreateMaterial ( 0.8f, 0.5f, 0.4f );
		GXVec3 convexCorrection = GXCreateVec3 ( -0.3f, 0.0f, -0.5f );

		PxConvexMesh* convex = ts_Physics->CreateConvexMesh ( L"../Physics/Thesis/Convex/Land_Cruiser_Body.bke" );
		PxConvexMeshGeometry convexGeometry ( convex );
		PxShape* chassisShape = actor->createShape ( convexGeometry, *mtr );
		chassisShape->setLocalPose ( PxTransform ( PxVec3 ( convexCorrection.x, convexCorrection.y, convexCorrection.z ) ) );

		PxFilterData filterData;
		filterData.word0 = GX_PHYSICS_COLLUSION_GROUP_DRIVABLE;
		filterData.word1 = GX_PHYSICS_COLLUSION_GROUP_CHASSIS | GX_PHYSICS_COLLUSION_GROUP_OBSTACLE | GX_PHYSICS_COLLUSION_GROUP_WHEEL;
		filterData.word3 = GX_PHYSICS_RAYCAST_DRIVABLE_SURFACE;

		chassisShape->setSimulationFilterData ( filterData );
		chassisShape->setQueryFilterData ( filterData );
	
		mtr->release ();

		actor->setRigidDynamicFlag ( PxRigidDynamicFlag::eKINEMATIC, GX_TRUE );

		ts_Physics->AddActor ( *actor );
	}

	actor->setKinematicTarget ( transform );
}

GXVoid TSLandCruiserPhysicsObject::SetOrigin ( const GXVec3 &location, const GXQuat &rotation )
{
	GXQuat physXQuat;
	GXQuatRehandCoordinateSystem ( physXQuat, rotation );

	transform = PxTransform ( PxVec3 ( location.x, location.y, location.z ), PxQuat ( physXQuat.x, physXQuat.y, physXQuat.z, physXQuat.w ) );
}

TSLandCruiserPhysicsObject::~TSLandCruiserPhysicsObject ()
{
	if ( actor )
		actor->release ();
}
