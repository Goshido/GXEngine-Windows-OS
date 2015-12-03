//version 1.1

#include "GXBossRainbowDash.h"


GXBossRainbowDash::GXBossRainbowDash ()
{
	bIsWalk = bIsRun = bIsStrafe = bIsRotate = bIsJumping = GX_FALSE;
	fStrafeSpeed = 0.02f;
	fWalkSpeed = 0.02f;
	fRunSpeed = 0.06f;
	fRotateSpeed = 0.003f;
	fJumpSpeed = 1.0f;
	fHitImpulse = 5.0f;
	fCurrentVerticalSpeed = 0.0f;
}

GXVoid GXBossRainbowDash::SetMesh ( GXSkeletalMesh* mesh )
{
	this->mesh = mesh;
	GXPhysics* physics = gx_Core->GetPhysics ();
	PxCapsuleControllerDesc desc;
	hitReport = new GXHitReportRainbowDash ();
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
	desc.position = PxExtendedVec3 ( 0.0f, 15.0f, 0.0f  );
	desc.userData = this;
	actor = physics->CreateCharacterController ( desc );
}

GXVoid GXBossRainbowDash::Pick ()
{
	//( (GXMSkeletalRainbowDash*)mesh )->Pick ();
}

GXVoid GXBossRainbowDash::UnPick ()
{
	//NOTHING
}

//----------------------------------------------------------

GXVoid GXHitReportRainbowDash::onShapeHit ( const PxControllerShapeHit& hit )
{
	if ( !( hit.shape->getActor ().isRigidDynamic () ) ) return;
	PxRigidDynamic* act = static_cast< PxRigidDynamic* >( &( hit.shape->getActor () ) );
	GXPlayerBoss* boss = static_cast< GXPlayerBoss* >( hit.controller->getUserData () );
	act->addForce ( PxVec3 ( hit.worldNormal * boss->GetHitImpulse () ), PxForceMode::eIMPULSE );
}