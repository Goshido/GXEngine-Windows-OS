#ifndef EM_MOVE_TOOL
#define EM_MOVE_TOOL


#include "EMTool.h"
#include <GXEngine/GXInput.h>
#include <GXEngine/GXVAOStorage.h>
#include <GXCommon/GXMTRStructs.h>


class EMMoveTool : public EMTool
{
	private:
		GXMat4			origin;
		GXUByte			mode;

		GXVAOInfo		xAxis;
		GXVAOInfo		xAxisMask;
		GXVAOInfo		yAxis;
		GXVAOInfo		yAxisMask;
		GXVAOInfo		zAxis;
		GXVAOInfo		zAxisMask;
		GXVAOInfo		center;

		GXUByte			activeAxis;

		GXShaderInfo	colorShader;
		GXShaderInfo	maskShader;

		GLint			clr_mod_view_proj_matLocation;
		GLint			clr_colorLocation;
		GLint			msk_mod_view_proj_matLocation;

		GXBool			isDeleted;
		GXBool			isLMBPressed;

		GXUShort		mouseX;
		GXUShort		mouseY;
		GXUShort		mouseBeginX;
		GXUShort		mouseBeginY;

	public:
		EMMoveTool ();
		GXVoid Delete ();

		virtual GXVoid Bind ();
		virtual GXVoid SetActor ( EMActor* actor );
		virtual GXVoid UnBind ();

		virtual GXVoid OnDrawHudColorPass ();
		virtual GXVoid OnDrawHudMaskPass ();

		GXVoid SetLocalMode ();
		GXVoid SetWorldMode ();

	private:
		virtual ~EMMoveTool ();

		GXVoid Load3DModels ();
		GXVoid InitUniforms ();

		GXVoid OnMoveActor ();

		static GXVoid GXCALL OnObject ( GXUInt object );
		static GXVoid GXCALL OnMouseMove ( GXInt win_x, GXInt win_y );
		static GXVoid GXCALL OnMouseButton ( EGXInputMouseFlags mouseflags );
};


#endif //EM_MOVE_TOOL
