//version 1.0

#include "TSLandCruiserNetController.h"


TSLandCruiserNetController::TSLandCruiserNetController ( GXUInt clientID, GXUInt objectID ):
TSNetController ( clientID, objectID )
{
	car = new TSLandCruiser ();
	actor = new TSLandCruiserPhysicsObject ();
}

TSLandCruiserNetController::~TSLandCruiserNetController ()
{
	car->Delete ();
	actor->Delete ();
}

GXVoid TSLandCruiserNetController::OnReceive ( const GXVoid* data, GXUInt size )
{
	TSNetFrameHeader* header = (TSNetFrameHeader*)data;
	const GXVoid* frameData = (GXVoid*)( (GXUChar*)data + sizeof ( TSNetFrameHeader ) );

	switch ( header->dataType )
	{
		case TS_NET_OBJECT_LAND_CRUISER_ORIGIN:
		{
			TSNetLandCruiserOriginFrameData* frame = (TSNetLandCruiserOriginFrameData*)frameData;

			SetBodyOrigin ( frame->bodyLocation, frame->bodyRotation );	

			for ( GXUChar i = 0; i < 4; i++ )
				SetWheelOrigin ( i, frame->wheelLocations[ i ], frame->wheelRotations[ i ] );
		}
		break;

		case TS_NET_PLAYER_DISCONNECT:
		{
			car->Delete ();
		}
		break;
	}
}

GXVoid TSLandCruiserNetController::SetBodyOrigin ( const GXVec3 &location, const GXQuat &rotation )
{
	GXVec3 tmp ( 0.0f, 0.0f, 0.0f );
	GXMat4 rot;
	rot.From ( rotation, tmp );

	car->SetBodyOrigin ( location, rot );
	actor->SetOrigin ( location, rotation );
}

GXVoid TSLandCruiserNetController::SetWheelOrigin ( GXUShort id, const GXVec3 &location, const GXQuat &rotation )
{
	GXVec3 tmp ( 0.0f, 0.0f, 0.0f );
	GXMat4 rot;
	rot.From ( rotation, tmp );

	car->SetWheelOrigin ( id, location, rot );
}