//version 1.3

#ifndef GX_LOGGER
#define GX_LOGGER


#include "GXTypes.h"
#include <locale.h>
#include <iostream>


#define GXDebugBox(x) MessageBoxW ( 0, x, L"Дебаг-окно", MB_ICONEXCLAMATION )


GXVoid GXCALL GXLogInit ();
GXVoid GXCALL GXLogDestroy ();

GXVoid GXCALL GXLogA ( const GXMBChar* format, ... );
GXVoid GXCALL GXLogW ( const GXWChar* format, ... );

//GXVoid GXCALL GXSeeError ( LPTSTR lpszFunction, GXBool isDebugBox );


#endif //GX_LOGGER