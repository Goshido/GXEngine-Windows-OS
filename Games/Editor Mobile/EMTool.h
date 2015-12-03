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

		virtual GXVoid Bind ( EMActor* actor );
		virtual GXVoid UnBind ();

		virtual GXVoid OnDrawCommonPass ();
		virtual GXVoid OnDrawHudDepthDependentPass ();
		virtual GXVoid OnDrawHudDepthIndependentPass ();
};


#endif //EM_TOOL
