//version 1.1

#include <GXEngine/GXUI.h>
#include <GXCommon/GXCommon.h>


GXMutex* gx_uiMutex = 0;


GXVoid GXCALL GXUIInit ()
{
	if ( gx_uiMutex ) return;

	gx_uiMutex = new GXMutex ();
}

GXVoid GXCALL GXUIDestroy ()
{
	GXSafeDelete ( gx_uiMutex );
}