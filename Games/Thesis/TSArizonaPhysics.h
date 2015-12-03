//version 1.0

#ifndef TS_ARIZONA_PHYSICS
#define TS_ARIZONA_PHYSICS


#include <GXCommon/GXTypes.h>
#include "TSMesh.h"
#include "TSPlayerController.h"
#include "TSLandCruiser.h"
#include "TSLandCruiserPlayerController.h"


extern TSMesh*							ts_playerMesh;
extern TSLandCruiserPlayerController*	ts_LandCruiserPlayerController;


GXVoid GXCALL TSOnPhysics ( GXFloat deltatime );

GXVoid TSCreatePhysicalPlayer ();
GXVoid TSDestroyPhysicalPlayer ();

GXVoid GXCALL TSPlayerWalkForward ();;
GXVoid GXCALL TSPlayerWalkBackward ();
GXVoid GXCALL TSPlayerStopWalk ();
GXVoid GXCALL TSPlayerRotateLeft ();
GXVoid GXCALL TSPlayerRotateRight ();
GXVoid GXCALL TSPlayerStopRotate ();
GXVoid GXCALL TSPlayerStrafeLeft ();
GXVoid GXCALL TSPlayerStrafeRight ();
GXVoid GXCALL TSPlayerStopStrafe ();
GXVoid GXCALL TSPlayerRun ();
GXVoid GXCALL TSPlayerStopRun();
GXVoid GXCALL TSPlayerJump ();

GXVoid GXCALL TSLeftStickProc ( GXFloat x, GXFloat y );
GXVoid GXCALL TSRightStickProc ( GXFloat x, GXFloat y );
GXVoid GXCALL TSStartAccelerate ();
GXVoid GXCALL TSStartBreak ();
GXVoid GXCALL TSStartTurnLeft ();
GXVoid GXCALL TSStartTurnRight ();
GXVoid GXCALL TSStopAccelerate ();
GXVoid GXCALL TSStopBreak ();
GXVoid GXCALL TSStopTurnLeft ();
GXVoid GXCALL TSStopTurnRight ();


#endif //TS_ARIZONA_PHYSICS