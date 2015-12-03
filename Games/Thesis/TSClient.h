//version 1.2

#ifndef TS_CLIENT
#define TS_CLIENT


#include <GXCommon/GXTypes.h>


extern GXUInt	ts_ClientID;


GXVoid GXCALL TSClientInit ( GXBool isHost );

GXVoid GXCALL TSClientSendTCP ( GXUInt objectID, GXUInt dataType, const GXVoid* data, GXUInt size );
GXVoid GXCALL TSClientSendUDP ( GXUInt objectID, GXUInt dataType, const GXVoid* data, GXUInt size );

GXVoid GXCALL TSClientDestroy ();

GXVoid GXCALL TSClientDeleteNetController ( GXUInt clientID, GXUInt objectID );


#endif //TS_CLIENT