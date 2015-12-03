//version 1.6

#ifndef GX_STATIC_MESH_BALL
#define GX_STATIC_MESH_BALL


#include <GXEngine/GXMesh.h>
#include <GXEngine/GXGlobals.h>


class GXStaticMeshBall : public GXMesh
{
	protected:
		GLuint groundTexture;
		GLuint mod_view_proj_mat_Location;

	public:
		GXStaticMeshBall ();
		virtual ~GXStaticMeshBall ();

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif //GX_STATIC_MESH_BALL