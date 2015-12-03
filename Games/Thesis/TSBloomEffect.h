//version 1.3

#ifndef TS_BLOOM_EFFECT
#define TS_BLOOM_EFFECT


#include <GXEngine/GXTextureUtils.h>
#include <GXEngine/GXMesh.h>


class TSBloomEffect : public GXMesh
{
	private:
		GLuint		fbo;

		GLuint		blurXTexture;
		GLuint		blurYTexture;

		GXUShort	resolutionX;	
		GXUShort	resolutionY;
		GXVec2		invResolution;
		
		GLuint		hdrTexture;
		GLuint		averageLumTexture;

		GLuint		blurXLocation;
		GLuint		blurYLocation;	
		GLuint		stepXYLocation;

	public:
		TSBloomEffect ( GXUShort width, GXUShort height, GLuint hdrTexture );
		virtual ~TSBloomEffect ();

		virtual GXVoid Draw ();

		GXVoid SetAverageTexture ( GLuint texture );
		GLuint GetBloomTexture ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};

#endif //TS_BLOOM_EFFECT