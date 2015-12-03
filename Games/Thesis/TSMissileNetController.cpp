//version 1.0

#include "TSMissileNetController.h"
#include <GXEngine/GXGlobals.h>


TSMissileNetController::TSMissileNetController ( GXUInt clientID, GXUInt objectID, const GXVec3 &location, const GXQuat &rotation ):
TSNetController ( clientID, objectID )
{
	missile = new TSMissile ();
	SetOrigin ( location, rotation );
}

TSMissileNetController::~TSMissileNetController ()
{
	missile->Delete ();
}

GXVoid TSMissileNetController::OnReceive ( const GXVoid* data, GXUInt size )
{
	TSNetFrameHeader* header = (TSNetFrameHeader*)data;
	const GXVoid* frameData = (const GXVoid*)( (GXUChar*)data + sizeof ( TSNetFrameHeader ) );

	switch ( header->dataType )
	{
		case TS_NET_OBJECT_MISSILE_ORIGIN:
		{
			TSNetMissileOriginFrameData* frame = (TSNetMissileOriginFrameData*)frameData;
			SetOrigin ( frame->location, frame->rotation );	
		}
		break;
	}
}

GXVoid TSMissileNetController::SetOrigin ( const GXVec3 &location, const GXQuat &rotation )
{
	GXVec3 tmp ( 0.0f, 0.0f, 0.0f );
	GXMat4 rot;
	rot.From ( rotation, tmp );

	missile->SetOrigin ( location, rot );
}