#ifndef EM_TOOL
#define EM_TOOL


#include "EMActor.h"


class EMTool
{
	protected:
		EMActor*		actor;

	public:
		EMTool ();
		virtual ~EMTool ();

		virtual GXVoid Bind ();
		virtual GXVoid SetActor ( EMActor* actor );
		virtual GXVoid UnBind ();

		virtual GXVoid OnDrawCommonPass ();
		virtual GXVoid OnDrawHudColorPass ();
		virtual GXVoid OnDrawHudMaskPass ();
};


#endif //EM_TOOL
