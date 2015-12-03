//version 1.0

#ifndef TS_GRASS
#define TS_GRASS


#include "TSMesh.h"


class TSGrass : public TSMesh
{
	protected:
		GLuint		offsetLocation;

		GXFloat		accum;
			
		GXVec2		offset;
		GXFloat		constOffset;
		GXVec2		direction;
		GXVec2		amplitude;
		GXFloat		speed;

	public:
		TSGrass ( GXWChar* meshFile, GXWChar* meshCache, GXWChar* materialFile, GXBool twoSided = GX_FALSE );
		virtual ~TSGrass ();

		virtual GXVoid Draw ();
		GXVoid UpdateWind ( GXDword deltaTime );

		GXVoid SetWindSpeed ( GXFloat speed );
		GXVoid SetWindDirection ( const GXVec2 &direction );
		GXVoid SetWindAmplitude ( const GXVec2 &amplitude );

	protected:
		virtual GXVoid InitUniforms ();
};



#endif //TS_GRASS