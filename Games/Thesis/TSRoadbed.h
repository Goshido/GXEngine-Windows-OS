//version 1.2

#ifndef TS_ROADBED
#define TS_ROADBED


#include <GXEngine/GXMesh.h>


class TSRoadbed : public GXMesh
{
	protected:
		GLuint		mod_view_proj_matLocation;
		GLuint		mod_view_matLocation;

	public:
		TSRoadbed ();
		virtual ~TSRoadbed ();

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif //TS_ROADBED