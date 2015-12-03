//version 1.0

#include "TSCannonballProjectileNetController.h"
#include <GXEngine/GXGlobals.h>
#include "TSClient.h"
#include "TSNetProtocol.h"


TSCannonballProjectileNetController::TSCannonballProjectileNetController ( GXUInt clientID, GXUInt objectID, const GXVec3 &location ):
TSNetController ( clientID, objectID )
{
	GXMat4 rot;
	GXSetMat4Identity ( rot );

	projectile = new TSCannonballProjectile ( location, rot );
}

TSCannonballProjectileNetController::~TSCannonballProjectileNetController ()
{
	projectile->Delete ();

}

GXVoid TSCannonballProjectileNetController::OnReceive ( const GXVoid* data, GXUInt size )
{
	TSNetFrameHeader* header = (TSNetFrameHeader*)data;
	const GXVoid* frameData = (GXVoid*)( (GXUChar*)data + sizeof ( TSNetFrameHeader ) );

	switch ( header->dataType )
	{
		case TS_NET_OBJECT_CANNONBALL_PROJECTILE_ORIGIN:
		{
			TSNetCannonballProjectileOriginFrameData* frame = (TSNetCannonballProjectileOriginFrameData*)frameData;
			SetOrigin ( frame->location );	
		}
		break;

		case TS_NET_OBJECT_CANNONBALL_PROJECTILE_EXPLOSED:
		{
			projectile->Explose ();
			TSClientDeleteNetController ( clientID, objectID );
		}
		break;
	}
}

GXVoid TSCannonballProjectileNetController::SetOrigin ( const GXVec3 &location )
{
	GXMat4 rot;
	GXSetMat4Identity ( rot );

	projectile->SetOrigin ( location, rot );
}