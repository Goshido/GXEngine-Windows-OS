//version 1.3

#ifndef GX_TGA_LOADER
#define GX_TGA_LOADER


#include <GXCommon/GXTypes.h>


/*
Порядок следования цветов в TGA
G B R A
или
G B R
*/

union GXTGAHeader
{
	struct
	{
		GXUChar		IdLength;
		GXUChar		ColorMap;
		GXUChar		DataType;
		GXUChar		ColorMapInfo[ 5 ];
		GXUShort	X_Origin;
		GXUShort	Y_Origin;
		GXUShort	Width;
		GXUShort	Height;
		GXUChar		BitsPerPixel;
		GXUChar		Description;
	};
	GXUChar v[ 5 * sizeof ( GXUChar ) + 5 * sizeof ( GXUChar ) + 4 * sizeof ( GXUShort ) ];
};


GXBool GXCALL GXLoadTGA ( const GXWChar* file_name, GXUInt &width, GXUInt &height, GXBool &bIsAlpha, GXUChar** data );


#endif //GX_TGA_LOADER