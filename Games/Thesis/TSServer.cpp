//version 1.1

#include "TSServer.h"
#include "TSNetProtocol.h"
#include <GXEngine/GXNetwork.h>
#include <GXEngine/GXGlobals.h>


GXNetServer*		ts_Server = 0;


class TSServerClientTable
{
	private:
		GXBool			connected[ GX_MAX_NETWORK_CLIENTS ];
		GXBool			haveAddress[ GX_MAX_NETWORK_CLIENTS ];
		sockaddr_in		address[ GX_MAX_NETWORK_CLIENTS ];
		
	public:
		TSServerClientTable ();

		GXBool IsConnected ( GXUInt clientID );
		GXBool IsHaveAddress ( GXUInt clientID );

		const sockaddr_in* GetClientAddress ( GXUInt clientID );

		GXVoid SetClientAddress ( GXUInt clientID, const sockaddr_in &address );
		GXVoid AddClient ( GXUInt clientID );
		GXVoid RemoveClient ( GXUInt clientID );
};

TSServerClientTable::TSServerClientTable ()
{
	for ( GXUInt i = 0; i < GX_MAX_NETWORK_CLIENTS; i++ )
	{
		connected[ i ] = GX_FALSE;
		haveAddress[ i ] = GX_FALSE;
	}
}

GXBool TSServerClientTable::IsConnected ( GXUInt clientID )
{
	return connected[ clientID ];
}

GXBool TSServerClientTable::IsHaveAddress ( GXUInt clientID )
{
	return haveAddress[ clientID ];
}

const sockaddr_in* TSServerClientTable::GetClientAddress ( GXUInt clientID )
{
	if ( IsHaveAddress ( clientID ) )
		return ( address + clientID );

	return 0;
}

GXVoid TSServerClientTable::SetClientAddress ( GXUInt clientID, const sockaddr_in &address )
{
	haveAddress[ clientID ] = GX_TRUE;
	this->address[ clientID ] = address;
}

GXVoid TSServerClientTable::AddClient ( GXUInt clientID )
{
	connected[ clientID ] = GX_TRUE;
	haveAddress[ clientID ] = GX_FALSE;
}

GXVoid TSServerClientTable::RemoveClient ( GXUInt clientID )
{
	connected[ clientID ] = GX_FALSE;
	haveAddress[ clientID ] = GX_FALSE;
}

TSServerClientTable ts_serverClientTable;

//-----------------------------------------------------------------------------------------------------------

GXVoid GXCALL TSServerOnMessageTCP ( GXUInt clientID, GXVoid* data, GXUInt size )
{
	for ( GXUInt i = 0; i < GX_MAX_NETWORK_CLIENTS; i++ )
	{
		if ( !ts_serverClientTable.IsConnected ( i ) || clientID == i ) continue;
		
		ts_Server->SendTCP ( i, data, size );
	}
}

GXVoid GXCALL TSServerOnMessageUDP ( const sockaddr_in &fromClient, GXVoid* data, GXUInt size )
{
	TSNetFrameHeader* frame = (TSNetFrameHeader*)data;

	if ( !ts_serverClientTable.IsHaveAddress ( frame->clientID ) )
		ts_serverClientTable.SetClientAddress ( frame->clientID, fromClient );

	for ( GXUInt i = 0; i < GX_MAX_NETWORK_CLIENTS; i++ )
	{
		if ( frame->clientID == i ) continue;

		const sockaddr_in* address = ts_serverClientTable.GetClientAddress ( i );
		ts_Server->SendUDP ( *address, data, size );
	}
}

GXVoid GXCALL TSServerOnNewTCPConnection ( GXUInt clientID )
{
	if ( !ts_Server ) return;

	ts_serverClientTable.AddClient ( clientID );
	ts_Server->SendTCP ( clientID, &clientID, sizeof ( GXUInt ) );
}

GXVoid GXCALL TSServerOnDisconnect ( GXUInt clientID )
{
	if ( !ts_Server ) return;

	ts_serverClientTable.RemoveClient ( clientID );

	TSNetFrameHeader h;
	h.clientID = clientID;
	h.objectID = 0xFFFFFFFF;
	h.dataType = TS_NET_PLAYER_DISCONNECT;

	ts_Server->BroadcastTCP ( &h, sizeof ( TSNetFrameHeader ) );
}

//--------------------------------------------------------------------------------------------

GXVoid GXCALL TSServerInit ()
{
	ts_Server = gx_Core->GetNetServer ();

	ts_Server->SetOnNewTCPConnection ( &TSServerOnNewTCPConnection );
	ts_Server->SetOnMessageFuncTCP ( &TSServerOnMessageTCP );
	ts_Server->SetOnMessageFuncUDP ( &TSServerOnMessageUDP );
	ts_Server->SetOnDisconnectFunc ( &TSServerOnDisconnect );

	GXBool isOK = GX_TRUE;

	if ( !ts_Server->CreateTCP ( TS_NET_TCP_PORT ) )
	{
		GXLogW ( L"TSServerInit::Warning - Не удалось создать TCP службу\n" );
		isOK = GX_FALSE;
	}

	if ( !ts_Server->CreateUDP ( TS_NET_UDP_PORT ) )
	{
		GXLogW ( L"TSServerInit::Warning - Не удалось создать UDP службу\n" );
		isOK = GX_FALSE;
	}

	if ( isOK )
		GXLogW ( L"TSServerInit::Info - Сервер запущен\n" );
}

GXVoid GXCALL TSServerDestroy ()
{
	if ( !ts_Server ) return;

	GXBool isOK = GX_TRUE;

	if ( !ts_Server->DestroyTCP () )
	{
		GXLogW ( L"TSServerDestroy::Warning - Не удалось остановить TCP службу\n" );
		isOK = GX_FALSE;
	}

	if ( !ts_Server->DestroyUDP () )
	{
		GXLogW ( L"TSServerDestroy::Warning - Не удалось остановить UDP службу\n" );
		isOK = GX_FALSE;
	}

	if ( isOK )
		GXLogW ( L"TSServerDestroy::Info - Сервер остановлен\n" );

	ts_Server = 0;
}