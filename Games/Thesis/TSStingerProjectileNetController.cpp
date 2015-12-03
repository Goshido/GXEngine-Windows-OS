//version 1.0

#include "TSStingerProjectileNetController.h"
#include <GXEngine/GXGlobals.h>
#include "TSClient.h"
#include "TSNetProtocol.h"


TSStingerProjectileNetController::TSStingerProjectileNetController ( GXUInt clientID, GXUInt objectID, const GXVec3 &location, const GXQuat &rotation ):
TSNetController ( clientID, objectID )
{
	GXVec3 temp ( 0.0f, 0.0f, 0.0f );
	GXMat4 rot;
	rot.From ( rotation, temp );

	projectile = new TSStingerProjectile ( location, rot );
}

TSStingerProjectileNetController::~TSStingerProjectileNetController ()
{
	projectile->Delete ();
}

GXVoid TSStingerProjectileNetController::OnReceive ( const GXVoid* data, GXUInt size )
{
	TSNetFrameHeader* header = (TSNetFrameHeader*)data;
	const GXVoid* frameData = (GXVoid*)( (GXUChar*)data + sizeof ( TSNetFrameHeader ) );

	switch ( header->dataType )
	{
		case TS_NET_OBJECT_STINGER_PROJECTILE_ORIGIN:
		{
			TSNetStingerProjectileOriginFrameData* frame = (TSNetStingerProjectileOriginFrameData*)frameData;
			SetOrigin ( frame->location, frame->rotation );	
		}
		break;

		case TS_NET_OBJECT_STINGER_PROJECTILE_EXPLOSED:
		{
			projectile->Explose ();
			TSClientDeleteNetController ( clientID, objectID );
		}
		break;
	}
}

GXVoid TSStingerProjectileNetController::SetOrigin ( const GXVec3 &location, const GXQuat &rotation )
{
	GXVec3 tmp ( 0.0f, 0.0f, 0.0f );
	GXMat4 rot;
	rot.From ( rotation, tmp );

	projectile->SetOrigin ( location, rot );
}