//version 1.3

#ifndef TS_RENDERER_OBJECT
#define TS_RENDERER_OBJECT


#include "TSGBuffer.h"
#include <GXCommon/GXMutex.h>


extern GXMutex* ts_renderObjectMutex;

class TSRenderObject
{
	friend GXVoid GXCALL TSRenderObjectOnUpdate ( GXFloat deltaTime );
	friend GXVoid GXCALL TSRenderObjectOnDraw ( eTSGBufferPass pass );
	friend GXVoid GXCALL TSRenderObjectDestroy ();

	private:
		TSRenderObject*		prev;
		TSRenderObject*		next;

	public:
		TSRenderObject ();
		virtual GXVoid Delete ();

		virtual GXVoid OnUpdate ( GXFloat deltaTime );
		virtual GXVoid OnDraw ( eTSGBufferPass pass ) = 0;

	protected:
		virtual ~TSRenderObject ();
};

//====================================================================================================

GXVoid GXCALL TSRenderObjectInit ();
GXVoid GXCALL TSRenderObjectOnUpdate ( GXFloat deltaTime );
GXVoid GXCALL TSRenderObjectOnDraw ( eTSGBufferPass pass );
GXVoid GXCALL TSRenderObjectDestroy ();


#endif //TS_RENDERER_OBJECT