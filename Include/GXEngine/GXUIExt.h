//version 1.0

#ifndef GX_UI_EXT
#define GX_UI_EXT


#include <GXCommon/GXMath.h>
#include <GXCommon/GXMutex.h>
#include <GXCommon/GXMemory.h>


enum eGXMouseButtonState : GXUByte
{
	GX_MOUSE_BUTTON_DOWN,
	GX_MOUSE_BUTTON_UP
};

class GXWidget;

typedef GXVoid ( GXCALL* PFNGXONMOUSEBUTTONPROC ) ( GXFloat x, GXFloat y, eGXMouseButtonState );
typedef GXVoid ( GXCALL* PFNGXONMOUSESCROLLPROC ) ( GXFloat scroll, GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* PFNGXONMOUSEMOVEPROC ) ( GXFloat x, GXFloat y );
typedef GXVoid ( GXCALL* PFNGXONSHOWSUBMENUPROC ) ( GXVoid* menuHandler, GXUByte item );

extern GXFloat gx_ui_Scale;		//rough pixels in centimeter

class GXWidgetRenderer
{
	protected:
		GXWidget*	widget;

	public:
		GXWidgetRenderer ( GXWidget* widget );
		virtual ~GXWidgetRenderer ();

		virtual GXVoid Update ();
		virtual GXVoid Draw ();
};

class GXWidget
{
	friend class GXTouchSurface;

	private:
		GXWidget*				next;
		GXBool					isVisible;

	protected:
		GXWidgetRenderer*		renderer;
		GXAABB					bounds;

	public:
		GXWidget ();
		virtual ~GXWidget ();

		virtual GXVoid OnMessage ( GXUInt message, const GXVoid* data );

		GXVoid OnDraw ();
		GXVoid Resize ( GXFloat x, GXFloat y, GXFloat width, GXFloat height );
		const GXAABB& GetBounds ();

		GXVoid Show ();
		GXVoid Hide ();
		GXBool IsVisible ();

		GXVoid SetRenderer ( GXWidgetRenderer* renderer );
		GXWidgetRenderer* GetRenderer ();
};

//-----------------------------------------------------------------------------------

struct GXUIMessage
{
	GXUIMessage*	next;
	GXWidget*		widget;
	GXUInt			message;
	GXVoid*			data;
	GXUInt			size;
};

class GXTouchSurface
{
	private:
		GXUIMessage*	messages;
		GXUIMessage*	lastMessage;
		GXWidget*		mouseOverWidget;

	public:
		GXTouchSurface ();
		~GXTouchSurface ();

		GXVoid OnLeftMouseButtonDown ( const GXVec2 &position );
		GXVoid OnLeftMouseButtonUp ( const GXVec2 &position );
		GXVoid OnMiddleMouseButtonDown ( const GXVec2 &position );
		GXVoid OnMiddleMouseButtonUp ( const GXVec2 &position );
		GXVoid OnRightMouseButtonDown ( const GXVec2 &position );
		GXVoid OnRightMouseButtonUp ( const GXVec2 &position );

		GXVoid OnScroll ( const GXVec2 &position, GXFloat scroll );
		GXVoid OnMouseMove ( const GXVec2 &position );

		GXVoid SendMessage ( GXWidget* widget, GXUInt message, const GXVoid* data, GXUInt size );
		GXVoid ExecuteMessages ();

		GXVoid DrawWidgets ();

	private:
		GXVoid DeleteWidgets ();
		GXWidget* FindWidget ( GXFloat x, GXFloat y );
};

//-----------------------------------------------------------------------------------

class GXUIButton : public GXWidget
{
	private:
		PFNGXONMOUSEBUTTONPROC		OnLeftMouseButton;

		GXBool						isPressed;
		GXBool						isHighlighted;
		GXBool						isDisabled;

	public:
		GXUIButton ();
		virtual ~GXUIButton ();

		virtual GXVoid OnMessage ( GXUInt message, const GXVoid* data );

		GXVoid SetOnLeftMouseButtonCallback ( PFNGXONMOUSEBUTTONPROC callback );

		GXVoid Enable ();
		GXVoid Disable ();
		GXVoid Redraw ();

		GXBool IsPressed ();
		GXBool IsHighlighted ();
		GXBool IsDisabled ();
};

//-----------------------------------------------------------------------------------

class GXUIMenu : public GXWidget
{
	private:
		GXDynamicArray			items;
		GXVoid*					menuHandler;
		PFNGXONSHOWSUBMENUPROC	OnShowSubmenu;

	public:
		GXUIMenu ( GXVoid* menuHandler, PFNGXONSHOWSUBMENUPROC onShowSubmenu );
		virtual ~GXUIMenu ();

		virtual GXVoid OnMessage ( GXUInt message, const GXVoid* data );

		GXVoid AddItem ();
		GXVoid ResizeItem ( GXUByte item, GXFloat width );
		GXUByte GetTotalItems ();
		const GXAABB& GetItemBounds ( GXUByte item );

		GXVoid Redraw ();

		GXBool IsItemPressed ( GXUByte item );
		GXBool IsItemHighlighted ( GXUByte item );
};

//-----------------------------------------------------------------------------------

class GXUISubmenu : public GXWidget
{
	private:
		GXDynamicArray		items;
		GXFloat				itemHeight;

	public:
		GXUISubmenu ();
		virtual ~GXUISubmenu ();

		virtual GXVoid OnMessage ( GXUInt message, const GXVoid* data );

		GXVoid AddItem ( PFNGXONMOUSEBUTTONPROC callback );
		GXVoid SetItemHeight ( GXFloat height );
		GXUByte GetTotalItems ();
		GXFloat GetItemHeight ();

		GXVoid Redraw ();

		GXBool IsItemPressed ( GXUByte item );
		GXBool IsItemHighlighted ( GXUByte item );
};


#endif //GX_UI_EXT
