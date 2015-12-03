//version 1.0

#ifndef TS_CASCADE_SHADOW_MAP
#define TS_CASCADE_SHADOW_MAP


#include <GXEngine/GXMesh.h>
#include <GXEngine/GXOpenGL.h>
#include <GXEngine/GXCameraPerspective.h>


union TSFrustum
{
	struct
	{
		GXVec3	nearA;
		GXVec3	nearB;
		GXVec3	nearC;
		GXVec3	nearD;

		GXVec3	farA;
		GXVec3	farB;
		GXVec3	farC;
		GXVec3	farD;
	};

	GXFloat	v[ 24 ];		//8 vertices with 3 coordinates
};

class TSDirectedLight;
class TSCascadeShadowmap
{
	private:
		GLuint			fbo;
		GLuint			shadowmaps;

		GLuint			mod_view_proj_crop_matLocation;

		GXUChar			numLevels;
		GXUShort		resolution;

		GXFloat*		splits;
		GXMat4*			lightVPC;
		GXMat4*			lightVPCB;
		GXMat4*			totalMVPC;

		GXMat4			lightM;
		GXMat4			lightV;
		GXMat4			lightP;
		GXMat4			lightC;
		GXMat4			lightVP;
		GXMat4			bias;

		TSFrustum		segmentLocal;
		TSFrustum		segmentWorld;
		TSFrustum		segmentLightClip;
		GXAABB			segmentAABB;
		GXVec3			segmentCenter;

		GXMesh**		shadowCasters;
		GXUInt			numShadowCasters;
		GXUInt			maxShadowCasters;

		GXMaterialInfo	matInfo;

		GXFloat			altitude;
		GXFloat			zFarLimit;

	public:
		TSCascadeShadowmap ( GXUShort resolution, GXFloat altitude, GXFloat zFarLimit );
		~TSCascadeShadowmap ();

		GXVoid MakeShadowmaps ( const GXMat4** lightVPCB, const GXFloat** splits, GLuint &csm, TSDirectedLight* light, GXCameraPerspective* view );

		GXVoid IncludeShadowCaster ( GXMesh* caster );
		GXVoid ExcludeShadowCaster ( GXMesh* caster );

	private:
		GXVoid InitFBO ();
		GXVoid InitVisualResources ();
		GXVoid InitShadowCasters ();

		GXVoid UpdateSplits ( TSDirectedLight* light, GXCameraPerspective* view );
		GXVoid UpdateShadows ( TSDirectedLight* light, GXCameraPerspective* view  );
		GXUInt FindShadowCasterIndex ( const GXMesh* caster );

		GXVoid UpdateFrustumSegment ( GXCameraPerspective* view, GXUChar level );
		GXVoid UpdateFrustumSegmentAABB ();
		GXVoid UpdateLightVPC ( TSDirectedLight* light, GXCameraPerspective* view, GXUChar level );

		GXFloat GetBestFarPlaneDistance ();
};


#endif //TS_CASCADE_SHADOW_MAP
