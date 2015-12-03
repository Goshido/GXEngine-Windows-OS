//version 1.5

#include <GXCommon/GXFileSystem.h>


GXBool GXCALL GXLoadFile ( const GXWChar* fileName, GXVoid** buffer, GXUInt &size, GXBool notsilent )
{
	FILE* input;
	GXUInt fileSize;
	GXUInt readed;

	_wfopen_s ( &input, fileName, L"rb" );
	if ( input == 0 )
	{
		if ( notsilent )
		{
			GXDebugBox ( L"GXLoadFile::Error - Не могу открыть файл" );
			GXLogW ( L"GXLoadFile::Error - Не могу открыть файл %s\n", fileName );
		}

		*buffer = 0;
		size = 0;

		return GX_FALSE;
	}

	fseek ( input, 0, SEEK_END );
	fileSize = (GXUInt)ftell ( input );
	rewind ( input );

	if ( fileSize == 0 )
	{
		GXDebugBox ( L"GXLoadFile::Error - Файл пуст" );
		GXLogW ( L"GXLoadFile::Error - Файл %s пуст\n", fileName );

		fclose ( input );
		*buffer = 0;
		size = 0;

		return GX_FALSE;
	}

	*buffer = (GXVoid*)malloc ( fileSize );
	readed = (GXUInt)fread ( *buffer, 1, fileSize, input );
	fclose ( input );

	if ( readed != fileSize )
	{
		GXDebugBox ( L"GXLoadFile::Error - Не могу прочитать файл" );
		GXLogW ( L"GXLoadFile::Error - Не могу прочитать файл %s\n", fileName );

		free ( *buffer );
		*buffer = 0;
		size = 0;

		return GX_FALSE;
	}

	size = fileSize;
	return GX_TRUE;
}

GXBool GXCALL GXWriteToFile ( const GXWChar* fileName, const GXVoid* buffer, GXUInt size )
{
	FILE* input;
	_wfopen_s ( &input, fileName, L"wb" );

	if ( !input )
	{
		GXDebugBox ( L"GXWriteToFile::Error - Не могу создать файл" );
		GXLogW ( L"GXWriteToFile::Error - Не могу создать файл %s\n", fileName );

		return GX_FALSE;
	}

	fwrite ( buffer, size, 1, input );
	fclose ( input );

	return GX_TRUE;
}

GXBool GXCALL GXDoesFileExist ( const GXWChar* fileName )
{
	FILE* input;

	_wfopen_s ( &input, fileName, L"rb" );

	if ( input == 0 )
	{
		return GX_FALSE;
	}
	else
	{
		fclose ( input );
		return GX_TRUE;
	}
}

//------------------------------------------------------------------------------------------------

GXWriteStream::GXWriteStream ( const GXWChar* fileName )
{
	_wfopen_s ( &input, fileName, L"wb" );

	if ( !input )
		GXLogW ( L"GXWriteToFile::Error - Can't create file %s", fileName );
}

GXWriteStream::~GXWriteStream ()
{
	Close ();
}

GXVoid GXWriteStream::Write ( const GXVoid* data, GXUInt size )
{
	if ( !input ) return;

	fwrite ( data, size, 1, input );
}

GXVoid GXWriteStream::Close ()
{
	if ( input )
	{
		fclose ( input );
		input = 0;
	}
}
