//version 1.1

#ifndef TS_HUD_PEN
#define TS_HUD_PEN


#include <GXEngine/GXMesh.h>


class TSHud2DPen : public GXMesh
{
	protected:
		GLuint		meshVBO;
		GLuint		meshVAO;
		GXUInt		numVertices;

		GXVec3		points[ 2 ];
		GXVec3		normal;
		GXVec4		color;
		GXUShort	thickness;

		GLuint		lineColorLocation;
		GLuint		mod_view_proj_matLocation;

	public:
		TSHud2DPen ();
		virtual ~TSHud2DPen ();

		virtual GXVoid Draw ( GXVoid* arg );

		GXVoid SetColor ( GXFloat r, GXFloat g, GXFloat b, GXFloat a );
		GXVoid SetThickness ( GXUShort thickness );
		GXVoid SetEnds ( GXVec3 &start, GXVec3 &finish );

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif //TS_HUD_PEN