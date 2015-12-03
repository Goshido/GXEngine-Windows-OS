//version 1.3

#include <GXEngine/GXFontStorage.h>
#include <GXCommon/GXStrings.h>
#include <GXCommon/GXFileSystem.h>


extern FT_Library		gx_ft_Library;

struct GXFontUnit
{
	GXFontUnit*		prev;
	GXFontUnit*		next;
	GXUInt			numRefs;

	GXWChar*		fileName;
	GXVoid*			mappedFile;
	GXUInt			totalSize;

	FT_Face			face;
	GXBool			valid;

	GXFontUnit ( const GXWChar* fileName );
	~GXFontUnit ();
};

GXFontUnit*		gx_strg_Fonts = 0;

GXFontUnit::GXFontUnit ( const GXWChar* fileName )
{
	if ( fileName )
	{
		GXUInt size = sizeof ( GXWChar ) * ( GXWcslen ( fileName ) + 1 );
		this->fileName = (GXWChar*)malloc ( size );
		memcpy ( this->fileName, fileName, size );

		if ( !GXLoadFile ( fileName, &mappedFile, totalSize, GX_TRUE ) )
		{
			GXLogW ( L"GXFontUnit::GXFontUnit::Error - Не удалось загрузить файл шрифта %s\n", fileName );
			valid = GX_FALSE;
		}
		else
		{
			if ( GXFtNewMemoryFace ( gx_ft_Library, (FT_Byte*)mappedFile, totalSize, 0, &face ) )
			{
				GXLogW ( L"GXFontUnit::GXFontUnit::Error - Функция FT_New_Memory_Face для файла %s провалилась\n", this->fileName );
				valid = GX_FALSE;
			}
			else
				valid = GX_TRUE;
		}
	}
	else
	{
		this->fileName = 0;
		mappedFile = 0;
		valid = GX_FALSE;
	}

	numRefs = 1;
	next = gx_strg_Fonts;
	prev = 0;
	gx_strg_Fonts = this;
	
}

GXFontUnit::~GXFontUnit ()
{
	if ( valid && GXFtDoneFace ( face ) )
		GXLogW ( L"GXFontUnit::~GXFontUnit::Error - Уничтожение FT_Face для шрифта %s провалено", fileName );

	GXSafeFree ( fileName );
	GXSafeFree ( mappedFile );

	if ( next ) next->prev = prev;
	if ( this == gx_strg_Fonts ) gx_strg_Fonts = next;
}

//---------------------------------------------------------------------------------------------


GXFontUnit* GXCALL GXFindFontByName ( const GXWChar* fileName )
{
	for ( GXFontUnit* p = gx_strg_Fonts; p; p = p->next )
		if ( GXWcscmp ( fileName, p->fileName ) == 0 )
			return p;

	return 0;
}

GXFontUnit* GXCALL GXFindFontById ( FT_Face font )
{
	for ( GXFontUnit* p = gx_strg_Fonts; p; p = p->next )
		if ( font == p->face )
			return p;

	return 0;
}

FT_Face GXCALL GXGetFont ( const GXWChar* fileName )
{
	GXFontUnit* p = GXFindFontByName ( fileName );

	if ( p )
	{
		p->numRefs++;
		return p->face;
	}

	GXFontUnit* font = new GXFontUnit ( fileName );

	return font->face;
}

GXVoid GXCALL GXRemoveFont ( FT_Face font )
{
	GXFontUnit* p = GXFindFontById ( font );

	if ( p )
	{
		p->numRefs--;
		if ( p->numRefs == 0 )
			delete p;
	}
}

GXVoid GXCALL GXCleanFonts ()
{
	GXFontUnit* p = gx_strg_Fonts;

	while ( p )
	{
		GXFontUnit* next = p->next;
		delete p;
		p = next;
	}

	gx_strg_Fonts = 0;
}

GXUInt GXCALL GXGetTotalFontStorageObjects ( GXWChar** lastObject )
{
	GXUInt objects = 0;
	*lastObject = 0; 

	for ( GXFontUnit* p = gx_strg_Fonts; p; p = p->next )
	{
		*lastObject = p->fileName;
		objects++;
	}

	return objects;
}