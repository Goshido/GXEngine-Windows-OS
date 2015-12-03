//version 1.2

#ifndef TS_CHALLENGER
#define TS_CHALLENGER


#include <GXEngine/GXMesh.h>

class TSChallenger : public GXMesh
{
	protected:
		GLuint		mod_view_proj_matLocation;
		GLuint		mod_view_matLocation;

	public:
		TSChallenger ();
		virtual ~TSChallenger ();

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};



#endif //TS_CHALLENGER