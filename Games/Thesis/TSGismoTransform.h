//version 1.1

#ifndef TS_GISMO_TRANSFORM
#define TS_GISMO_TRANSFORM


#include <GXEngine/GXMesh.h>


class TSGismoTransform : public GXMesh
{
	protected:
		GLuint	meshVAO;
		GLuint	meshVBO;
		GXUInt	numVertices;

		GLuint	mod_view_proj_matLocation;

	public:
		TSGismoTransform ();
		virtual ~TSGismoTransform ();

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif //TS_GISMO_TRANSFORM