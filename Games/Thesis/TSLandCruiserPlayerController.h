//version 1.4

#ifndef TS_LAND_CRUISER_PLAYER_CONTROLLER
#define TS_LAND_CRUISER_PLAYER_CONTROLLER


#include "TSPlayerControllerExt.h"
#include "TSLandCruiser.h"
#include "TSClient.h"
#include <GXEngineDLL/GXPhysX.h>


class TSLandCruiserPlayerController : public TSPlayerControllerExt, public GXPhysicsActorState
{
	private:
		TSLandCruiser*			car;
		GXInt					health;

		PxVehicleDrive4W*		vehicle;
		GXVec3					bodyConvexCorrection;

		GXQuat					rotationCorrection;

		GXBool					isAccelerate;
		GXBool					isBreak;
		GXBool					isHandbreak;
		GXBool					isTurnLeft;
		GXBool					isTurnRight;

		GXFloat					accelerate;
		GXFloat					breaking;
		GXFloat					turnRight;
		GXFloat					turnLeft;

		GXBool					isAnalogControl;

		GXBool					isReplicated;

	public:
		TSLandCruiserPlayerController ( const GXVec3 &location, const GXMat4 &rotation );
		virtual ~TSLandCruiserPlayerController ();

		virtual GXVoid SetPivotOrigin ( const GXVec3 &location, const GXQuat &rotation );
		virtual GXVoid SetShapeOrigin ( GXUShort shapeID, const GXVec3 &location, const GXQuat &rotation );

		virtual GXVoid Update ( GXFloat deltaTime );
		virtual GXVoid GetLocation ( GXVec3 &location );
		virtual GXVoid GetRotation ( GXMat4 &rotation );

		virtual GXVoid SetOrdinaryWeapon ( TSWeaponAIController* weapon );
		virtual GXVoid SetSpecialWeapon ();

		virtual GXVoid Replicate ();

		GXVoid DigitalStartAccelerate ();
		GXVoid DigitalStartBreak ();
		GXVoid DigitalStartTurnLeft ();
		GXVoid DigitalStartTurnRight ();

		GXVoid DigitalStopAccelerate ();
		GXVoid DigitalStopBreak ();
		GXVoid DigitalStopTurnLeft ();
		GXVoid DigitalStopTurnRight ();

		GXVoid AnalogAccelerate ( GXFloat value );
		GXVoid AnalogBreak ( GXFloat value );
		GXVoid AnalogTurnLeft ( GXFloat value );
		GXVoid AnalogTurnRight ( GXFloat value );

		GXVoid StartHandBreak ();
		GXVoid StopHandBreak ();

		GXVoid Idle ();
		GXVoid Reset ();

	private:
		GXVoid DoDigitalInput ();
		GXVoid DoAnalogInput ();
		GXVoid ReplicateOrigin ();
};


#endif //TS_LAND_CRUISER_PLAYER_CONTROLLER