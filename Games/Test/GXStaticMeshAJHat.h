//version 1.7

#ifndef GX_STATIC_MESH_AJ_HAT
#define GX_STATIC_MESH_AJ_HAT


#include <GXEngine/GXMesh.h>
#include <GXEngine/GXGlobals.h>


class GXStaticMeshAJHat : public GXMesh
{
	protected:
		GLuint groundTexture;
		GLuint mod_view_proj_mat_Location;

	public:
		GXStaticMeshAJHat ();
		virtual ~GXStaticMeshAJHat ();

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif //GX_STATIC_MESH_AJ_HAT