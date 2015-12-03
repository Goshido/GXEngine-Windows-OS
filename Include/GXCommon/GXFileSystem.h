//version 1.5

#ifndef GX_FILE_SYSTEM
#define GX_FILE_SYSTEM


#include "GXCommon.h"
#include <iostream>


GXBool GXCALL GXLoadFile ( const GXWChar* fileName, GXVoid** buffer, GXUInt &size, GXBool notsilent );
GXBool GXCALL GXWriteToFile ( const GXWChar* fileName, const GXVoid* buffer, GXUInt size );
GXBool GXCALL GXDoesFileExist ( const GXWChar* fileName );

class GXWriteStream
{
	private:
		FILE* input;

	public:
		GXWriteStream ( const GXWChar* fileName );
		~GXWriteStream ();

		GXVoid Write ( const GXVoid* data, GXUInt size );
		GXVoid Close ();
};


#endif //GX_FILE_SYSTEM
