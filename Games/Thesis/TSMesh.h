//version 1.2

#ifndef TS_MESH
#define TS_MESH


#include <GXEngine/GXMesh.h>


class TSMesh : public GXMesh
{
	protected:
		GXBool		twoSided;

		GLuint		mod_view_proj_matLocation;
		GLuint		mod_view_matLocation;

		GXWChar*	meshFile;
		GXWChar*	meshCache;
		GXWChar*	materialFile;

		GXBool		isHidden;
		GXBool		isDelete;

	public:
		TSMesh ( GXWChar* meshFile, GXWChar* meshCache, GXWChar* materialFile, GXBool twoSided = GX_FALSE );

		virtual GXVoid Draw ();

		GXVoid Hide ( GXBool hidden );
		GXVoid Delete ();

	protected:
		virtual ~TSMesh ();

		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();

		GXVoid InitGraphicalResources ();
};



#endif //TS_MESH