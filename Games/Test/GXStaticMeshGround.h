//version 1.6

#ifndef GX_STATIC_MESH_SKY_GROUND
#define GX_STATIC_MESH_SKY_GROUND


#include <GXEngine/GXMesh.h>
#include <GXEngine/GXGlobals.h>


class GXStaticMeshGround : public GXMesh
{
	protected:
		GLuint groundTexture;
		GLuint mod_view_proj_mat_Location;

	public:
		GXStaticMeshGround ();
		virtual ~GXStaticMeshGround ();

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif //GX_STATIC_MESH_SKY_GROUND