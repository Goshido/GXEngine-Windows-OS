//version 1.2

#ifndef TS_MARKER
#define TS_MARKER


#include <GXEngine/GXMesh.h>


class TSMarker : public GXMesh
{
	protected:
		GLuint			mod_view_proj_matLocation;
		GLuint			normal_matLocation;

		GXVec4			diffuse;
		GXVec3			emit;
		GXVec3			specColor;
		GXVec3			specPowMetalIntens;

		GLuint			diffuseLocation;
		GLuint			emitLocation;
		GLuint			specColorLocation;
		GLuint			specPowMetalIntensLocation;

	public:
		TSMarker ();
		virtual ~TSMarker ();

		virtual GXVoid Draw ();

		GXVoid SetDiffuseColor ( GXFloat r, GXFloat g, GXFloat b, GXFloat a );
		GXVoid SetEmitColor ( GXFloat r, GXFloat g, GXFloat b );

		GXVoid SetSpecColor ( GXFloat r, GXFloat g, GXFloat b );
		GXVoid SetSpecPower ( GXUChar specFactor );
		GXVoid SetMetallicFactor ( GXFloat metallic );
		GXVoid SetSpecIntensityFactor ( GXFloat intensity );

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();

};


#endif //TS_MARKER