//version 1.1

#include "GXBossTwilightSparkle.h"


GXBossTwilightSparkle::GXBossTwilightSparkle ()
{
	bIsWalk = bIsRun = bIsStrafe = bIsRotate = bIsJumping = GX_FALSE;
	fStrafeSpeed = 20.0f;
	fWalkSpeed = 20.0f;
	fRunSpeed = 60.0f;
	fRotateSpeed = 3.0f;
	fJumpSpeed = 1.5f;
	fHitImpulse = 5.0f;
	fCurrentVerticalSpeed = 0.0f;
}

GXVoid GXBossTwilightSparkle::SetMesh ( GXSkeletalMesh* mesh )
{
	this->mesh = mesh;

	GXPhysics* physics = gx_Core->GetPhysics ();
	PxCapsuleControllerDesc desc;
	hitReport = new GXHitReportTwilightSparkle ();
	desc.height = 3.0f;
	PxMaterial* mat = physics->CreateMaterial ( 100.0f, 500.0f, 0.1f );
	desc.material = mat;
	desc.callback = hitReport;
	desc.behaviorCallback = 0;
	desc.climbingMode = PxCapsuleClimbingMode::eEASY;
	desc.contactOffset = 0.02f;
	desc.density = 10.0f;
	desc.maxJumpHeight = 3.0f;
	desc.radius = 1.2f;
	desc.position = PxExtendedVec3 ( 12.0f, 15.0f, 0.0f  );
	desc.userData = this;
	actor = physics->CreateCharacterController ( desc );
}

GXVoid GXBossTwilightSparkle::Pick ()
{
	//NOTHING
}

GXVoid GXBossTwilightSparkle::UnPick ()
{
	//NOTHING
}

//----------------------------------------------------------

GXVoid GXHitReportTwilightSparkle::onShapeHit ( const PxControllerShapeHit& hit )
{
	if ( !( hit.shape->getActor ().isRigidDynamic () ) ) return;
	PxRigidDynamic* act = static_cast< PxRigidDynamic* >( &( hit.shape->getActor () ) );
	GXPlayerBoss* boss = static_cast< GXPlayerBoss* >( hit.controller->getUserData () );
	act->addForce ( PxVec3 ( hit.worldNormal * boss->GetHitImpulse () ), PxForceMode::eIMPULSE );
}