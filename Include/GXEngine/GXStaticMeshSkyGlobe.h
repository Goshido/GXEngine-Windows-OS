//version 1.4

#ifndef GX_STATIC_MESH_SKY_GLOBE
#define GX_STATIC_MESH_SKY_GLOBE


#include "GXMesh.h"


class GXStaticMeshSkyGlobe : public GXMesh
{
	protected:
		GLuint skyTexture;
		GLuint mod_view_proj_mat_Location;

	public:
		GXStaticMeshSkyGlobe ();
		virtual ~GXStaticMeshSkyGlobe ();

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif //GX_STATIC_MESH_SKY_GLOBE