//version 1.0

#ifndef TS_PLAYER_CONTROLLER
#define TS_PLAYER_CONTROLLER


#include <GXEngineDLL/GXPhysX.h>
#include <GXEngine/GXMesh.h>


enum eTSMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class TSHitReport : public PxUserControllerHitReport
{
	public:
		~TSHitReport ();

		virtual GXVoid onShapeHit ( const PxControllerShapeHit &hit );
		virtual GXVoid onControllerHit ( const PxControllersHit &hit );
		virtual GXVoid onObstacleHit ( const PxControllerObstacleHit &hit );
};

class TSPlayerController
{
	private:
		GXVec3					location;
		GXVec3					rotation;

		GXBool					isWalk;
		GXBool					isRun;
		GXBool					isStrafe;
		GXBool					isRotate;
		GXBool					isJumping;

		eTSMovement				walk;
		eTSMovement				strafe;
		eTSMovement				rotate;

		GXFloat					walkSpeed;
		GXFloat					rotateSpeed;
		GXFloat					runSpeed;
		GXFloat					strafeSpeed;
		GXFloat					jumpSpeed;

		GXFloat					hitImpulse;
		GXFloat					currentVerticalSpeed;

		TSHitReport*			hitReport;
		PxController*			actor;
		GXMesh*					mesh;

		GXFloat					jumpTime;			
		GXFloat					gravityDown;

	public:
		TSPlayerController ();
		virtual ~TSPlayerController (){ /*PURE VIRTUAL*/ };

		GXVoid SetMesh ( GXMesh* mesh );

		GXVoid Update ( GXFloat deltatime );

		GXVoid WalkForward ();
		GXVoid WalkBackward ();
		GXVoid StopWalk ();

		GXVoid Freeze ();
		GXVoid Jump ();

		GXVoid Run ();
		GXVoid StopRun ();

		GXVoid StrafeLeft ();
		GXVoid StrafeRight ();
		GXVoid StopStrafe ();

		GXVoid RotateLeft ();
		GXVoid RotateRight ();
		GXVoid StopRotate ();

		virtual GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		virtual GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );	

		GXFloat GetHitImpulse ();
};


#endif //TS_PLAYER_CONTROLLER