//version 1.1

#ifndef GX_UI
#define GX_UI


#include <GXCommon/GXMutex.h>


extern GXMutex*	gx_uiMutex;


GXVoid GXCALL GXUIInit ();
GXVoid GXCALL GXUIDestroy ();


#endif //GX_UI