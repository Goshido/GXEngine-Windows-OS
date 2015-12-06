#ifndef EM_UI
#define EM_UI


#include <GXCommon/GXTypes.h>


class EMUI
{
	friend GXVoid GXCALL EMDrawUI ();
	friend GXVoid GXCALL EMDrawUIMasks ();

	private:
		EMUI*	next;
		EMUI*	prev;

	public:
		EMUI ();
		virtual ~EMUI ();

		virtual GXVoid OnDraw ();
		virtual GXVoid OnDrawMask ();
};


GXVoid GXCALL EMDrawUI ();
GXVoid GXCALL EMDrawUIMasks ();


#endif //EM_UI
