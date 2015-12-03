//version 1.1

#ifndef GX_PLAYER_BOSS
#define GX_PLAYER_BOSS


#include "GXGlobals.h"
#include "GXSkeletalMesh.h"
#include "GXAnimSolverMovement.h"
#include "GXPhysics.h"


enum eMovement
{
	BOSS_FORWARD,
	BOSS_BACKWARD,
	BOSS_LEFT,
	BOSS_RIGHT
};


class GXHitReportBase : public PxUserControllerHitReport
{
	public:
		~GXHitReportBase ();

		virtual GXVoid onShapeHit ( const PxControllerShapeHit& hit );
		virtual GXVoid onControllerHit ( const PxControllersHit& hit );
		virtual GXVoid onObstacleHit ( const PxControllerObstacleHit& hit );
};

class GXPlayerBoss
{
	protected:
		GXVec3					location;
		GXVec3					rotation;

		GXBool					bIsWalk;
		GXBool					bIsRun;
		GXBool					bIsStrafe;
		GXBool					bIsRotate;
		GXBool					bIsJumping;
		eMovement				eWalk;
		eMovement				eStrafe;
		eMovement				eRotate;
		GXDword					lasttime;
		GXFloat					fWalkSpeed;
		GXFloat					fRotateSpeed;
		GXFloat					fRunSpeed;
		GXFloat					fStrafeSpeed;
		GXFloat					fJumpSpeed;
		GXFloat					fHitImpulse;
		GXFloat					fCurrentVerticalSpeed;
		GXHitReportBase*		hitReport;
		PxController*			actor;
		GXFloat					fSimulationDelay;
		GXFloat					fJumpTime;			
		GXFloat					fToleranceLength;
		GXVec3					gravity;
		GXSkeletalMesh*			mesh;
		GXAnimSolverMovement*	animSolver;

	public:
		GXPlayerBoss ();
		virtual ~GXPlayerBoss ();

		virtual GXVoid Pick () = 0;
		virtual GXVoid UnPick () = 0;
		virtual GXVoid SetMesh ( GXSkeletalMesh* mesh ) = 0;
		virtual GXVoid SetAnimSolver ( GXAnimSolverMovement* animSolver );

		virtual GXVoid Perform ( GXFloat deltatime );
		virtual GXVoid WalkForward ();
		virtual GXVoid WalkBackward ();
		virtual GXVoid StopWalk ();
		virtual GXVoid Freeze ();
		virtual GXVoid Run ();
		virtual GXVoid StopRun ();
		virtual GXVoid StrafeLeft ();
		virtual GXVoid StrafeRight ();
		virtual GXVoid StopStrafe ();
		virtual GXVoid RotateLeft ();
		virtual GXVoid RotateRight ();
		virtual GXVoid StopRotate ();
		virtual GXVoid Jump ();

		virtual GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		virtual GXVoid GetLocation ( GXVec3* out );

		virtual GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );
		virtual GXVoid SetRotation ( GXQuat qt );
		virtual GXVoid GetRotation ( GXVec3* out_rad );

		GXFloat GetHitImpulse ();
};


#endif //GX_PLAYER_BOSS