#include "EMUIMenu.h"
#include <GXEngine/GXGlobals.h>
#include <GXEngine/GXHudSurfaceExt.h>
#include <GXEngine/GXFontStorageExt.h>
#include <GXEngine/GXTextureStorageExt.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXStrings.h>


class EMUIMenuRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurfaceExt*	surface;
		GXTexture			background;
		GXFontExt*			font;
		GXDynamicArray		captions;
		GXVec4				oldBounds;
		GXFloat				layer;

	public:
		EMUIMenuRenderer ( GXUIMenu* widget );
		virtual ~EMUIMenuRenderer ();

		virtual GXVoid Update ();
		virtual GXVoid Draw ();

		GXVoid AddItem ( const GXWChar* caption );
		GXVoid RenameItem ( GXUByte item, const GXWChar* caption );

		GXFloat EstimateItemWidth ( const GXWChar* caption );
		GXUByte GetTotalItems ();

		GXVoid SetLayer ( GXFloat z );

	private:
		GXVoid CheckBounds ();
		GXVoid Refresh ();
};

EMUIMenuRenderer::EMUIMenuRenderer ( GXUIMenu* widget ):
GXWidgetRenderer ( widget ), captions ( sizeof ( GXWChar* ) )
{
	surface = new GXHudSurfaceExt ( (GXUShort)( gx_ui_Scale * 4.0f ), (GXUShort)( gx_ui_Scale * 0.5f ), GX_FALSE );
	font = GXGetFontExt ( L"../Fonts/trebuc.ttf", (GXUShort)( gx_ui_Scale * 0.33f ) );
	GXLoadTexture ( L"../Textures/System/Default_Diffuse.tga", background );
	oldBounds = GXCreateVec4 ( 0.0f, 0.0f, gx_ui_Scale * 4.0f, gx_ui_Scale * 0.5f );
	layer = 1.0f;
}

EMUIMenuRenderer::~EMUIMenuRenderer ()
{
	GXRemoveTextureExt ( background );
	GXRemoveFontExt ( font );
	delete surface;

	GXUByte len = (GXUByte)captions.GetLength ();
	for ( GXUByte i = 0; i < len; i++ )
	{
		GXWChar* caption = *( (GXWChar**)captions.GetValue ( i ) );
		GXSafeFree ( caption );
	}
}

GXVoid EMUIMenuRenderer::Update ()
{
	CheckBounds ();
	Refresh ();
}

GXVoid EMUIMenuRenderer::Draw ()
{
	surface->Draw ();
}

GXVoid EMUIMenuRenderer::AddItem ( const GXWChar* caption )
{
	GXWChar* str;
	GXWcsclone ( &str, caption );

	GXUByte total = (GXUByte)captions.GetLength ();
	captions.SetValue ( total, &str );
}

GXVoid EMUIMenuRenderer::RenameItem ( GXUByte item, const GXWChar* caption )
{
	GXWChar* str = *( (GXWChar**)captions.GetValue ( item ) );
	GXSafeFree ( str );
	GXWcsclone ( &str, caption );
	captions.SetValue ( item, &str );
}

GXFloat EMUIMenuRenderer::EstimateItemWidth ( const GXWChar* caption )
{
	GXUInt len = surface->GetTextLength ( font, 0, caption );
	return len + 0.5f * gx_ui_Scale;
}

GXUByte EMUIMenuRenderer::GetTotalItems ()
{
	return (GXUByte)captions.GetLength ();
}

GXVoid EMUIMenuRenderer::SetLayer ( GXFloat z )
{
	layer = z;
}

GXVoid EMUIMenuRenderer::CheckBounds ()
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

GXVoid EMUIMenuRenderer::Refresh ()
{
	GXImageInfoExt ii;
	GXPenInfoExt pi;

	ii.texture = background;
	GXColorToVec4 ( ii.color, 49, 48, 48, 255 );
	ii.insertX = ii.insertY = 0;
	ii.insertWidth = (GXInt)oldBounds.z;
	ii.insertHeight = (GXInt)oldBounds.w;
	ii.overlayType = GX_SIMPLE_REPLACE;

	surface->AddImage ( ii );

	pi.font = font;
	GXColorToVec4 ( pi.color, 115, 185, 0 , 255 );
	pi.overlayType = GX_ALPHA_TRANSPARENCY;

	GXUIMenu* menu = (GXUIMenu*)widget;

	GXUByte total = menu->GetTotalItems ();
	for ( GXUByte i = 0; i < total; i++ )
	{
		const GXAABB& itemBounds = menu->GetItemBounds ( i );
		ii.insertX = (GXInt)itemBounds.min.x;
		ii.insertY = 0;
		ii.insertWidth = (GXInt)( itemBounds.max.x - itemBounds.min.x );
		ii.insertHeight = (GXInt)( itemBounds.max.y - itemBounds.min.y );

		if ( menu->IsItemHighlighted ( i ) )
			GXColorToVec4 ( ii.color, 46, 64, 11, 255 );
		else
			GXColorToVec4 ( ii.color, 49, 48, 48, 255 );

		surface->AddImage ( ii );

		GXWChar* caption = *( (GXWChar**)captions.GetValue ( i ) );

		pi.insertX = (GXInt)( ii.insertX + 0.25f * gx_ui_Scale );
		pi.insertY = (GXInt)( ( ii.insertHeight - font->GetSize () * 0.8f ) * 0.5f );
		
		surface->AddText ( pi, 0, caption );
	}
}

//------------------------------------------------------------------------------------------

struct EMUISubmenuItem
{
	GXWChar*	caption;
	GXWChar*	hotkey;
};

class EMUISubmenuRenderer : public GXWidgetRenderer
{
	private:
		GXHudSurfaceExt*	surface;
		GXTexture			background;
		GXFontExt*			font;
		GXDynamicArray		items;
		GXVec4				oldBounds;
		GXFloat				layer;

	public:
		EMUISubmenuRenderer ( GXUISubmenu* widget );
		virtual ~EMUISubmenuRenderer ();

		virtual GXVoid Update ();
		virtual GXVoid Draw ();

		GXVoid AddItem ( const GXWChar* caption, const GXWChar* hotkey );
		GXVoid RenameItem ( GXUByte item, const GXWChar* caption, const GXWChar* hotkey );

		GXVoid SetLayer ( GXFloat z );

	private:
		GXVoid CheckBounds ();
		GXVoid Refresh ();
};

EMUISubmenuRenderer::EMUISubmenuRenderer ( GXUISubmenu* widget ):
GXWidgetRenderer ( widget ), items ( sizeof ( EMUISubmenuItem ) )
{
	surface = new GXHudSurfaceExt ( (GXUShort)( gx_ui_Scale * 8.0f ), (GXUShort)( gx_ui_Scale * 2.0f ), GX_FALSE );
	font = GXGetFontExt ( L"../Fonts/trebuc.ttf", (GXUShort)( gx_ui_Scale * 0.33f ) );
	GXLoadTexture ( L"../Textures/System/Default_Diffuse.tga", background );
	oldBounds = GXCreateVec4 ( 0.0f, 0.0f, gx_ui_Scale * 8.0f, gx_ui_Scale * 2.0f );
	layer = 1.0f;
}

EMUISubmenuRenderer::~EMUISubmenuRenderer ()
{
	GXRemoveTextureExt ( background );
	GXRemoveFontExt ( font );
	delete surface;

	GXUByte len = (GXUByte)items.GetLength ();
	for ( GXUByte i = 0; i < len; i++ )
	{
		EMUISubmenuItem* item = (EMUISubmenuItem*)items.GetValue ( i );
		GXSafeFree ( item->caption );
		GXSafeFree ( item->hotkey );
	}
}

GXVoid EMUISubmenuRenderer::Update ()
{
	CheckBounds ();
	Refresh ();
}

GXVoid EMUISubmenuRenderer::Draw ()
{
	surface->Draw ();
}

GXVoid EMUISubmenuRenderer::AddItem ( const GXWChar* caption, const GXWChar* hotkey )
{
	EMUISubmenuItem smi;
	GXWcsclone ( &smi.caption, caption );
	if ( hotkey )
		GXWcsclone ( &smi.hotkey, hotkey );
	else
		smi.hotkey = 0;

	items.SetValue ( items.GetLength (), &smi ); 
}

GXVoid EMUISubmenuRenderer::RenameItem ( GXUByte item, const GXWChar* caption, const GXWChar* hotkey )
{
	EMUISubmenuItem* p = (EMUISubmenuItem*)items.GetValue ( item );
	GXSafeFree ( p->caption );
	GXSafeFree ( p->hotkey );
	
	GXWcsclone ( &p->caption, caption );
	if ( hotkey )
		GXWcsclone ( &p->hotkey, hotkey );
	else
		p->hotkey = 0;
}

GXVoid EMUISubmenuRenderer::SetLayer ( GXFloat z )
{
	layer = z;
}

GXVoid EMUISubmenuRenderer::CheckBounds ()
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

GXVoid EMUISubmenuRenderer::Refresh ()
{
	const GXAABB& bounds = widget->GetBounds ();

	GXImageInfoExt ii;
	ii.texture = background;
	GXColorToVec4 ( ii.color, 49, 48, 48, 255 );
	ii.insertX = ii.insertY = 0;
	ii.insertWidth = (GXInt)( bounds.max.x - bounds.min.x );
	ii.insertHeight = (GXInt)( bounds.max.y - bounds.min.y );
	ii.overlayType = GX_SIMPLE_REPLACE;
	surface->AddImage ( ii );

	GXUISubmenu* submenu = (GXUISubmenu*)widget;
	GXUByte total = submenu->GetTotalItems ();
	GXFloat itemHeight = submenu->GetItemHeight ();

	GXPenInfoExt pi;
	pi.font = font;
	pi.overlayType = GX_ALPHA_TRANSPARENCY;

	for ( GXUByte i = 0; i < total; i++ )
	{
		if ( submenu->IsItemPressed ( i ) )
		{
			GXColorToVec4 ( ii.color, 83, 116, 20, 255 );
			GXColorToVec4 ( pi.color, 142, 255, 5, 255 );
			ii.insertY = (GXInt)( ( total - i - 1 ) * itemHeight );
			ii.insertHeight = (GXInt)itemHeight;
			surface->AddImage ( ii );
		}
		else if ( submenu->IsItemHighlighted ( i ) )
		{
			GXColorToVec4 ( ii.color, 46, 64, 11, 255 );
			GXColorToVec4 ( pi.color, 115, 185, 0, 255 );
			ii.insertY = (GXInt)( ( total - i - 1 ) * itemHeight );
			ii.insertHeight = (GXInt)itemHeight;
			surface->AddImage ( ii );
		}
		else
			GXColorToVec4 ( pi.color, 115, 185, 0, 255 );

		const EMUISubmenuItem* item = (const EMUISubmenuItem*)items.GetValue ( i );

		pi.insertX = (GXInt)( 0.15f * gx_ui_Scale );
		pi.insertY = (GXInt)( 0.15f * gx_ui_Scale + ( total - i - 1 ) * itemHeight );
		
		surface->AddText ( pi, 0, item->caption );

		if ( !item->hotkey ) continue;

		GXInt len = (GXInt)surface->GetTextLength ( font, 0, item->hotkey );
		pi.insertX = (GXInt)( ii.insertWidth - len - 0.15f * gx_ui_Scale );
		surface->AddText ( pi, 0, item->hotkey );
	}

	GXLineInfo li;
	GXColorToVec4 ( li.color, 128, 128, 128, 255 );
	li.thickness = 1.0f;
	li.startPoint = GXCreateVec3 ( 1.0f, 1.0f, 0.0f );
	li.endPoint = GXCreateVec3 ( (GXFloat)ii.insertWidth - 1.0f, 1.0f, 0.0f );
	li.overlayType = GX_SIMPLE_REPLACE;

	ii.insertHeight = (GXInt)( bounds.max.y - bounds.min.y );
	
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

//------------------------------------------------------------------------------------------

EMUIMenu::EMUIMenu ():
submenuWidgets ( sizeof ( GXUISubmenu* ) )
{
	menuWidget = new GXUIMenu ( this, &OnShowSubmenu );
	menuWidget->SetRenderer ( new EMUIMenuRenderer ( menuWidget ) );
	menuWidget->Resize ( 0.0f, 0.0f, gx_ui_Scale * 4.0f, gx_ui_Scale * 0.5f );
}

EMUIMenu::~EMUIMenu ()
{
	delete menuWidget->GetRenderer ();
	delete menuWidget;

	GXUInt total = submenuWidgets.GetLength ();
	for ( GXUInt i = 0; i < total; i++ )
	{
		GXUISubmenu* submenu = *( (GXUISubmenu**)submenuWidgets.GetValue ( i ) );
		delete submenu->GetRenderer ();
		delete submenu;
	}
}

GXVoid EMUIMenu::AddItem ( const GXWChar* caption )
{
	EMUIMenuRenderer* renderer = (EMUIMenuRenderer*)menuWidget->GetRenderer ();

	GXUByte total = renderer->GetTotalItems ();
	renderer->AddItem ( caption );

	menuWidget->AddItem ();
	GXFloat r = renderer->EstimateItemWidth ( caption );
	menuWidget->ResizeItem ( total, r );

	GXUISubmenu* submenu = new GXUISubmenu ();
	submenu->SetRenderer ( new EMUISubmenuRenderer ( submenu ) );
	submenu->Hide ();
	submenu->Resize ( 0.0f, 0.0f, 8.0f * gx_ui_Scale, 0.5f * gx_ui_Scale );

	submenuWidgets.SetValue ( total, &submenu );
}

GXVoid EMUIMenu::RenameItem ( GXUByte item, const GXWChar* caption )
{
	EMUIMenuRenderer* renderer = (EMUIMenuRenderer*)menuWidget->GetRenderer ();
	menuWidget->ResizeItem ( item, renderer->EstimateItemWidth ( caption ) );
	renderer->RenameItem ( item, caption );
}

GXVoid EMUIMenu::SetLocation ( GXFloat x, GXFloat y )
{
	const GXAABB& menuBounds = menuWidget->GetBounds ();
	menuWidget->Resize ( x, y, gx_ui_Scale * 4.0f, gx_ui_Scale * 0.5f );
}

GXVoid EMUIMenu::AddSubitem ( GXUByte item, const GXWChar* caption, const GXWChar* hotkey, PFNGXONMOUSEBUTTONPROC onMouseButton )
{
	GXUISubmenu* submenu = *( (GXUISubmenu**)submenuWidgets.GetValue ( item ) );
	EMUISubmenuRenderer* renderer = (EMUISubmenuRenderer*)submenu->GetRenderer ();
	submenu->AddItem ( onMouseButton );
	renderer->AddItem ( caption, hotkey );
}

GXVoid EMUIMenu::RenameSubitem ( GXUByte item, GXUByte subitem, const GXWChar* caption, const GXWChar* hotkey )
{
	GXUISubmenu* submenu = *( (GXUISubmenu**)submenuWidgets.GetValue ( item ) );
	EMUISubmenuRenderer* renderer = (EMUISubmenuRenderer*)submenu->GetRenderer ();
	renderer->RenameItem ( item, caption, hotkey );
	submenu->Redraw ();
}

GXVoid EMUIMenu::SetLayer ( GXFloat z )
{
	EMUIMenuRenderer* renderer = (EMUIMenuRenderer*)menuWidget->GetRenderer ();
	renderer->SetLayer ( z );

	GXUInt total = submenuWidgets.GetLength ();
	for ( GXUInt i = 0; i < total; i++ )
	{
		GXUISubmenu* submenu = *( (GXUISubmenu**)submenuWidgets.GetValue ( i ) );
		EMUISubmenuRenderer* submenuRenderer = (EMUISubmenuRenderer*)submenu->GetRenderer ();
		submenuRenderer->SetLayer ( z );
	}
}

GXVoid GXCALL EMUIMenu::OnShowSubmenu ( GXVoid* menuHandler, GXUByte item )
{
	EMUIMenu* menu = (EMUIMenu*)menuHandler;

	GXUByte totalSubmenus = (GXUByte)menu->submenuWidgets.GetLength ();
	for ( GXUByte i = 0; i < totalSubmenus; i++ )
	{
		GXUISubmenu* submenu = *( (GXUISubmenu**)menu->submenuWidgets.GetValue ( i ) );
		if ( i != item && submenu->IsVisible () )
			submenu->Hide ();
	}

	GXUISubmenu* activeSubmenu = *( (GXUISubmenu**)menu->submenuWidgets.GetValue ( item ) );
	const GXAABB& itemBounds = menu->menuWidget->GetItemBounds ( item );

	activeSubmenu->Resize ( itemBounds.min.x, itemBounds.min.y - ( activeSubmenu->GetTotalItems () * activeSubmenu->GetItemHeight () ), gx_ui_Scale, gx_ui_Scale );
	activeSubmenu->Show ();
}
