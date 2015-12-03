//version 1.0

#ifndef TS_CONFIG
#define TS_CONFIG


#include <GXCommon/GXTypes.h>


struct TSConfig
{
	GXFloat		musicVolume;
	GXFloat		effectVolume;
};


GXVoid GXCALL TSLoadConfig ( TSConfig &out );
GXVoid GXCALL TSSaveConfig ( const TSConfig &config );


#endif //TS_CONFOG