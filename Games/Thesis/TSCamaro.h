//version 1.2


#ifndef TS_CAMARO
#define TS_CAMARO


#include <GXEngine/GXMesh.h>


class TSCamaro : public GXMesh
{
	protected:
		GLuint		mod_view_proj_matLocation;
		GLuint		mod_view_matLocation;

	public:
		TSCamaro ();
		virtual ~TSCamaro ();

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif //TS_CAMARO