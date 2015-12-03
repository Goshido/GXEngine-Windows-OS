//version 1.1

#ifndef TS_AMBIENT_OCCLUSION
#define TS_AMBIENT_OCCLUSION


#include <GXEngine/GXOpenGL.h>
#include <GXEngine/GXVAOStorage.h>
#include <GXCommon/GXMTRStructs.h>
#include <GXCommon/GXMath.h>


class TSAmbientOcclusion
{
	public:
		GLuint			fbo;
		GLuint			texture1;
		GLuint			texture2;
		GLuint			noiseTexture;

		GLint			kernelLocation;
		GLint			noiseScaleLocation;
		GLint			kernelRadiusLocation;
		GLint			maxDistanceLocation;
		GLint			inv_proj_matLocation;
		GLint			proj_matLocation;

		GXVAOInfo		screenQuad;

		GXMaterialInfo	material;
		GXVec3*			kernel;
		GXFloat			kernelRadius;
		GXFloat			maxDistance;
		
	public:
		TSAmbientOcclusion ();
		~TSAmbientOcclusion ();

		GLuint GetAmbientOcclusion ( GLuint depthTexture, GLuint normalTexture );
		GXVoid SetKernelRadius ( GXFloat kernelRadius );
		GXVoid SetMaxDistance ( GXFloat maxDistance );

	private:
		GXVoid InitKernel ();
		GXVoid InitNoiseTexture ();
		GXVoid InitScreenQuad ();
		GXVoid InitFBO ();
		GXVoid InitShaders ();
		GXVoid InitUniforms ();
};


#endif //TS_AMBIENT_OCCLUSION