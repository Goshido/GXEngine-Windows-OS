//version 1.2

#include "TSClient.h"
#include "TSNetProtocol.h"
#include "TSNetController.h"
#include "TSGlobals.h"
#include "TSAIController.h"
#include <GXEngine/GXNetwork.h>
#include <GXCommon/GXMutex.h>
#include <GXCommon/GXStrings.h>

#include "TSLandCruiserNetController.h"
#include "TSCannonNetController.h"
#include "TSCannonballProjectileNetController.h"
#include "TSMissileNetController.h"
#include "TSStingerProjectileNetController.h"


GXVoid GXCALL TSClientOnMessageTCP ( const GXVoid* data, GXUInt size );
GXVoid GXCALL TSClientOnMessageUDP ( const GXVoid* data, GXUInt size );


GXNetClient*	ts_Client = 0;
GXUInt			ts_ClientID = TS_NET_INVALID_ID;

GXUChar			ts_clientBufferTCP[ 1024 * 1024 * 4 ];
GXUChar			ts_clientBufferUDP[ 1024 * 1024 * 4 ];

GXMutex*		ts_clientMutexTCP = 0;
GXMutex*		ts_clientMutexUDP = 0;

//---------------------------------------------------------------------------------------------

struct TSClientObjectRecord
{
	TSClientObjectRecord*	prev;
	TSClientObjectRecord*	next;

	TSNetController*		controller;
};

struct TSClientTable
{
	GXBool					isExist;
	TSClientObjectRecord*	records;	
};

TSClientTable ts_clientTable[ GX_MAX_NETWORK_CLIENTS ];

GXVoid GXCALL TSClientAddNetController ( GXUInt clientID, TSNetController* controller )
{
	ts_clientMutexTCP->Lock ();

	if ( !ts_clientTable[ clientID ].isExist )
		ts_clientTable[ clientID ].isExist = GX_TRUE;

	TSClientObjectRecord* record = new TSClientObjectRecord ();
	record->controller = controller;
	record->prev = 0;

	TSClientObjectRecord* top = ts_clientTable[ clientID ].records;
	if ( top ) 
		top->prev = record;

	record->next = top;
	ts_clientTable[ clientID ].records = record;

	ts_clientMutexTCP->Release ();
}

TSNetController* GXCALL TSClientFindNetController ( GXUInt clientID, GXUInt objectID )
{
	ts_clientMutexTCP->Lock ();

	if ( !ts_clientTable[ clientID ].isExist ) 
	{
		ts_clientMutexTCP->Release ();
		return 0;
	}

	for ( TSClientObjectRecord* p = ts_clientTable[ clientID ].records; p; p = p->next )
	{
		if ( p->controller->GetObjectID () == objectID )
		{
			ts_clientMutexTCP->Release ();
			return p->controller;
		}
	}

	ts_clientMutexTCP->Release ();
	return 0;
}

TSNetController* GXCALL TSClientFindAnyNetController ( GXUInt clientID )
{
	ts_clientMutexTCP->Lock ();

	if ( !ts_clientTable[ clientID ].isExist ) 
	{
		ts_clientMutexTCP->Release ();
		return 0;
	}

	ts_clientMutexTCP->Release ();
	return ts_clientTable[ clientID ].records->controller;
}

GXVoid GXCALL TSClientDeleteNetController ( GXUInt clientID, GXUInt objectID )
{
	ts_clientMutexTCP->Lock ();

	if ( !ts_clientTable[ clientID ].isExist ) 
	{
		ts_clientMutexTCP->Release ();
		return;
	}

	TSClientObjectRecord* p;
	for ( p = ts_clientTable[ clientID ].records; p; p = p->next )
	{
		if ( p->controller->GetObjectID () == objectID )
			break;
	}

	if ( !p ) 
	{
		ts_clientMutexTCP->Release ();
		return;
	}

	delete p->controller;

	if ( p->next ) p->next->prev = p->prev;

	if ( p->prev )
		p->prev->next = p->next;
	else
	{
		ts_clientTable[ clientID ].records = p->next;

		if ( !ts_clientTable[ clientID ].records )
			ts_clientTable[ clientID ].isExist = GX_FALSE;
	}

	delete p;

	ts_clientMutexTCP->Release ();
}

GXVoid GXCALL TSClientReplicate ()
{
	if ( ts_PlayerController )
		ts_PlayerController->Replicate ();

	TSAIControllerOnReplicate ();
}

//---------------------------------------------------------------------------------------------

GXVoid GXCALL TSClientInit ( GXBool isHost )
{
	ts_clientMutexTCP = new GXMutex ();
	ts_clientMutexUDP = new GXMutex ();

	for ( GXUInt i = 0; i < GX_MAX_NETWORK_CLIENTS; i++ )
	{
		ts_clientTable[ i ].isExist = GX_FALSE;
		ts_clientTable[ i ].records = 0;
	}

	ts_Client = gx_Core->GetNetClient ();
	ts_ClientID = TS_NET_INVALID_ID;

	ts_Client->SetOnMessageTCPFunc ( TSClientOnMessageTCP );
	ts_Client->SetOnMessageUDPFunc ( TSClientOnMessageUDP );

	GXWChar* urlW;
	GXChar* urlM;

	if ( isHost )
	{
		urlM = "localhost";
	}
	else
	{
		GXUInt size;
		GXLoadFile ( L"../Config/Thesis/TSNetwork.cfg", (GXVoid**)&urlW, size, GX_TRUE );

		GXToUTF8 ( &urlM, urlW );
	}

	if ( !ts_Client->ConnectTCP ( urlM, TS_NET_TCP_PORT ) )
		GXLogW ( L"TSClientInit::Error - Не удалось подключиться к серверу по TCP\n" );

	if ( !ts_Client->DeployUDP ( urlM, TS_NET_UDP_PORT ) )
		GXLogW ( L"TSClientInit::Error - Не удалось создать UDP\n" );

	if ( !isHost )
	{
		free ( urlW );
		free ( urlM );
	}
}

GXVoid GXCALL TSClientSendTCP ( GXUInt objectID, GXUInt dataType, const GXVoid* data, GXUInt size )
{
	TSNetFrameHeader* frame = (TSNetFrameHeader*)ts_clientBufferTCP;
	frame->clientID = ts_ClientID;
	frame->objectID = objectID;
	frame->dataType = dataType;

	memcpy ( ts_clientBufferTCP + sizeof ( TSNetFrameHeader ), data, size );

	ts_Client->SendTCP ( ts_clientBufferTCP, sizeof ( TSNetFrameHeader ) + size );
}

GXVoid GXCALL TSClientSendUDP ( GXUInt objectID, GXUInt dataType, const GXVoid* data, GXUInt size )
{
	TSNetFrameHeader* frame = (TSNetFrameHeader*)ts_clientBufferTCP;
	frame->clientID = ts_ClientID;
	frame->objectID = objectID;
	frame->dataType = dataType;

	memcpy ( ts_clientBufferTCP + sizeof ( TSNetFrameHeader ), data, size );

	ts_Client->SendUDP ( ts_clientBufferTCP, sizeof ( TSNetFrameHeader ) + size );
}

GXVoid GXCALL TSClientDestroy ()
{
	if ( ts_ClientID == TS_NET_INVALID_ID ) return;

	ts_Client->DisconnectTCP ();
	ts_Client->DestroyUDP ();

	GXSafeDelete ( ts_clientMutexTCP );
	GXSafeDelete ( ts_clientMutexUDP );

	ts_ClientID = TS_NET_INVALID_ID;
}

//-----------------------------------------------------------------------------------------------

GXVoid GXCALL TSClientSpawnNetController ( const GXVoid* data, GXUInt size );

GXVoid GXCALL TSClientOnMessageTCP ( const GXVoid* data, GXUInt size )
{
	ts_clientMutexTCP->Lock ();

	TSNetFrameHeader* frameHeader = (TSNetFrameHeader*)data;

	if ( ts_ClientID == TS_NET_INVALID_ID )
	{
		ts_ClientID = frameHeader->clientID;

		GXLogW ( L"TSClientOnMessageTCP::Info - Мой ID клиента %i\n", ts_ClientID );

		ts_clientMutexTCP->Release ();
		return;
	}

	switch ( frameHeader->dataType ) 
	{
		case TS_NET_OBJECT_SPAWN:
		{
			TSClientSpawnNetController ( data, size );
		}
		break;

		case TS_NET_PLAYER_DISCONNECT:
		{
			TSNetController* controller = TSClientFindAnyNetController ( frameHeader->clientID );
			while ( controller )
			{
				controller->OnReceive ( data, size );
				TSClientDeleteNetController ( frameHeader->clientID, controller->GetObjectID () );
				controller = TSClientFindAnyNetController ( frameHeader->clientID );
			}
		}
		break;

		case TS_NET_OBJECT_DESTROY:
		{
			TSClientDeleteNetController ( frameHeader->clientID, frameHeader->objectID );
		}
		break;

		case TS_NET_OBJECT_CANNONBALL_PROJECTILE_EXPLOSED:
		case TS_NET_OBJECT_CANNON_FIRE:
		case TS_NET_OBJECT_STINGER_PROJECTILE_EXPLOSED:
		{
			TSNetController* controller = (TSNetController*)TSClientFindNetController ( frameHeader->clientID, frameHeader->objectID );

			if ( !controller )
			{
				ts_clientMutexTCP->Release ();
				return;
			}

			controller->OnReceive ( data, size );
		}
		break;

		default:
		break;
	}

	ts_clientMutexTCP->Release ();
}

GXVoid GXCALL TSClientOnMessageUDP ( const GXVoid* data, GXUInt size )
{
	ts_clientMutexUDP->Lock ();

	TSNetFrameHeader* frameHeader = (TSNetFrameHeader*)data;

	switch ( frameHeader->dataType )
	{
		case TS_NET_OBJECT_LAND_CRUISER_ORIGIN:
		case TS_NET_OBJECT_CANNON_ORIGIN:
		case TS_NET_OBJECT_CANNONBALL_PROJECTILE_ORIGIN:
		case TS_NET_OBJECT_MISSILE_ORIGIN:
		case TS_NET_OBJECT_STINGER_PROJECTILE_ORIGIN:
		{
			TSNetController* controller = (TSNetController*)TSClientFindNetController ( frameHeader->clientID, frameHeader->objectID );

			if ( !controller )
			{
				ts_clientMutexUDP->Release ();
				return;
			}

			controller->OnReceive ( data, size );
		}
		break;

		default:
		break;
	}

	ts_clientMutexUDP->Release ();
}

//--------------------------------------------------------------------------------------------

GXVoid GXCALL TSClientSpawnNetController ( const GXVoid* data, GXUInt size )
{
	TSNetFrameHeader* frameHeader = (TSNetFrameHeader*)data;

	if ( TSClientFindNetController ( frameHeader->clientID, frameHeader->objectID ) ) return;

	TSNetObjectSpawnFrameData* spawnData = (TSNetObjectSpawnFrameData*)( (GXUChar*)data + sizeof ( TSNetFrameHeader ) );

	switch ( spawnData->controllerID )
	{
		case TS_NET_LAND_CRUISER_CONTROLLER_ID:
		{
			TSClientReplicate ();

			TSNetLandCruiserOriginFrameData* originData = (TSNetLandCruiserOriginFrameData*)( (GXUChar*)spawnData + sizeof ( TSNetObjectSpawnFrameData ) );
			TSLandCruiserNetController* landCruiserNetController = new TSLandCruiserNetController ( frameHeader->clientID, frameHeader->objectID );

			landCruiserNetController->SetBodyOrigin ( originData->bodyLocation, originData->bodyRotation );
			for ( GXUInt i = 0; i < 4; i++ )
				landCruiserNetController->SetWheelOrigin ( i, originData->wheelLocations[ i ], originData->wheelRotations[ i ] );

			TSClientAddNetController ( frameHeader->clientID, landCruiserNetController );
		}
		break;

		case TS_NET_CANNON_CONTROLLER_ID:
		{
			TSNetCannonOriginFrameData* originData = (TSNetCannonOriginFrameData*)( (GXUChar*)spawnData + sizeof ( TSNetObjectSpawnFrameData ) );
			TSCannonNetController* cannonNetController = new TSCannonNetController ( frameHeader->clientID, frameHeader->objectID );

			cannonNetController->SetOrigin ( originData->location, originData->rotation );

			TSClientAddNetController ( frameHeader->clientID, cannonNetController );
		}
		break;

		case TS_NET_CANNONBALL_PROJECTILE_CONTROLLER_ID:
		{
			TSNetCannonballProjectileOriginFrameData* originData = (TSNetCannonballProjectileOriginFrameData*)( (GXUChar*)spawnData + sizeof ( TSNetObjectSpawnFrameData ) );
			TSCannonballProjectileNetController* cannonballNetController = new TSCannonballProjectileNetController ( frameHeader->clientID, frameHeader->objectID, originData->location );

			TSClientAddNetController ( frameHeader->clientID, cannonballNetController );
		}
		break;

		case TS_NET_MISSILE_CONTROLLER_ID:
		{
			TSNetMissileOriginFrameData* originData = (TSNetMissileOriginFrameData*)( (GXUChar*)spawnData + sizeof ( TSNetObjectSpawnFrameData ) );
			TSMissileNetController* missileNetController = new TSMissileNetController ( frameHeader->clientID, frameHeader->objectID, originData->location, originData->rotation );

			TSClientAddNetController ( frameHeader->clientID, missileNetController );
		}
		break;

		case TS_NET_STINGER_PROJECTILE_CONTROLLER_ID:
		{
			TSNetStingerProjectileOriginFrameData* originData = (TSNetStingerProjectileOriginFrameData*)( (GXUChar*)spawnData + sizeof ( TSNetObjectSpawnFrameData ) );
			TSStingerProjectileNetController* stingerNetController = new TSStingerProjectileNetController ( frameHeader->clientID, frameHeader->objectID, originData->location, originData->rotation );

			TSClientAddNetController ( frameHeader->clientID, stingerNetController );
		}
		break;

		default:
		break;
	}
}