//version 1.2

#ifndef TS_BILLBOARD
#define TS_BILLBOARD


#include <GXEngine/GXMesh.h>


class TSBillboard : public GXMesh
{
	protected:
		GXVAOInfo	lockedQuad;
		GLuint		mod_view_proj_matLocation;
		GXWChar*	materialFile;

		GXUChar		lockedAxis;

		GXBool		isDelete;

	public:
		TSBillboard ( const GXWChar* materialFile );

		virtual GXVoid Draw ();
		GXVoid Delete ();

		GXVoid LockRight ();
		GXVoid LockUp ();
		GXVoid LockForward ();
		GXVoid UnlockAxis ();

	protected:
		virtual ~TSBillboard ();

		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();

		GXVoid InitGraphicalResources ();
};



#endif //TS_BILLBOARD