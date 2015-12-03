//version 1.0

#include "TSConfig.h"
#include <GXCommon/GXFileSystem.h>


GXVoid GXCALL TSLoadConfig ( TSConfig &out )
{
	GXUInt size;
	GXUChar* data;

	GXLoadFile ( L"../Config/Thesis/TSConfig.cfg", (GXVoid**)&data, size, GX_TRUE );

	memcpy ( &out, data, sizeof ( TSConfig ) );

	free ( data );
}

GXVoid GXCALL TSSaveConfig ( const TSConfig &config )
{
	GXWriteToFile ( L"../Config/Thesis/TSConfig.cfg", (GXVoid*)&config, sizeof ( TSConfig ) );
}