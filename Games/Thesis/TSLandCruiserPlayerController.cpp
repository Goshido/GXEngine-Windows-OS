//version 1.4

#include "TSLandCruiserPlayerController.h"
#include "TSNetProtocol.h"
#include "TSGlobals.h"


TSLandCruiserPlayerController::TSLandCruiserPlayerController ( const GXVec3 &location, const GXMat4 &rotation )
{
	car = new TSLandCruiser ();
	health = 1000;

	GXSetQuatRotationAxis ( rotationCorrection, 0.0f, 1.0f, 0.0f, GX_MATH_PI );

	GXVehicleInfo info;
	info.renderCarAddress = this;

	info.spawnLocation = location;
	GXQuat rot;
	GXSetQuatIdentity ( rot );
	info.spawnRotation = rot;

	info.chassisCoMOffset = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
	info.chassisMass = 1500.0f;
	info.chassisMoI = GXCreateVec3 ( 3250.205f, 3250.859f, 875.881f );
	info.bodyBakeFile = L"../Physics/Thesis/Convex/Land_Cruiser_Body.bke";

	bodyConvexCorrection = GXCreateVec3 ( -0.3f, 0.0f, -0.5f );
	info.bodyConvexOffset = bodyConvexCorrection;
	
	info.numWheels = 4;
	GXFloat breaks[ 4 ] = { 1000.0f, 1000.0f, 1000.0f, 1000.0f };
	info.wheelBreaks = breaks;
	GXFloat dampings[ 4 ] = { 0.3f, 0.3f, 0.3f, 0.3f };
	info.wheelDampings = dampings;
	GXFloat handBreaks[ 4 ] = { 0.0f, 0.0f, 4000.0f, 4000.0f };
	info.wheelHandBreaks = handBreaks;
	GXVec3 centres[ 4 ];
	centres[ 0 ] = GXCreateVec3 ( 0.818f, -0.419f, 1.743f );
	centres[ 1 ] = GXCreateVec3 ( -0.818f, -0.419f, 1.743f );
	centres[ 2 ] = GXCreateVec3 ( 0.827f, -0.419f, -1.089f );
	centres[ 3 ] = GXCreateVec3 ( -0.827f, -0.419f, -1.089f );
	info.wheelLocalCentres = centres;
	GXFloat masses[ 4 ] = { 25.0f, 25.0f, 25.0f, 25.0f };
	info.wheelMasses = masses;
	GXFloat momentOfInertia[ 4 ] = { 0.992f, 0.992f, 0.992f, 0.992f };
	info.wheelMoIs = momentOfInertia;
	GXFloat radiuses[ 4 ] = { 0.335f, 0.335f, 0.335f, 0.335f };
	info.wheelRadiuses = radiuses;
	GXFloat steers[ 4 ] = { 0.698f, 0.698f, 0.0f, 0.0f };
	info.wheelSteers = steers;
	GXFloat widths[ 4 ] = { 0.237f, 0.237f, 0.237f, 0.237f };
	info.wheelWidths = widths;

	GXFloat compressions[ 4 ] = { 0.114f, 0.114f, 0.135f, 0.135f };
	info.suspensionCompressions = compressions;
	GXFloat droops[ 4 ] = { 0.128f, 0.128f, 0.14f, 0.14f };
	info.suspensionDroops = droops;
	GXFloat dampers[ 4 ] = { 4500.0f, 4500.0f, 4500.0f, 4500.0f };
	info.suspensionSpringDampers = dampers;
	GXFloat forces[ 4 ] = { 20000.0f, 20000.0f, 20000.0f, 20000.0f };
	info.suspensionSpringForces = forces;
	GXFloat sprungMasses[ 4 ] = { 375.0f, 375.0f, 375.0f, 375.0f };
	info.suspensionSprungMasses = sprungMasses;
	GXVec3 traverses[ 4 ];
	traverses[ 0 ] = GXCreateVec3 ( 0.0f, -1.0f, 0.0f );
	traverses[ 1 ] = GXCreateVec3 ( 0.0f, -1.0f, 0.0f );
	traverses[ 2 ] = GXCreateVec3 ( 0.0f, -1.0f, 0.0f );
	traverses[ 3 ] = GXCreateVec3 ( 0.0f, -1.0f, 0.0f );
	info.suspensionTraverses = traverses;

	info.clutchStrength = 10.0f;

	info.differentialType = GX_PHYSICS_DIFFERENTIAL_LS_4WD;

	info.enginePeakTorque = 600.0f;
	info.engineRotation = 600.04f;

	info.numGears = 7;
	GXFloat ratios[ 7 ] = { -3.52f, 0.0f, 3.52f, 2.042f, 1.4f, 1.0f, 0.716f };
	info.gearboxRatios = ratios;
	info.gearboxSwitchTime = 0.25f;

	info.ackermannAccuracy = 0.8f;
	info.ackermannFrontWidth = 1.636f;
	info.ackermannRearWidth = 1.654f;
	info.ackermannAxleSeparation = 2.261f;

	vehicle = ts_Physics->CreateVehicle ( info );

	isAccelerate = isBreak = isHandbreak = isTurnLeft = isTurnRight = GX_FALSE;

	isAnalogControl = GX_FALSE;

	isReplicated = GX_FALSE;

	Replicate ();
}

TSLandCruiserPlayerController::~TSLandCruiserPlayerController ()
{
	car->Delete ();
	ts_Physics->DeleteVehicle ( vehicle );
}

GXVoid TSLandCruiserPlayerController::SetPivotOrigin ( const GXVec3 &location, const GXQuat &rotation )
{
	//NOTHING
}

GXVoid TSLandCruiserPlayerController::SetShapeOrigin ( GXUShort shapeID, const GXVec3 &location, const GXQuat &rotation )
{
	if ( !car ) return;

	GXVec3 tmp = GXCreateVec3 ( 0.0f, 0.0f, 0.0f );
	GXMat4 rot;

	if ( shapeID < 4 )
	{

		if ( shapeID & 0x0001 )							//Проверка на нечётность		
		{
			GXQuat resultRotation;
			GXMulQuatQuat ( resultRotation, rotationCorrection, rotation );
			rot.From ( resultRotation, tmp );
			car->SetWheelOrigin ( shapeID, location, rot );
		}
		else
		{
			rot.From ( rotation, tmp );
			car->SetWheelOrigin ( shapeID, location, rot );
		}
	}
	else
	{
		GXVec3 offset;
		GXQuatTransform ( offset, rotation, bodyConvexCorrection );

		GXVec3 loc;
		loc.x = location.x - offset.x;
		loc.y = location.y - offset.y;
		loc.z = location.z - offset.z;

		rot.From ( rotation, tmp );

		car->SetBodyOrigin ( loc, rot );
	}
}

GXVoid TSLandCruiserPlayerController::Update ( GXFloat deltaTime )
{
	if ( isAnalogControl )
		DoAnalogInput ();
	else
		DoDigitalInput ();

	if ( isHandbreak )
		ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_HANDBRAKE, 1.0f );
	else
		ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_HANDBRAKE, 0.0f );

	if ( !isReplicated )
		Replicate ();

	ReplicateOrigin ();
}

GXVoid TSLandCruiserPlayerController::GetLocation ( GXVec3 &location )
{
	GXMat4 rot;
	car->GetBodyOrigin ( location, rot );
}

GXVoid TSLandCruiserPlayerController::GetRotation ( GXMat4 &rotation )
{
	GXVec3 loc;
	car->GetBodyOrigin ( loc, rotation );
}

GXVoid TSLandCruiserPlayerController::SetOrdinaryWeapon ( TSWeaponAIController* weapon )
{
	GXSafeDelete ( ordinary );

	ordinary = weapon;
	GXAttachment* attachement = car->GetAttachment ( weapon->GetAttachementName () );
	if ( !attachement ) return;

	ordinary->SetAttachment ( attachement );
	ordinary->SetTarget ( target );
}

GXVoid TSLandCruiserPlayerController::SetSpecialWeapon ()
{
	//TODO
}

GXVoid TSLandCruiserPlayerController::Replicate ()
{
	if ( ts_ClientID == TS_NET_INVALID_ID ) return;

	GXVec3 loc;
	GXMat4 rot;
	GXQuat q;

	car->GetBodyOrigin ( loc, rot );
	q = GXCreateQuat ( rot );

	GXUInt size = sizeof ( TSNetObjectSpawnFrameData ) + sizeof ( TSNetLandCruiserOriginFrameData );
	GXUChar* buffer = (GXUChar*)malloc ( size );

	TSNetObjectSpawnFrameData* spawnData = (TSNetObjectSpawnFrameData*)buffer;
	spawnData->controllerID = TS_NET_LAND_CRUISER_CONTROLLER_ID;

	TSNetLandCruiserOriginFrameData* frameData = (TSNetLandCruiserOriginFrameData*)( buffer + sizeof ( TSNetObjectSpawnFrameData ) );
		
	frameData->bodyLocation = loc;
	frameData->bodyRotation = q;

	for ( GXUChar i = 0; i < 4; i++ )
	{
		car->GetWheelOrigin ( i, loc, rot );
		q = GXCreateQuat ( rot );

		frameData->wheelLocations[ i ] = loc;
		frameData->wheelRotations[ i ] = q;
	}

	TSClientSendTCP ( (GXUInt)this, TS_NET_OBJECT_SPAWN, buffer, size );

	isReplicated = GX_TRUE;

	free ( buffer );
}

GXVoid TSLandCruiserPlayerController::DigitalStartAccelerate ()
{
	isAnalogControl = GX_FALSE;
	isAccelerate = GX_TRUE;
	isBreak = GX_FALSE;
}

GXVoid TSLandCruiserPlayerController::DigitalStartBreak ()
{
	isAnalogControl = GX_FALSE;
	isBreak = GX_TRUE;
	isAccelerate = GX_FALSE;
}

GXVoid TSLandCruiserPlayerController::DigitalStartTurnLeft ()
{
	isAnalogControl = GX_FALSE;
	isTurnLeft = GX_TRUE;
	isTurnRight = GX_FALSE;
}

GXVoid TSLandCruiserPlayerController::DigitalStartTurnRight ()
{
	isAnalogControl = GX_FALSE;
	isTurnRight = GX_TRUE;
	isTurnLeft = GX_FALSE;
}

GXVoid TSLandCruiserPlayerController::DigitalStopAccelerate ()
{
	isAnalogControl = GX_FALSE;
	isAccelerate = GX_FALSE;
}

GXVoid TSLandCruiserPlayerController::DigitalStopBreak ()
{
	isAnalogControl = GX_FALSE;
	isBreak = GX_FALSE;
}

GXVoid TSLandCruiserPlayerController::DigitalStopTurnLeft ()
{
	isAnalogControl = GX_FALSE;
	isTurnLeft = GX_FALSE;
}

GXVoid TSLandCruiserPlayerController::DigitalStopTurnRight ()
{
	isAnalogControl = GX_FALSE;
	isTurnRight = GX_FALSE;
}

GXVoid TSLandCruiserPlayerController::AnalogAccelerate ( GXFloat value )
{
	isAnalogControl = GX_TRUE;

	if ( value < 0.0f )
		value = 0.0f;

	accelerate = value;
	breaking = 0.0f;

}

GXVoid TSLandCruiserPlayerController::AnalogBreak ( GXFloat value )
{
	isAnalogControl = GX_TRUE;

	if ( value < 0.0f )
		value = 0.0f;

	breaking = value;
	accelerate = 0.0f;
}

GXVoid TSLandCruiserPlayerController::AnalogTurnLeft ( GXFloat value )
{
	isAnalogControl = GX_TRUE;

	if ( value < 0.0f )
		value = 0.0f;

	turnLeft = value;
	turnRight = 0.0f;
}

GXVoid TSLandCruiserPlayerController::AnalogTurnRight ( GXFloat value )
{
	isAnalogControl = GX_TRUE;

	if ( value < 0.0f )
		value = 0.0f;

	turnRight = value;
	turnLeft = 0.0f;
}

GXVoid TSLandCruiserPlayerController::StartHandBreak ()
{
	isHandbreak = GX_TRUE;
}

GXVoid TSLandCruiserPlayerController::StopHandBreak ()
{
	isHandbreak = GX_FALSE;
}

GXVoid TSLandCruiserPlayerController::Idle ()
{
	ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_ACCELERATE, 0.0f );
	ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_BRAKE, 0.0f );
	ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_HANDBRAKE, 0.0f );
	ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_TURN_LEFT, 0.0f );
	ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_TURN_RIGHT, 0.0f );
}

GXVoid TSLandCruiserPlayerController::Reset ()
{
	PxRigidDynamic* actor = vehicle->getRigidDynamicActor ();
	PxTransform tr;
	tr = actor->getGlobalPose ();

	GXVec3 location = GXCreateVec3 ( tr.p.x, tr.p.y + 5.0f, tr.p.z );
	GXQuat rotation;
	GXSetQuatIdentity ( rotation );

	ts_Physics->SetRigidActorOrigin ( actor, location, rotation );
}

GXVoid TSLandCruiserPlayerController::DoDigitalInput ()
{
	if ( isAccelerate ) 
		ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_ACCELERATE, 1.0f );
	else
		ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_ACCELERATE, 0.0f );

	if ( isBreak )
		ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_BRAKE, 1.0f );
	else
		ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_BRAKE, 0.0f );

	if ( isTurnLeft )
		ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_TURN_LEFT, 1.0f );
	else
		ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_TURN_LEFT, 0.0f );

	if ( isTurnRight )
		ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_TURN_RIGHT, 1.0f );
	else
		ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_TURN_RIGHT, 0.0f );
}

GXVoid TSLandCruiserPlayerController::DoAnalogInput ()
{
	ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_ACCELERATE, accelerate );
	ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_BRAKE, breaking );
	ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_TURN_RIGHT, turnRight );
	ts_Physics->ControlVehicle ( vehicle, GX_PHYSICS_VEHICLE_TURN_LEFT, turnLeft );
}

GXVoid TSLandCruiserPlayerController::ReplicateOrigin ()
{
	if ( ts_ClientID == 0xFFFFFFFF ) return;
	
	GXVec3 loc;
	GXMat4 rot;
	GXQuat q;

	car->GetBodyOrigin ( loc, rot );
	q = GXCreateQuat ( rot );

	TSNetLandCruiserOriginFrameData frameData;
		
	frameData.bodyLocation = loc;
	frameData.bodyRotation = q;

	for ( GXUChar i = 0; i < 4; i++ )
	{
		car->GetWheelOrigin ( i, loc, rot );
		q = GXCreateQuat ( rot );

		frameData.wheelLocations[ i ] = loc;
		frameData.wheelRotations[ i ] = q;
	}

	TSClientSendUDP ( (GXUInt)this, TS_NET_OBJECT_LAND_CRUISER_ORIGIN, &frameData, sizeof ( TSNetLandCruiserOriginFrameData ) );
}