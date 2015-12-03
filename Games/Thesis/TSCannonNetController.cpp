//version 1.0

#include "TSCannonNetController.h"
#include <GXEngine/GXGlobals.h>


TSCannonNetController::TSCannonNetController ( GXUInt clientID, GXUInt objectID ):
TSNetController ( clientID, objectID )
{
	cannon = new TSCannon ();
}

TSCannonNetController::~TSCannonNetController ()
{
	cannon->Delete ();
}

GXVoid TSCannonNetController::OnReceive ( const GXVoid* data, GXUInt size )
{
	TSNetFrameHeader* header = (TSNetFrameHeader*)data;
	const GXVoid* frameData = (const GXVoid*)( (GXUChar*)data + sizeof ( TSNetFrameHeader ) );

	switch ( header->dataType )
	{
		case TS_NET_OBJECT_CANNON_ORIGIN:
		{
			TSNetCannonOriginFrameData* frame = (TSNetCannonOriginFrameData*)frameData;
			SetOrigin ( frame->location, frame->rotation );	
		}
		break;

		case TS_NET_OBJECT_CANNON_FIRE:
		{
			cannon->Fire ();
		}
		break;
	}
}

GXVoid TSCannonNetController::SetOrigin ( const GXVec3 &location, const GXQuat &rotation )
{
	GXVec3 tmp ( 0.0f, 0.0f, 0.0f );
	GXMat4 rot;
	rot.From ( rotation, tmp );

	cannon->SetOrigin ( location, rot );
}