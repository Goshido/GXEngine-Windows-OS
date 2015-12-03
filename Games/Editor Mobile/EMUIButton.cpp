#include "EMUIButton.h"
#include <GXEngine/GXHudSurfaceExt.h>
#include <GXEngine/GXFontStorageExt.h>
#include <GXEngine/GXTextureStorageExt.h>
#include <GXEngine/GXGlobals.h>
#include <GXCommon/GXCommon.h>
#include <GXCommon/GXStrings.h>


class EMUIButtonRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurfaceExt*	surface;
		GXFontExt*			font;
		GXWChar*			caption;
		GXTexture			background;
		GXFloat				layer;
		GXVec4				oldBounds;

	public:
		EMUIButtonRenderer ( GXUIButton* buttonWidget );
		virtual ~EMUIButtonRenderer ();

		virtual GXVoid Update ();
		virtual GXVoid Draw ();

		GXVoid SetCaption ( const GXWChar* caption );
		GXVoid SetLayer ( GXFloat z );

	private:
		GXVoid CheckBounds ();
		GXVoid Refresh ();
};

EMUIButtonRenderer::EMUIButtonRenderer ( GXUIButton* buttonWidget ):
GXWidgetRenderer ( buttonWidget )
{
	surface = new GXHudSurfaceExt ( (GXUShort)( gx_ui_Scale * 4.0f ), (GXUShort)( gx_ui_Scale * 0.5f ), GX_FALSE );
	oldBounds = GXCreateVec4 ( 0.0f, 0.0f, gx_ui_Scale * 4.0f, gx_ui_Scale * 0.5f );
	font = GXGetFontExt ( L"../Fonts/trebuc.ttf", (GXUShort)( gx_ui_Scale * 0.33f ) );
	GXLoadTexture ( L"../Textures/System/Default_Diffuse.tga", background );
	layer = 1.0f;
	GXWcsclone ( &caption, L"������" );
}

EMUIButtonRenderer::~EMUIButtonRenderer ()
{
	GXSafeFree ( caption );
	GXRemoveTextureExt ( background );
	GXRemoveFontExt ( font );
	delete surface;
}

GXVoid EMUIButtonRenderer::Update ()
{
	CheckBounds ();
	Refresh ();
}

GXVoid EMUIButtonRenderer::Draw ()
{
	surface->Draw ();
}

GXVoid EMUIButtonRenderer::SetCaption ( const GXWChar* caption )
{
	GXSafeFree ( this->caption );

	if ( caption )
		GXWcsclone ( &this->caption, caption );
	else
		caption = 0;
}

GXVoid EMUIButtonRenderer::SetLayer ( GXFloat z )
{
	layer = z;
}

GXVoid EMUIButtonRenderer::CheckBounds ()
{
	const GXAABB& bounds = widget->GetBounds ();
	GXBool changed = GX_FALSE;

	if ( fabs ( bounds.max.x - bounds.min.x - oldBounds.z ) > 1.0f )
		changed = GX_TRUE;
	if ( fabs ( bounds.max.y - bounds.min.y - oldBounds.w ) > 1.0f )
		changed = GX_TRUE;

	if ( changed )
	{
		oldBounds.x = bounds.min.x;
		oldBounds.y = bounds.min.y;
		oldBounds.z = bounds.max.x - bounds.min.x;
		oldBounds.w = bounds.max.y - bounds.min.y;

		delete surface;
		surface = new GXHudSurfaceExt ( (GXUShort)oldBounds.z, (GXUShort)oldBounds.w, GX_FALSE );

		surface->SetLocation ( bounds.min.x + oldBounds.z * 0.5f - gx_Core->GetRenderer ()->GetWidth () / 2, bounds.min.y + oldBounds.w * 0.5f - gx_Core->GetRenderer ()->GetHeight () / 2, layer );
		return;
	}

	changed = GX_FALSE;

	if ( fabs ( bounds.min.x - oldBounds.x ) > 1.0f )
		changed = GX_TRUE;
	if ( fabs ( bounds.min.y - oldBounds.y ) > 1.0f )
		changed = GX_TRUE;

	if ( changed )
	{
		oldBounds.x = bounds.min.x;
		oldBounds.y = bounds.min.y;
		oldBounds.z = bounds.max.x - bounds.min.x;
		oldBounds.w = bounds.max.y - bounds.min.y;

		surface->SetLocation ( bounds.min.x + oldBounds.z * 0.5f - gx_Core->GetRenderer ()->GetWidth () / 2, bounds.min.y + oldBounds.w * 0.5f - gx_Core->GetRenderer ()->GetHeight () / 2, layer );
	}
}

GXVoid EMUIButtonRenderer::Refresh ()
{
	GXUIButton* button = (GXUIButton*)widget;
	const GXAABB& bounds = button->GetBounds ();

	GXImageInfoExt ii;
	GXPenInfoExt pi;

	ii.texture = background;

	if ( button->IsDisabled () )
	{
		GXColorToVec4 ( ii.color, 29, 29, 29, 255 );
		GXColorToVec4 ( pi.color, 34, 52, 4, 255 );
	}
	else if ( !button->IsHighlighted () )
	{
		GXColorToVec4 ( ii.color, 49, 48, 48, 255 );
		GXColorToVec4 ( pi.color, 115, 185, 0, 255 );
	}
	else if ( button->IsPressed () )
	{
		GXColorToVec4 ( ii.color, 83, 116, 20, 255 );
		GXColorToVec4 ( pi.color, 142, 255, 5, 255 );
	}
	else
	{
		GXColorToVec4 ( ii.color, 46, 64, 11, 255 );
		GXColorToVec4 ( pi.color, 115, 185, 0, 255 );
	}

	ii.insertX = ii.insertY = 0;
	ii.insertWidth = (GXInt)( bounds.max.x - bounds.min.x );
	ii.insertHeight = (GXInt)( bounds.max.y - bounds.min.y );
	ii.overlayType = GX_SIMPLE_REPLACE;

	surface->AddImage ( ii );

	if ( !caption ) return;

	GXInt len = (GXInt)surface->GetTextLength ( font, 0, caption );
	pi.font = font;
	pi.insertX = (GXInt)( ( ii.insertWidth - len ) / 2 );
	pi.insertY = (GXInt)( ( ii.insertHeight - font->GetSize () * 0.8f ) * 0.5f );
	pi.overlayType = GX_ALPHA_TRANSPARENCY;

	surface->AddText ( pi, 0, caption );

	GXLineInfo li;
	GXColorToVec4 ( li.color, 128, 128, 128, 255 );
	li.thickness = 1.0f;
	li.startPoint = GXCreateVec3 ( 1.0f, 1.0f, 0.0f );
	li.endPoint = GXCreateVec3 ( (GXFloat)ii.insertWidth - 1.0f, 1.0f, 0.0f );
	li.overlayType = GX_SIMPLE_REPLACE;
	
	surface->AddLine ( li );
	
	li.startPoint = GXCreateVec3 ( (GXFloat)ii.insertWidth, 1.0f, 0.0f );
	li.endPoint = GXCreateVec3 ( (GXFloat)ii.insertWidth, (GXFloat)ii.insertHeight - 1.0f, 0.0f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( (GXFloat)ii.insertWidth - 1.0f, (GXFloat)ii.insertHeight, 0.0f );
	li.endPoint = GXCreateVec3 ( 1.0f, (GXFloat)ii.insertHeight, 0.0f );

	surface->AddLine ( li );

	li.startPoint = GXCreateVec3 ( 1.0f, (GXFloat)ii.insertHeight - 1.0f, 0.0f );
	li.endPoint = GXCreateVec3 ( 1.0f, 1.0f, 0.0f );

	surface->AddLine ( li );
}

//----------------------------------------------------------------------------------

EMUIButton::EMUIButton ()
{
	widget = new GXUIButton ();
	widget->SetRenderer ( new EMUIButtonRenderer ( widget ) );
}

EMUIButton::~EMUIButton ()
{
	delete widget->GetRenderer ();
	delete widget;
}

GXVoid EMUIButton::Enable ()
{
	widget->Enable ();
}

GXVoid EMUIButton::Disable ()
{
	widget->Disable ();
}

GXVoid EMUIButton::Resize ( GXFloat x, GXFloat y, GXFloat width, GXFloat height )
{
	widget->Resize ( x, y, width, height );
}

GXVoid EMUIButton::SetCaption ( const GXWChar* caption )
{
	EMUIButtonRenderer* renderer = (EMUIButtonRenderer*)widget->GetRenderer ();
	renderer->SetCaption ( caption );
	widget->Redraw ();
}

GXVoid EMUIButton::SetLayer ( GXFloat z )
{
	EMUIButtonRenderer* renderer = (EMUIButtonRenderer*)widget->GetRenderer ();
	renderer->SetLayer ( z );
	widget->Redraw ();
}

GXVoid EMUIButton::Show ()
{
	widget->Show ();
}

GXVoid EMUIButton::Hide ()
{
	widget->Hide ();
}

GXVoid EMUIButton::SetOnLeftMouseButtonCallback ( PFNGXONMOUSEBUTTONPROC callback )
{
	widget->SetOnLeftMouseButtonCallback ( callback );
}
