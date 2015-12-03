//version 1.1

#ifndef TS_MESH_TANGENT
#define TS_MESH_TANGENT


#include <GXEngine/GXMesh.h>


class TSMeshTangent : public GXMesh
{
	protected:
		GXBool		twoSided;

		GLuint		mod_view_proj_matLocation;
		GLuint		mod_view_matLocation;

		GXWChar*	meshFile;
		GXWChar*	materialFile;

		GXBool		isHidden;
		GXBool		isDelete;

	public:
		TSMeshTangent ( const GXWChar* meshFile, const GXWChar* materialFile, GXBool twoSided = GX_FALSE );

		virtual GXVoid Draw ();

		GXVoid Hide ( GXBool hidden );
		GXVoid Delete ();

	protected:
		virtual ~TSMeshTangent ();

		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();

		GXVoid InitGraphicalResources ();
};



#endif //TS_MESH_TANGENT