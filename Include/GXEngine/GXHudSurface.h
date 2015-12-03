//version 1.10

#ifndef GX_HUD_SURFACE
#define GX_HUD_SURFACE


#include "GXMesh.h"
#include "GXFontRenderer.h"


struct GXPenInfo
{
	FT_Face				font;
	GXVec4*				fontColor;
	GXUInt				fontSize;
	GXBool				kerning;

	GXInt				insertX;
	GXInt				insertY;
};

enum eGXImageOverlayType
{
	GX_ALPHA_TRANSPARENCY,
	GX_ALPHA_ADD,
	GX_ALPHA_MULTIPLY,
	GX_SIMPLE_REPLACE
};

struct GXImageInfo
{
	GXGLTextureStruct*	texture;
	GLuint				textureID;

	GXInt				insertX;
	GXInt				insertY;
	GXInt				insertWidth;
	GXInt				insertHeight;

	eGXImageOverlayType	overlayType;
};

class GXHudSurface : public GXMesh
{
	protected:
		GLuint				fbo;
		GLuint				mod_view_proj_matLocation;

		GXGLTextureStruct	texture;

		GXFontInfo			fontInfo;
		GXFontRenderer		fontRenderer;

		GXFloat				invAspect;

		GXMat4				imageProjGXMat;

		GXUInt				surfaceWidth;
		GXUInt				surfaceHeight;

		GXBool				needUpdateFont;
		GXBool				needUpdateImage;

	public:
		GXHudSurface ( GXUShort width, GXUShort height );
		virtual ~GXHudSurface ();
		
		GXVoid Reset ();
		GXVoid AddImage ( const GXImageInfo &imageInfo );
		GXInt AddText ( const GXPenInfo &penInfo, const GXWChar* fmt, ... );

		GXUInt GetTextRasterLength ( const FT_Face font, GXUInt fontSize, GXBool kerning, const GXWChar* fmt, ... );

		virtual GXVoid SetScale ( GXFloat x, GXFloat y, GXFloat z );
		virtual GXVoid SetScale ( GXFloat scaleFactor );

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();

		GXVoid UpdateFont ();
		GXVoid UpdateImage ();
};


#endif //GX_HUD_SURFACE
