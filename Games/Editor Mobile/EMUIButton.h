#ifndef EM_UI_BUTTON
#define EM_UI_BUTTON


#include <GXEngine/GXUIExt.h>

class EMUIButton
{
	private:
		GXUIButton*		widget;

	public:
		EMUIButton ();
		~EMUIButton ();

		GXVoid Enable ();
		GXVoid Disable ();

		GXVoid Resize ( GXFloat x, GXFloat y, GXFloat width, GXFloat height );
		GXVoid SetCaption ( const GXWChar* caption );
		GXVoid SetLayer ( GXFloat z );

		GXVoid Show ();
		GXVoid Hide ();

		GXVoid SetOnLeftMouseButtonCallback ( PFNGXONMOUSEBUTTONPROC callback );
};


#endif //EM_UI_BUTTON
