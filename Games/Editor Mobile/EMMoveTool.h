#ifndef EM_MOVE_TOOL
#define EM_MOVE_TOOL


#include "EMTool.h"
#include <GXEngine/GXInput.h>
#include <GXEngine/GXVAOStorage.h>
#include <GXCommon/GXMTRStructs.h>


class EMMoveTool : public EMTool
{
	private:
		GXVec3			startLocationWorld;
		GXVec3			deltaWorld;
		GXFloat			gismoScaleCorrector;
		GXFloat			axisStartParameter;
		GXUByte			mode;
		GXUByte			activeAxis;

		GXBool			isLMBPressed;
		GXUShort		mouseX;
		GXUShort		mouseY;

		GXVAOInfo		xAxis;
		GXVAOInfo		xAxisMask;
		GXVAOInfo		yAxis;
		GXVAOInfo		yAxisMask;
		GXVAOInfo		zAxis;
		GXVAOInfo		zAxisMask;
		GXVAOInfo		center;

		GXShaderInfo	colorShader;
		GXShaderInfo	maskShader;

		GLint			clr_mod_view_proj_matLocation;
		GLint			clr_colorLocation;
		GLint			msk_mod_view_proj_matLocation;

		GXBool			isDeleted;

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

		GXFloat GetScaleFactor ( const GXVec3 &deltaWorld );
		GXVoid GetAxis ( GXVec3& axisView );
		GXVoid GetRayPerspective ( GXVec3 &rayView );
		GXFloat GetAxisParameter ( const GXVec3 &axisLocationView, const GXVec3 &axisDirectionView, const GXVec3 &rayView );

		static GXVoid GXCALL OnObject ( GXUInt object );
		static GXVoid GXCALL OnMouseMove ( GXInt win_x, GXInt win_y );
		static GXVoid GXCALL OnMouseButton ( EGXInputMouseFlags mouseflags );
};


#endif //EM_MOVE_TOOL
