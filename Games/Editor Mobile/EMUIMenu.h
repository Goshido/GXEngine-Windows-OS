#ifndef EM_UI_MENU
#define EM_UI_MENU


#include <GXEngine/GXUIExt.h>


class EMUIMenu
{
	private:
		GXUIMenu*		menuWidget;
		GXDynamicArray	submenuWidgets;

	public:
		EMUIMenu ();
		~EMUIMenu ();

		GXVoid AddItem ( const GXWChar* caption );
		GXVoid RenameItem ( GXUByte item, const GXWChar* caption );

		GXVoid AddSubitem ( GXUByte item, const GXWChar* caption, const GXWChar* hotkey, PFNGXONMOUSEBUTTONPROC onMouseButton );
		GXVoid RenameSubitem ( GXUByte item, GXUByte subitem, const GXWChar* caption, const GXWChar* hotkey );

		GXVoid SetLocation ( GXFloat x, GXFloat y );
		GXVoid SetLayer ( GXFloat z );

	private:
		static GXVoid GXCALL OnShowSubmenu ( GXVoid* menuHandler, GXUByte item );
};


#endif //EM_UI_MENU
