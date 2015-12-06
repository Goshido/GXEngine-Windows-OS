#ifndef EM_MOVE_TOOL
#define EM_MOVE_TOOL


#include "EMTool.h"


class EMMoveTool : public EMTool
{
	public:
		EMMoveTool ();
		virtual ~EMMoveTool ();

		virtual GXVoid Bind ();
		virtual GXVoid SetActor ( EMActor* actor );
		virtual GXVoid UnBind ();

		virtual GXVoid OnDrawHudColorPass ();
		virtual GXVoid OnDrawHudMaskPass ();
};


#endif //EM_MOVE_TOOL
