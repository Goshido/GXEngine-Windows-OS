//version 1.0

#ifndef TS_NET_PROTOCOL
#define TS_NET_PROTOCOL


#include <GXCommon/GXMath.h>

#define TS_NET_TCP_PORT		0x785F
#define TS_NET_UDP_PORT		0x785F

#define TS_NET_INVALID_ID	0xFFFFFFFF


struct TSNetFrameHeader
{
	GXUInt		clientID;
	GXUInt		objectID;
	GXUInt		dataType;
};


//----------------------------------------------------------------------------------------
/*Типы данных*/
/*Поле dataType в заголовке фрейма*/

#define TS_NET_OBJECT_SPAWN								0
struct TSNetObjectSpawnFrameData
{
	GXUInt	controllerID;
};

#define	TS_NET_OBJECT_DESTROY							1

#define	TS_NET_OBJECT_LAND_CRUISER_ORIGIN				2
struct TSNetLandCruiserOriginFrameData
{
	GXVec3		bodyLocation;
	GXQuat		bodyRotation;

	GXVec3		wheelLocations[ 4 ];
	GXQuat		wheelRotations[ 4 ];
};

#define TS_NET_OBJECT_CANNON_ORIGIN						3
struct TSNetCannonOriginFrameData
{
	GXVec3		location;
	GXQuat		rotation;
};

#define TS_NET_OBJECT_CANNON_FIRE						4

#define	TS_NET_OBJECT_CANNONBALL_PROJECTILE_ORIGIN		5
struct TSNetCannonballProjectileOriginFrameData
{
	GXVec3		location;
};

#define	TS_NET_OBJECT_CANNONBALL_PROJECTILE_EXPLOSED	6

#define TS_NET_OBJECT_MISSILE_ORIGIN					7
struct TSNetMissileOriginFrameData
{
	GXVec3		location;
	GXQuat		rotation;
};

#define	TS_NET_OBJECT_STINGER_PROJECTILE_ORIGIN			8
struct TSNetStingerProjectileOriginFrameData
{
	GXVec3		location;
	GXQuat		rotation;
};

#define	TS_NET_OBJECT_STINGER_PROJECTILE_EXPLOSED		9

#define TS_NET_PLAYER_DISCONNECT						10


//-----------------------------------------------------------------------------------------
//Controller IDs

#define TS_NET_LAND_CRUISER_CONTROLLER_ID				0
#define TS_NET_CANNON_CONTROLLER_ID						1
#define TS_NET_CANNONBALL_PROJECTILE_CONTROLLER_ID		2
#define TS_NET_MISSILE_CONTROLLER_ID					3
#define TS_NET_STINGER_PROJECTILE_CONTROLLER_ID			4


#endif //TS_NET_PROTOCOL