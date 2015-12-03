#ifndef EM_MOVE_TOOL
#define EM_MOVE_TOOL


#include "EMTool.h"


class EMMoveTool : public EMTool
{
	public:
		EMMoveTool ();
		virtual ~EMMoveTool ();

		virtual GXVoid Bind ( EMActor* actor );
		virtual GXVoid UnBind ();

		virtual GXVoid OnDrawHudDepthIndependentPass ();
};


#endif //EM_MOVE_TOOL
