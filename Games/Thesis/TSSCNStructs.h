//version 1.0

#ifndef TS_SCN_STRUCTS
#define TS_SCN_STRUCTS


#include <GXCommon/GXTypes.h>


#define	TS_SCENE_OBJ_TYPE_MESH			0
#define TS_SCENE_OBJ_TYPE_DIR_LIGHT		1
#define TS_SCENE_OBJ_BULP				2
#define TS_SCENE_OBJ_SPOT_LIGHT			3


struct TSSceneObjectHeader
{
	GXUShort	objectType;
	GXUInt		objectName;
};

struct TSSceneMeshHeader
{
	GXBool	castShadows;

	GXUInt	meshFile;
	GXUInt	meshCache;
	GXUInt	materialFile;

	GXFloat	euler_rad[ 3 ];
	GXFloat	scale[ 3 ];
	GXFloat	location[ 3 ];
};

struct TSSceneDirectedLightHeader
{
	GXFloat		color[ 3 ];
	GXFloat		intensity;

	GXFloat		euler_rad[ 3 ];
};

struct TSSceneBulpHeader
{
	GXFloat		color[ 3 ];
	GXFloat		intensity;

	GXFloat		influence;

	GXFloat		location[ 3 ];
};

struct TSSceneSpotLightHeader
{
	GXFloat		color[ 3 ];
	GXFloat		intensity;

	GXFloat		influence;

	GXFloat		cutoff_rad;

	GXFloat		euler_rad[ 3 ];
	GXFloat		location[ 3 ];
};



#endif //TS_SCN_STRUCTS