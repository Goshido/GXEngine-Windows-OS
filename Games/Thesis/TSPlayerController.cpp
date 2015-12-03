//version 1.0

#include "TSPlayerController.h"
#include <GXEngine/GXGlobals.h>


TSHitReport::~TSHitReport ()
{
	//NOTHING
}

GXVoid TSHitReport::onShapeHit ( const PxControllerShapeHit &hit )
{
	if ( !( hit.shape->getActor ().isRigidDynamic () ) ) return;

	PxRigidDynamic* act = static_cast< PxRigidDynamic* >( &( hit.shape->getActor () ) );
	TSPlayerController* controller = static_cast< TSPlayerController* >( hit.controller->getUserData () );
	act->addForce ( PxVec3 ( hit.worldNormal * controller->GetHitImpulse () ), PxForceMode::eIMPULSE );
}

GXVoid TSHitReport::onControllerHit ( const PxControllersHit &hit )
{
	//NOTHING
}

GXVoid TSHitReport::onObstacleHit ( const PxControllerObstacleHit &hit )
{
	//NOTHING
}

//------------------------------------------------------------------

TSPlayerController::TSPlayerController ()
{
	mesh = 0;
	actor = 0;

	currentVerticalSpeed = 0.0f;
	gravityDown = GX_PHYSICS_GRAVITY_FACTOR * 0.5f;
	hitImpulse = 5.0f;	

	rotation = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
	isWalk = isRun = isStrafe = isRotate = isJumping = GX_FALSE;

	strafeSpeed = 20.0f;
	walkSpeed = 20.0f;
	runSpeed = 60.0f;
	rotateSpeed = 3.0f;
	jumpSpeed = 1.0f;
}

GXVoid TSPlayerController::SetMesh ( GXMesh* mesh )
{
	this->mesh = mesh;
	GXPhysics* physics = gx_Core->GetPhysics ();

	hitReport = new TSHitReport ();
	PxMaterial* mat = physics->CreateMaterial ( 100.0f, 500.0f, 0.1f );

	PxCapsuleControllerDesc desc;
	desc.height = 3.333f - 2.041f;
	desc.material = mat;
	desc.callback = hitReport;
	desc.behaviorCallback = 0;
	desc.climbingMode = PxCapsuleClimbingMode::eEASY;
	desc.contactOffset = 0.1f;
	desc.density = 10.0f;
	desc.maxJumpHeight = 0.0f;
	desc.radius = 2.041f;
	desc.position = PxExtendedVec3 ( 0.0f, 15.0f, 0.0f );
	desc.userData = this;

	actor = physics->CreateCharacterController ( desc );
}

PxControllerFilters ts_PlayerControllerFilters;

GXVoid TSPlayerController::Update ( GXFloat deltatime )
{
	if ( !actor ) return;

	PxExtendedVec3 pos = actor->getFootPosition ();
	SetLocation ( (GXFloat)pos.x, (GXFloat)pos.y, (GXFloat)pos.z );

	jumpTime += deltatime;

	GXFloat speed;
	if ( isRotate )
	{
		speed = ( ( rotate == LEFT ) ? -rotateSpeed : rotateSpeed ) * deltatime;
		rotation.yaw_rad += speed;
		mesh->SetRotation ( rotation );
	}

	PxVec3 resultDir;
	resultDir.x = resultDir.y = resultDir.z = 0.0f;

	if ( isWalk )
	{
		speed = ( isRun ? runSpeed : walkSpeed ) * deltatime;
		speed = ( walk == FORWARD ) ? -speed : speed;
		resultDir.z = cos ( rotation.yaw_rad ) * speed;
		resultDir.x = sin ( rotation.yaw_rad ) * speed;
	}

	if ( isStrafe )
	{
		speed = ( ( strafe == LEFT ) ? strafeSpeed : -strafeSpeed ) * deltatime;
		resultDir.z += cos ( rotation.yaw_rad + GX_MATH_HALFPI ) * speed;
		resultDir.x += sin ( rotation.yaw_rad + GX_MATH_HALFPI ) * speed;
	}

	currentVerticalSpeed += deltatime * gravityDown;
	resultDir.y = currentVerticalSpeed;

	PxU32 doEvent = actor->move ( resultDir, 0.03f, deltatime, ts_PlayerControllerFilters );
	switch ( doEvent )
	{
		case PxControllerFlag::eCOLLISION_DOWN:
			isJumping = GX_FALSE;
			currentVerticalSpeed = 0.0f;
			jumpTime = 0;
		break;

		case PxControllerFlag::eCOLLISION_UP:
			currentVerticalSpeed = 0.0f;
			jumpTime = 0;
		break;
	}
}

GXVoid TSPlayerController::WalkForward ()
{
	if ( isWalk && ( walk == FORWARD ) ) return;

	isWalk = GX_TRUE;
	walk = FORWARD;
}

GXVoid TSPlayerController::WalkBackward ()
{
	if ( isWalk && ( walk == BACKWARD ) ) return;

	isWalk = GX_TRUE;
	isRun = GX_FALSE;
	walk = BACKWARD;
}

GXVoid TSPlayerController::StopWalk ()
{
	if ( !isWalk ) return;
	isWalk = GX_FALSE;
}

GXVoid TSPlayerController::Freeze ()
{
	if ( !isWalk && !isRun && !isStrafe && !isRotate ) return;
	isWalk = isRun = isStrafe = isRotate = GX_FALSE;
}

GXVoid TSPlayerController::Jump ()
{
	if ( isJumping ) return;

	isJumping = GX_TRUE;
	currentVerticalSpeed = jumpSpeed;
	jumpTime = 0;
}

GXVoid TSPlayerController::Run ()
{
	if ( !isWalk || ( walk != FORWARD ) || isRun ) return;
	isRun = GX_TRUE;
}

GXVoid TSPlayerController::StopRun ()
{
	if ( !isRun ) return;
	isRun = GX_FALSE;
}

GXVoid TSPlayerController::StrafeLeft ()
{
	if ( isStrafe && ( strafe == LEFT )  ) return;

	strafe = LEFT;
	isStrafe = GX_TRUE;
}

GXVoid TSPlayerController::StrafeRight ()
{
	if ( isStrafe && ( strafe == RIGHT ) ) return;

	strafe = RIGHT;
	isStrafe = GX_TRUE;
}

GXVoid TSPlayerController::StopStrafe ()
{
	isStrafe = GX_FALSE;
}

GXVoid TSPlayerController::RotateLeft ()
{
	if ( isRotate && ( rotate == LEFT ) ) return;

	rotate = LEFT;
	isRotate = GX_TRUE;
}

GXVoid TSPlayerController::RotateRight ()
{
	if ( isRotate && ( rotate == RIGHT ) ) return;

	rotate = RIGHT;
	isRotate = GX_TRUE;
}

GXVoid TSPlayerController::StopRotate ()
{
	if ( !isRotate ) return;
	isRotate = GX_FALSE;
}

GXVoid TSPlayerController::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	location.x = x;
	location.y = y;
	location.z = z;

	if ( mesh )
		mesh->SetLocation ( location.v );
}

GXVoid TSPlayerController::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	rotation.pitch_rad = pitch_rad;
	rotation.yaw_rad = yaw_rad;
	rotation.roll_rad = roll_rad;

	if ( mesh )
		mesh->SetRotation ( rotation.v );
}

GXFloat TSPlayerController::GetHitImpulse ()
{
	return hitImpulse;
}