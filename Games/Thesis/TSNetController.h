//version 1.1

#ifndef TS_NET_CONTROLLER
#define TS_NET_CONTROLLER


#include "TSNetProtocol.h"


class TSNetController
{
	protected:
		GXUInt		clientID;
		GXUInt		objectID;

	public:
		TSNetController ( GXUInt clientID, GXUInt objectID );
		virtual ~TSNetController ();

		virtual GXVoid OnReceive ( const GXVoid* data, GXUInt size ) = 0;

		GXUInt GetClientID ();
		GXUInt GetObjectID ();
};


#endif //TS_NET_CONTROLLER