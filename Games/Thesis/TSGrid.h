//version 1.2

#ifndef TS_GRID
#define TS_GRID


#include <GXEngine/GXMesh.h>


class TSGrid : public GXMesh
{
	protected:
		GLuint		meshVAO;
		GLuint		meshVBO;
		GXUInt		numVertices;

		GLuint		lineColorLocation;
		GLuint		mod_view_proj_matLocation;

		GXUInt		cellFactor;

		GXVec4		gridColor;

	public:
		TSGrid ( GXUInt cellFactor );
		virtual ~TSGrid () ;

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();	
};


#endif //TS_GRID