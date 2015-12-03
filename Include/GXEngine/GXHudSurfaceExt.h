//version 1.13

#ifndef GX_HUD_SURFACE_EXT
#define GX_HUD_SURFACE_EXT


#include "GXMesh.h"
#include "GXTextureStorageExt.h"
#include "GXFontExt.h"
#include <GXCommon/GXMemory.h>


enum eGXImageOverlayType
{
	GX_ALPHA_TRANSPARENCY,
	GX_ALPHA_ADD,
	GX_ALPHA_MULTIPLY,
	GX_SIMPLE_REPLACE
};

struct GXImageInfoExt
{
	GXTexture			texture;

	GXVec4				color;
	GXInt				insertX;
	GXInt				insertY;
	GXInt				insertWidth;
	GXInt				insertHeight;

	eGXImageOverlayType	overlayType;
};

struct GXPenInfoExt
{
	GXFontExt*			font;
	GXVec4				color;
	GXInt 				insertX;
	GXInt				insertY;

	eGXImageOverlayType	overlayType;
};

struct GXLineInfo
{
	GXVec4				color;
	GXFloat				thickness;
	GXVec3				startPoint;
	GXVec3				endPoint;
	
	eGXImageOverlayType	overlayType;
};

class GXHudSurfaceExt : public GXMesh
{
	protected:
		GLuint				fbo;
		GLuint				uvVBO;
		GLuint				lineVAO;
		GLuint				lineVBO;
		GLuint				texObj;

		GLint				img_mod_view_proj_matLocation;
		GLint				img_colorLocation;

		GLint				txt_mod_view_proj_matLocation;
		GLint				txt_colorLocation;

		GLint				line_colorLocation;
		GLint				line_mod_view_proj_matLocation;

		GXFloat				invAspect;

		GXMat4				imageProjMat;

		GXUInt				surfaceWidth;
		GXUInt				surfaceHeight;

		GXVAOInfo			screenQuad;

	public:
		GXHudSurfaceExt ( GXUShort width, GXUShort height, GXBool enableSmooth = GX_TRUE );
		virtual ~GXHudSurfaceExt ();
		
		GXVoid Reset ();

		GXVoid AddImage ( const GXImageInfoExt &imageInfo );
		GXVoid AddLine ( const GXLineInfo &lineInfo );
		GXInt AddText ( const GXPenInfoExt &penInfo, GXUInt bufferNumSymbols, const GXWChar* format, ... );

		GXUInt GetTextLength ( GXFontExt* font, GXUInt bufferNumSymbols, const GXWChar* format, ... );

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();

		GXVoid DrawImageQuad ();
		GXVoid DrawSymbolQuad ();
		GXVoid DrawLine ();

		GXVoid UpdateUV ( const GXVec2 &min, const GXVec2 &max );
		GXVoid UpdateLine ( const GXVec3 &start, const GXVec3 &end );
};

//------------------------------------------------------------------------------------

enum eGXTextAlignment : GXUByte
{
	GX_TEXT_ALIGNMENT_LEFT,
	GX_TEXT_ALIGNMENT_CENTER,
	GX_TEXT_ALIGNMENT_RIGHT
};

struct GXTextPageInfo
{
	GXWChar*	text;
	GXUInt		numLines;
};

class GXTextBox : public GXHudSurfaceExt
{
	private:
		GXDynamicArray			pageInfo;

		GXFontExt*				font;
		GXVec4					fontColor;
		GXInt					fontHeight;

		eGXTextAlignment		alignment;
		GXByte					currentPage;
		GXByte					pageNum;

	public:
		GXTextBox ( GXFontExt* font, const GXVec4 &fontColor, eGXTextAlignment alignment, GXUShort width, GXUShort height );
		virtual ~GXTextBox ();

		GXVoid AddFullText ( const GXWChar* text );
		GXUInt GetPageLines ();
		GXByte GetPageNum ();

		GXVoid NextPage ();
		GXVoid PrevPage ();
		GXVoid SetPage ( GXUByte page );

	private:
		GXVoid SetText ( const GXWChar* text );
		GXWChar* GetTextFittingLine ( const GXWChar* text, GXInt &size );
		GXVoid Update ();
};


#endif //GX_HUD_SURFACE_EXT
