//version 1.3

#ifndef GX_SKY_GLOBE
#define GX_SKY_GLOBE


#include "GXMesh.h"
#include "GXGlobals.h"


class GXSkyGlobe
{
	protected:
		GXMesh* mesh;

	public:
		GXSkyGlobe ();
		virtual ~GXSkyGlobe ();

		virtual GXVoid Draw ();
};


#endif	//GX_SKY_GLOBE