//version 1.11

#ifndef GX_RENDERER
#define GX_RENDERER


#include <GXCommon/GXThread.h>


#define GX_RENDERER_SUPPORTED_POSTEFFECTS	2

#define GX_WINDOW_OPENGL_CLASS	L"GX_RENDERER_CLASS"

typedef GXBool ( GXCALL* PFNGXONFRAMEPROC ) ( GXFloat deltatime );
typedef GXVoid ( GXCALL* PFNGXONINITRENDERABLEOBJECTSPROC ) ();
typedef GXVoid ( GXCALL* PFNGXONDELETERENDERABLEOBJECTSPROC ) ();


class GXRendererResolutions
{
	friend class GXRenderer;

	private:
		GXUShort	total;
		GXUShort*	wxh;

	public:
		GXRendererResolutions ();
		~GXRendererResolutions ();
		GXVoid Cleanup ();

		GXUShort GetTotalResolutions ();
		GXVoid GetResolution ( GXUShort i, GXUShort &width, GXUShort &height );	
};

class GXCore;
class GXRenderer
{	
	private:
		static GXCore*								core;
		static GXThread*							thread;

		static GXBool								loopFlag;

		static HWND									hwnd;
		static HGLRC								hglRC;
		static HINSTANCE							hinst;
		static HDC									hDC;

		static GXBool								isFullScreen;
		static GXBool								isSettingsChanged;

		static GXInt								width;
		static GXInt								height;
		static GXInt								vsync;
		static GXInt								colorsBits;
		static GXInt								depthBits;

		static GXWChar*								title;

		static GXDouble								lastTime;
		static GXDouble								accumulator;
		static GXUInt								currentFPS;

		static PFNGXONFRAMEPROC						onFrameFunc;
		static PFNGXONINITRENDERABLEOBJECTSPROC		onInitRenderableObjectsFunc;
		static PFNGXONDELETERENDERABLEOBJECTSPROC	onDeleteRenderableObjectsFunc;
		static GXBool								isRenderableObjectInited;

	public:
		GXRenderer ( GXCore* core, const GXWChar* gameName, GXInt color_bits, GXInt depth_bits );
		~GXRenderer ();

		GXVoid Start ();
		GXVoid Suspend ();
		GXBool Shutdown ();

		GXVoid SetFullscreen ( GXBool enabled );
		GXVoid SetVSync ( GXBool enabled );
		GXVoid SetColorsBits ( GXInt bits );
		GXVoid SetDepthBits ( GXInt bits );
		GXVoid SetResolution ( GXInt width, GXInt height );
		GXVoid SetWindowTitle ( const GXWChar* title );

		GXVoid SetOnFrameFunc ( PFNGXONFRAMEPROC callback );
		GXVoid SetOnInitRenderableObjectsFunc ( PFNGXONINITRENDERABLEOBJECTSPROC callback );
		GXVoid SetOnDeleteRenderableObjectsFunc ( PFNGXONDELETERENDERABLEOBJECTSPROC callback );

		GXUInt GetCurrentFPS ();
		GXVoid GetSupportedResolutions ( GXRendererResolutions &out );

		GXInt GetWidth ();
		GXInt GetHeight ();
		static GXBool UpdateRendererSettings ();
		static GXVoid ReSizeScene ( GXInt width, GXInt height );

	private:
		static GXDword GXTHREADCALL RenderLoop ( GXVoid* args );
		static GXVoid InitOpenGL ();
		static GXBool DrawScene ();
		static GXVoid Destroy ();
		static GXBool MakeWindow ();
		static GXVoid InitRenderableObjects ();
		static GXVoid DeleteRenderableObjects ();
};


#endif //GX_RENDERER