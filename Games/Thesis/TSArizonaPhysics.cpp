//version 1.0

#include "TSArizonaPhysics.h"
#include "TSMesh.h"
#include "TSAIController.h"
#include "TSPhysicsObject.h"
#include "TSGlobals.h"


TSPlayerController*				ts_playerController = 0;
TSMesh*							ts_playerMesh = 0;
TSLandCruiserPlayerController*	ts_LandCruiserPlayerController = 0;


GXVoid GXCALL TSOnPhysics ( GXFloat deltatime )
{
	if ( ts_playerController )
		ts_playerController->Update ( deltatime );

	if ( ts_PlayerController )
		ts_PlayerController->Update ( deltatime );

	TSPhysicsObjectOnPhysics ( deltatime );
	TSAIControllerOnUpdate ( deltatime );
}

GXVoid TSCreatePhysicalPlayer ()
{
	if ( ts_playerController ) return;

	ts_playerController = new TSPlayerController ();
	ts_playerMesh = new TSMesh ( L"../3D Models/Barrel.obj", L"../3D Models/Cached Models/Barrel.cache", L"../Materials/TSRoadBed.mtr", GX_FALSE );
	ts_playerController->SetMesh ( ts_playerMesh );
}

GXVoid TSDestroyPhysicalPlayer ()
{
	GXSafeDelete ( ts_playerController );
	ts_playerMesh->Delete ();
}

GXVoid GXCALL TSPlayerWalkForward ()
{
	if ( ts_playerController )
		ts_playerController->WalkForward ();
}

GXVoid GXCALL TSPlayerWalkBackward ()
{
	if ( ts_playerController )
		ts_playerController->WalkBackward ();
}

GXVoid GXCALL TSPlayerStopWalk ()
{
	if ( ts_playerController )
		ts_playerController->StopWalk ();
}

GXVoid GXCALL TSPlayerRotateLeft ()
{
	if ( ts_playerController )
		ts_playerController->RotateLeft ();
}

GXVoid GXCALL TSPlayerRotateRight ()
{
	if ( ts_playerController )
		ts_playerController->RotateRight ();
}

GXVoid GXCALL TSPlayerStopRotate ()
{
	if ( ts_playerController )
		ts_playerController->StopRotate ();
}

GXVoid GXCALL TSPlayerStrafeLeft ()
{
	if ( ts_playerController )
		ts_playerController->StrafeLeft ();
}

GXVoid GXCALL TSPlayerStrafeRight ()
{
	if ( ts_playerController )
		ts_playerController->StrafeRight ();
}

GXVoid GXCALL TSPlayerStopStrafe ()
{
	if ( ts_playerController )
		ts_playerController->StopStrafe ();
}

GXVoid GXCALL TSPlayerRun ()
{
	if ( ts_playerController )
		ts_playerController->Run ();
}

GXVoid GXCALL TSPlayerStopRun()
{
	if ( ts_playerController )
		ts_playerController->StopRun ();
}

GXVoid GXCALL TSPlayerJump ()
{
	if ( ts_playerController )
		ts_playerController->Jump ();
}

//---------------------------------------------------------------------------------------------------------


GXVoid GXCALL TSLeftStickProc ( GXFloat x, GXFloat y )
{
	if ( !ts_PlayerController ) return;

	if ( x > 0 )
		ts_LandCruiserPlayerController->AnalogTurnRight ( x );
	else
		ts_LandCruiserPlayerController->AnalogTurnLeft ( -x );
}

GXVoid GXCALL TSRightStickProc ( GXFloat x, GXFloat y )
{
	if ( !ts_LandCruiserPlayerController ) return;

	if ( y > 0 )
		ts_LandCruiserPlayerController->AnalogAccelerate ( y );
	else
		ts_LandCruiserPlayerController->AnalogBreak ( -y );
}

GXVoid GXCALL TSStartAccelerate ()
{
	if ( !ts_LandCruiserPlayerController ) return;

	ts_LandCruiserPlayerController->DigitalStartAccelerate ();
}

GXVoid GXCALL TSStartBreak ()
{
	if ( !ts_LandCruiserPlayerController ) return;

	ts_LandCruiserPlayerController->DigitalStartBreak ();
}

GXVoid GXCALL TSStartTurnLeft ()
{
	if ( !ts_LandCruiserPlayerController ) return;

	ts_LandCruiserPlayerController->DigitalStartTurnLeft ();
}

GXVoid GXCALL TSStartTurnRight ()
{
	if ( !ts_LandCruiserPlayerController ) return;

	ts_LandCruiserPlayerController->DigitalStartTurnRight ();
}

GXVoid GXCALL TSStopAccelerate ()
{
	if ( !ts_LandCruiserPlayerController ) return;

	ts_LandCruiserPlayerController->DigitalStopAccelerate ();
}

GXVoid GXCALL TSStopBreak ()
{
	if ( !ts_LandCruiserPlayerController ) return;

	ts_LandCruiserPlayerController->DigitalStopBreak ();
}

GXVoid GXCALL TSStopTurnLeft ()
{
	if ( !ts_LandCruiserPlayerController ) return;

	ts_LandCruiserPlayerController->DigitalStopTurnLeft ();
}

GXVoid GXCALL TSStopTurnRight ()
{
	if ( !ts_LandCruiserPlayerController ) return;

	ts_LandCruiserPlayerController->DigitalStopTurnRight ();
}