//version 1.0

#include <GXEngine/GXUIExt.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXMemory.h>
#include <GXCommon/GXCommon.h>


#define GX_MSG_LMBDOWN				0
#define GX_MSG_LMBUP				1
#define GX_MSG_MMBDOWN				2
#define GX_MSG_MMBUP				3
#define GX_MSG_RMBDOWN				4
#define GX_MSG_RMBUP				5
#define GX_MSG_SCROLL				6
#define GX_MSG_MOUSE_MOVE			7
#define GX_MSG_MOUSE_OVER			8
#define GX_MSG_MOUSE_LEAVE			9
#define GX_MSG_ENABLE				10
#define GX_MSG_DISABLE				11
#define GX_MSG_RESIZE				12
#define GX_MSG_REDRAW				13
#define GX_MSG_MENU_ADD_ITEM		14
#define GX_MSG_MENU_RESIZE_ITEM		15
#define GX_MSG_SUBMENU_ADD_ITEM		16
#define GX_MSG_SUBMENU_SET_HEIGHT	17
#define GX_MSG_HIDE					18
#define GX_MSG_SHOW					19


GXFloat				gx_ui_Scale = 1.0f;

GXTouchSurface*		gx_ui_TouchSurface = 0;
GXWidget*			gx_ui_Widgets = 0;
GXMutex*			gx_ui_Mutex = 0;
GXCircleBuffer*		gx_ui_MessageBuffer = 0;

GXWidgetRenderer::GXWidgetRenderer ( GXWidget* widget )
{
	this->widget = widget;
}

GXWidgetRenderer::~GXWidgetRenderer ()
{
	//NOTHING
}

GXVoid GXWidgetRenderer::OnUpdate ()
{
	//NOTHING
}

GXVoid GXWidgetRenderer::OnDraw ()
{
	//NOTHING
}

//---------------------------------------------------------------------------------

GXWidget::GXWidget ()
{
	GXSetAABBEmpty ( bounds );
	GXAddVertexToAABB ( bounds, -1.0f, -1.0f, -1.0f );
	GXAddVertexToAABB ( bounds, 1.0f, 1.0f, 1.0f );

	gx_ui_Mutex->Lock ();

	next = gx_ui_Widgets;
	gx_ui_Widgets = this;
	renderer = 0;

	gx_ui_Mutex->Release ();
}

GXWidget::~GXWidget ()
{
	gx_ui_Mutex->Lock ();

	if ( gx_ui_Widgets == this && next == 0 )
	{
		gx_ui_Widgets = 0;
		gx_ui_Mutex->Release ();
		return;
	}

	if ( gx_ui_Widgets == this )
	{
		gx_ui_Widgets = next;
		gx_ui_Mutex->Release ();
		return;
	}

	GXWidget* prev = gx_ui_Widgets;
	while ( prev->next != this )
		prev = prev->next;

	prev->next = next;
	gx_ui_Mutex->Release ();

	isVisible = GX_TRUE;
}

GXVoid GXWidget::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_RESIZE:
		{
				memcpy ( &bounds, data, sizeof ( GXAABB ) );
				if ( renderer )
					renderer->OnUpdate ();
		}
		break;

		case GX_MSG_SHOW:
			isVisible = GX_TRUE;
		break;

		case GX_MSG_HIDE:
			isVisible = GX_FALSE;
		break;
	}
}

GXVoid GXWidget::OnDraw ()
{
	if ( renderer )
		renderer->OnDraw ();
}

GXVoid GXWidget::Resize ( GXFloat x, GXFloat y, GXFloat width, GXFloat height )
{
	GXAABB newBounds;
	GXSetAABBEmpty ( newBounds );
	GXAddVertexToAABB ( newBounds, x, y, -10.0f );
	GXAddVertexToAABB ( newBounds, x + width, y + height, 10.0f );

	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_RESIZE, &newBounds, sizeof ( GXAABB ) );
}

const GXAABB& GXWidget::GetBounds ()
{
	return bounds;
}

GXVoid GXWidget::Show ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_SHOW, 0, 0 );
}

GXVoid GXWidget::Hide ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_HIDE, 0, 0 );
}

GXBool GXWidget::IsVisible ()
{
	return isVisible;
}

GXVoid GXWidget::SetRenderer ( GXWidgetRenderer* renderer )
{
	this->renderer = renderer;
}

GXWidgetRenderer* GXWidget::GetRenderer ()
{
	return renderer;
}

//--------------------------------------------------------------------------

GXTouchSurface::GXTouchSurface ()
{
	messages = lastMessage = 0;

	gx_ui_Widgets = 0;
	gx_ui_Mutex = new GXMutex ();
	gx_ui_MessageBuffer = new GXCircleBuffer ( 1024 * 1024 ); //1 Mb
	gx_ui_TouchSurface = this;
	mouseOverWidget = 0;
}

GXTouchSurface::~GXTouchSurface ()
{
	delete gx_ui_MessageBuffer;
	DeleteWidgets ();
	delete gx_ui_Mutex;
	gx_ui_TouchSurface = 0;
}

GXVoid GXTouchSurface::OnLeftMouseButtonDown ( const GXVec2 &position )
{
	GXWidget* target = FindWidget ( position.x, position.y );
	if ( !target ) return;

	SendMessage ( target, GX_MSG_LMBDOWN, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnLeftMouseButtonUp ( const GXVec2 &position )
{
	GXWidget* target = FindWidget ( position.x, position.y );
	if ( !target ) return;

	SendMessage ( target, GX_MSG_LMBUP, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnMiddleMouseButtonDown ( const GXVec2 &position )
{
	GXWidget* target = FindWidget ( position.x, position.y );
	if ( !target ) return;

	SendMessage ( target, GX_MSG_MMBDOWN, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnMiddleMouseButtonUp ( const GXVec2 &position )
{
	GXWidget* target = FindWidget ( position.x, position.y );
	if ( !target ) return;

	SendMessage ( target, GX_MSG_MMBUP, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnRightMouseButtonDown ( const GXVec2 &position )
{
	GXWidget* target = FindWidget ( position.x, position.y );
	if ( !target ) return;

	SendMessage ( target, GX_MSG_RMBDOWN, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnRightMouseButtonUp ( const GXVec2 &position )
{
	GXWidget* target = FindWidget ( position.x, position.y );
	if ( !target ) return;

	SendMessage ( target, GX_MSG_RMBUP, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::OnScroll ( const GXVec2 &position, GXFloat scroll )
{
	GXWidget* target = FindWidget ( position.x, position.y );
	if ( !target ) return;

	GXVec3 data = GXCreateVec3 ( position.x, position.y, scroll );

	SendMessage ( target, GX_MSG_SCROLL, &data, sizeof ( GXVec3 ) );
}

GXVoid GXTouchSurface::OnMouseMove ( const GXVec2 &position )
{
	GXWidget* target = FindWidget ( position.x, position.y );

	if ( target != mouseOverWidget )
	{
		if ( mouseOverWidget )
			SendMessage ( mouseOverWidget, GX_MSG_MOUSE_LEAVE, 0, 0 );

		mouseOverWidget = target;

		if ( target )
			SendMessage ( target, GX_MSG_MOUSE_OVER, 0, 0 );
	}

	if ( !target ) return;

	SendMessage ( target, GX_MSG_MOUSE_MOVE, &position, sizeof ( GXVec2 ) );
}

GXVoid GXTouchSurface::SendMessage ( GXWidget* widget, GXUInt message, const GXVoid* data, GXUInt size )
{
	GXUIMessage* msg = (GXUIMessage*)gx_ui_MessageBuffer->Allocate ( sizeof ( GXUIMessage ) );
	msg->next = 0;
	msg->widget = widget;
	msg->message = message;
	msg->data = gx_ui_MessageBuffer->Allocate ( size );
	if ( size )
		memcpy ( msg->data, data, size );
	msg->size = size;

	gx_ui_Mutex->Lock ();

	if ( !messages )
		messages = lastMessage = msg;
	else
	{
		lastMessage->next = msg;
		lastMessage = msg;
	}

	gx_ui_Mutex->Release ();
}

GXVoid GXTouchSurface::ExecuteMessages ()
{
	while ( messages )
	{
		GXUIMessage* msg = messages;

		gx_ui_Mutex->Lock ();
		messages = messages->next;
		gx_ui_Mutex->Release ();

		msg->widget->OnMessage ( msg->message, msg->data );
	}
}

GXVoid GXTouchSurface::DrawWidgets ()
{
	gx_ui_Mutex->Lock ();

	for ( GXWidget* p = gx_ui_Widgets; p; p = p->next )
	{
		if ( p->IsVisible () )
			p->OnDraw ();
	}

	gx_ui_Mutex->Release ();
}

GXVoid GXTouchSurface::DeleteWidgets ()
{
	while ( gx_ui_Widgets ) delete gx_ui_Widgets;
}

GXWidget* GXTouchSurface::FindWidget ( GXFloat x, GXFloat y )
{
	GXVec3 v = GXCreateVec3 ( x, y, 0.0f );

	gx_ui_Mutex->Lock ();
	for ( GXWidget* p = gx_ui_Widgets; p; p = p->next )
	{
		const GXAABB& aabb = p->GetBounds ();
		if ( p->IsVisible () && GXIsOverlapedAABBVec3 ( aabb, v ) )
		{
			gx_ui_Mutex->Release ();
			return p;
		}
	}
	gx_ui_Mutex->Release ();

	return 0;
}

//--------------------------------------------------------------------------

GXUIButton::GXUIButton ()
{
	OnLeftMouseButton = 0;

	isPressed = GX_FALSE;
	isDisabled = GX_FALSE;
	isHighlighted = GX_FALSE;
}

GXUIButton::~GXUIButton ()
{
	//NOTHING
}

GXVoid GXUIButton::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_LMBDOWN:
		{
			if ( isDisabled ) return;

			isPressed = GX_TRUE;

			if ( renderer )
				renderer->OnUpdate ();

			if ( !OnLeftMouseButton ) return;

			const GXVec2* position = (const GXVec2*)data;
			OnLeftMouseButton ( position->x, position->y, GX_MOUSE_BUTTON_DOWN );
		}
		break;

		case GX_MSG_LMBUP:
		{
			if ( isDisabled ) return;

			isPressed = GX_FALSE;

			if ( renderer )
				renderer->OnUpdate ();

			if ( !OnLeftMouseButton ) return;

			const GXVec2* position = (const GXVec2*)data;
			OnLeftMouseButton ( position->x, position->y, GX_MOUSE_BUTTON_UP );
		}
		break;

		case GX_MSG_MOUSE_OVER:
			if ( isDisabled ) return;

			isHighlighted = GX_TRUE;

			if ( renderer )
				renderer->OnUpdate ();
		break;

		case GX_MSG_MOUSE_LEAVE:
			if ( isDisabled ) return;

			isHighlighted = GX_FALSE;
			isPressed = GX_FALSE;

			if ( renderer )
				renderer->OnUpdate ();
		break;

		case GX_MSG_ENABLE:
			if ( !isDisabled ) return;

			isDisabled = GX_FALSE;
			isHighlighted = GX_FALSE;

			if ( renderer )
				renderer->OnUpdate ();
		break;

		case GX_MSG_DISABLE:
			if ( isDisabled ) return;

			isDisabled = GX_TRUE;

			if ( renderer )
				renderer->OnUpdate ();
		break;

		case GX_MSG_REDRAW:
			if ( renderer )
				renderer->OnUpdate ();
		break;

		default:
			GXWidget::OnMessage ( message, data );
		break;
	}
}

GXVoid GXUIButton::SetOnLeftMouseButtonCallback ( PFNGXONMOUSEBUTTONPROC callback )
{
	OnLeftMouseButton = callback;
}

GXVoid GXUIButton::Enable ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_ENABLE, 0, 0 );
}

GXVoid GXUIButton::Disable ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_DISABLE, 0, 0 );
}

GXVoid GXUIButton::Redraw ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_REDRAW, 0, 0 );
}

GXBool GXUIButton::IsPressed ()
{
	return isPressed;
}

GXBool GXUIButton::IsHighlighted ()
{
	return isHighlighted;
}

GXBool GXUIButton::IsDisabled ()
{
	return isDisabled;
}

//--------------------------------------------------------------------------

struct GXUIMenuItem
{
	GXAABB		bounds;
	GXBool		isPressed;
	GXBool		isHighlighted;
};

struct GXUIMenuResizeItem
{
	GXUByte		item;
	GXFloat		width;
};

GXUIMenu::GXUIMenu ( GXVoid* menuHandler, PFNGXONSHOWSUBMENUPROC onShowSubmenu ):
items ( sizeof ( GXUIMenuItem ) )
{
	this->menuHandler = menuHandler;
	OnShowSubmenu = onShowSubmenu;
}

GXUIMenu::~GXUIMenu ()
{
	//NOTHING
}

GXVoid GXUIMenu::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_RESIZE:
		{
			GXUInt total = items.GetLength ();

			if ( total == 0 )
			{
				GXWidget::OnMessage ( message, data );

				if ( renderer )
					renderer->OnUpdate ();

				return;
			}

			const GXAABB* b = (const GXAABB*)data;

			GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( 0 );
			GXVec2 delta = GXCreateVec2 ( b->min.x - item->bounds.min.x, b->min.y - item->bounds.min.y );

			for ( GXUInt i = 0; i < total; i++ )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( i );
				item->bounds.min.x += delta.x;
				item->bounds.min.y += delta.y;

				item->bounds.max.x += delta.x;
				item->bounds.max.y += delta.y;
			}

			GXSetAABBEmpty ( bounds );
			GXAddVertexToAABB ( bounds, item->bounds.min.x, item->bounds.min.y, item->bounds.min.z );

			if ( total > 1 )
				item = (GXUIMenuItem*)items.GetValue ( total - 1 );

			GXAddVertexToAABB ( bounds, item->bounds.max.x, item->bounds.max.y, item->bounds.max.z );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MENU_ADD_ITEM:
		{
			GXUIMenuItem item;
			item.isHighlighted = GX_FALSE;
			item.isPressed = GX_FALSE;
			GXSetAABBEmpty ( item.bounds );

			GXUInt total = items.GetLength ();
			if ( total == 0 )
			{
				GXAddVertexToAABB ( item.bounds, bounds.min.x, bounds.min.y, bounds.min.z );
				GXAddVertexToAABB ( item.bounds, bounds.min.x + gx_ui_Scale * 1.5f, bounds.max.y, bounds.max.z );

				memcpy ( &bounds, &item.bounds, sizeof ( GXAABB ) );
			}
			else
			{
				GXUIMenuItem* last = (GXUIMenuItem*)items.GetValue ( total - 1 );
				GXAddVertexToAABB ( item.bounds, last->bounds.max.x, last->bounds.min.y, last->bounds.min.z );
				GXAddVertexToAABB ( item.bounds, last->bounds.max.x + gx_ui_Scale * 1.5f, bounds.max.y, last->bounds.max.z );

				GXUIMenuItem* first = (GXUIMenuItem*)items.GetValue ( 0 );
				GXSetAABBEmpty ( bounds );
				GXAddVertexToAABB ( bounds, first->bounds.min.x, first->bounds.min.y, first->bounds.min.z );
				GXAddVertexToAABB ( bounds, item.bounds.max.x, item.bounds.max.y, item.bounds.max.z );
			}

			items.SetValue ( total, &item );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MENU_RESIZE_ITEM:
		{
			const GXUIMenuResizeItem* ri = (const GXUIMenuResizeItem*)data;

			GXUByte total = (GXUByte)items.GetLength ();
			if ( total < ri->item + 1 ) return;

			GXUIMenuItem* p = (GXUIMenuItem*)items.GetValue ( ri->item );
			GXFloat delta = ri->width - ( p->bounds.max.x - p->bounds.min.x );

			p->bounds.max.x += delta;

			for ( GXUByte i = ri->item + 1; i < total; i++ )
			{
				p = (GXUIMenuItem*)items.GetValue ( i );
				p->bounds.min.x += delta;
				p->bounds.max.x += delta;
			}

			p = (GXUIMenuItem*)items.GetValue ( total - 1 );
			bounds.max.x = p->bounds.max.x;

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOUSE_MOVE:
		{
			const GXVec2* pos = (const GXVec2*)data;

			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			GXUIMenuItem* itemMouseOver = 0;
			GXUByte itemIndex = 0xFF;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( i );
				if ( GXIsOverlapedAABBVec3 ( item->bounds, pos->x, pos->y, 0.0f ) )
				{
					itemIndex = i;
					itemMouseOver = item;
					break;
				}
			}

			if ( itemMouseOver->isHighlighted ) return;

			itemMouseOver->isHighlighted = GX_TRUE;
			
			if ( itemIndex != 0 )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( itemIndex - 1 );
				item->isHighlighted = GX_FALSE;
				item->isPressed = GX_FALSE;
			}

			if ( itemIndex < total - 1 )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( itemIndex + 1 );
				item->isHighlighted = GX_FALSE;
				item->isPressed = GX_FALSE;
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOUSE_LEAVE:
		{
			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( i );
				item->isHighlighted = GX_FALSE;
				item->isPressed = GX_FALSE;
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_REDRAW:
			if ( renderer )
				renderer->OnUpdate ();
		break;

		case GX_MSG_LMBDOWN:
		{
			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( i );
				if ( item->isHighlighted )
				{
					item->isPressed = GX_TRUE;
					OnShowSubmenu ( menuHandler, i );
					break;
				}
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LMBUP:
		{
			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUIMenuItem* item = (GXUIMenuItem*)items.GetValue ( i );
				if ( item->isHighlighted )
				{
					item->isPressed = GX_FALSE;
					break;
				}
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		default:
		break;
	}
}

GXVoid GXUIMenu::AddItem ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_MENU_ADD_ITEM, 0, 0 );
}

GXVoid GXUIMenu::ResizeItem ( GXUByte item, GXFloat width )
{
	GXUIMenuResizeItem ri;
	ri.item = item;
	ri.width = width;

	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_MENU_RESIZE_ITEM, &ri, sizeof ( GXUIMenuResizeItem ) );
}

GXUByte GXUIMenu::GetTotalItems ()
{
	return (GXUByte)items.GetLength ();
}

const GXAABB& GXUIMenu::GetItemBounds ( GXUByte item )
{
	GXUIMenuItem* p = (GXUIMenuItem*)items.GetValue ( item );
	return p->bounds;
}

GXVoid GXUIMenu::Redraw ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_REDRAW, 0, 0 );
}

GXBool GXUIMenu::IsItemPressed ( GXUByte item )
{
	GXUIMenuItem* p = (GXUIMenuItem*)items.GetValue ( item );
	return p->isPressed;
}

GXBool GXUIMenu::IsItemHighlighted ( GXUByte item )
{
	GXUIMenuItem* p = (GXUIMenuItem*)items.GetValue ( item );
	return p->isHighlighted;
}

//--------------------------------------------------------------------------

struct GXUISubmenuItem
{
	GXAABB						bounds;
	PFNGXONMOUSEBUTTONPROC		OnMouseButton;
	GXBool						isPressed;
	GXBool						isHighlighted;
};

GXUISubmenu::GXUISubmenu ():
items ( sizeof ( GXUISubmenuItem ) )
{
	itemHeight = 0.5f * gx_ui_Scale;
}

GXUISubmenu::~GXUISubmenu ()
{
	//NOTHING
}

GXVoid GXUISubmenu::OnMessage ( GXUInt message, const GXVoid* data )
{
	switch ( message )
	{
		case GX_MSG_RESIZE:
		{
			GXUByte total = items.GetLength ();
			if ( total == 0 )
			{
				const GXAABB* newBounds = (const GXAABB*)data;
				itemHeight = newBounds->max.y - newBounds->min.y;

				GXWidget::OnMessage ( message, data );

				if ( renderer )
					renderer->OnUpdate ();

				return;
			}

			const GXAABB* b = (const GXAABB*)data;

			GXUISubmenuItem* bottomItem = (GXUISubmenuItem*)items.GetValue ( total - 1 );
			GXVec2 delta = GXCreateVec2 ( b->min.x - bottomItem->bounds.min.x, b->min.y - bottomItem->bounds.min.y );

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( i );
				item->bounds.min.x += delta.x;
				item->bounds.min.y += delta.y;

				item->bounds.max.x += delta.x;
				item->bounds.max.y += delta.y;
			}

			GXSetAABBEmpty ( bounds );
			GXAddVertexToAABB ( bounds, bottomItem->bounds.min.x, bottomItem->bounds.min.y, bottomItem->bounds.min.z );

			if ( total > 1 )
			{
				GXUISubmenuItem* topItem = (GXUISubmenuItem*)items.GetValue ( 0 );
				GXAddVertexToAABB ( bounds, topItem->bounds.max.x, topItem->bounds.max.y, topItem->bounds.max.z );
			}
			else
				GXAddVertexToAABB ( bounds, bottomItem->bounds.max.x, bottomItem->bounds.max.y, bottomItem->bounds.max.z );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_SUBMENU_ADD_ITEM:
		{
			GXUISubmenuItem item;
			item.isHighlighted = item.isPressed = GX_FALSE;
			item.OnMouseButton = *( (PFNGXONMOUSEBUTTONPROC*)data );

			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 )
			{
				GXAddVertexToAABB ( item.bounds, bounds.min.x, bounds.min.y, bounds.min.z );
				GXAddVertexToAABB ( item.bounds, bounds.max.x, bounds.max.y, bounds.max.z );
			}
			else
			{
				GXUISubmenuItem* i = (GXUISubmenuItem*)items.GetValue ( total - 1 );
				GXAddVertexToAABB ( item.bounds, i->bounds.min.x, i->bounds.min.y - itemHeight, i->bounds.min.z );
				GXAddVertexToAABB ( item.bounds, i->bounds.max.x, i->bounds.min.y, i->bounds.max.z );
				

				bounds.min.x = item.bounds.min.x;
				bounds.min.y = item.bounds.min.y;
			}

			items.SetValue ( total, &item );

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_SUBMENU_SET_HEIGHT:
		{
			const GXFloat* height = (const GXFloat*)data;
			itemHeight = *height;

			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 )
			{
				bounds.max.y = bounds.min.y + itemHeight;
				return;
			}

			GXFloat y = bounds.min.y;
			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( total - i - 1 );
				item->bounds.min.y = y;
				y += itemHeight;
				item->bounds.max.y = y;
			}

			GXUISubmenuItem* topItem = (GXUISubmenuItem*)items.GetValue ( 0 );
			bounds.max.y = topItem->bounds.max.y;

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOUSE_MOVE:
		{
			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			const GXVec2* pos = (const GXVec2*)data;

			GXUISubmenuItem* itemMouseOver = 0;
			GXUByte itemIndex;

			for ( itemIndex = 0; itemIndex < total; itemIndex++ )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( itemIndex );
				if ( GXIsOverlapedAABBVec3 ( item->bounds, pos->x, pos->y, 0.0f ) )
				{
					itemMouseOver = item;
					break;
				}
			}

			if ( itemMouseOver->isHighlighted ) return;

			itemMouseOver->isHighlighted = GX_TRUE;
			
			if ( itemIndex != 0 )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( itemIndex - 1 );
				item->isHighlighted = GX_FALSE;
				item->isPressed = GX_FALSE;
			}

			if ( itemIndex < total - 1 )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( itemIndex + 1 );
				item->isHighlighted = GX_FALSE;
				item->isPressed = GX_FALSE;
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_MOUSE_LEAVE:
		{
			Hide ();

			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( i );
				item->isHighlighted = GX_FALSE;
				item->isPressed = GX_FALSE;
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LMBDOWN:
		{
			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( i );
				if ( item->isHighlighted )
				{
					item->isPressed = GX_TRUE;
					break;
				}
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_LMBUP:
		{
			Hide ();

			GXUByte total = (GXUByte)items.GetLength ();
			if ( total == 0 ) return;

			for ( GXUByte i = 0; i < total; i++ )
			{
				GXUISubmenuItem* item = (GXUISubmenuItem*)items.GetValue ( i );
				if ( item->isHighlighted )
				{
					item->isPressed = GX_FALSE;
					if ( item->OnMouseButton )
					{
						const GXVec2* pos = (const GXVec2*)data;
						item->OnMouseButton ( pos->x, pos->y, GX_MOUSE_BUTTON_UP );
					}
					break;
				}
			}

			if ( renderer )
				renderer->OnUpdate ();
		}
		break;

		case GX_MSG_REDRAW:
			if ( renderer )
				renderer->OnUpdate ();
		break;

		default:
			GXWidget::OnMessage ( message, data );
		break;
	}
}

GXVoid GXUISubmenu::AddItem ( PFNGXONMOUSEBUTTONPROC callback )
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_SUBMENU_ADD_ITEM, &callback, sizeof ( PFNGXONMOUSEBUTTONPROC ) );
}

GXVoid GXUISubmenu::SetItemHeight ( GXFloat height )
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_SUBMENU_SET_HEIGHT, &height, sizeof ( GXFloat ) );
}

GXUByte GXUISubmenu::GetTotalItems ()
{
	return (GXUByte)items.GetLength ();
}

GXFloat GXUISubmenu::GetItemHeight ()
{
	return itemHeight;
}

GXVoid GXUISubmenu::Redraw ()
{
	gx_ui_TouchSurface->SendMessage ( this, GX_MSG_REDRAW, 0, 0 );
}

GXBool GXUISubmenu::IsItemPressed ( GXUByte item )
{
	GXUISubmenuItem* i = (GXUISubmenuItem*)items.GetValue ( item );
	return i->isPressed;
}

GXBool GXUISubmenu::IsItemHighlighted ( GXUByte item )
{
	GXUISubmenuItem* i = (GXUISubmenuItem*)items.GetValue ( item );
	return i->isHighlighted;
}
