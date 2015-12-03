//versoin 1.3

#ifndef TS_SHADOWMAP
#define TS_SHADOWMAP


#include <GXEngine/GXMesh.h>
#include <GXEngine/GXOpenGL.h>
#include <GXEngine/GXShaderStorage.h>


class TSSpotlight;
class TSShadowmap
{
	private:
		GLuint			fbo;
		GLuint			shadowmap;
		
		GXMesh**		shadowCasters;
		GXUInt			numShadowCasters;
		GXUInt			maxShadowCasters;

		GXUShort		width;
		GXUShort		height;

		GXMaterialInfo	matInfo;

		GLuint			mod_view_proj_matLocation;

	public:
		TSShadowmap ();
		~TSShadowmap ();

		GXVoid ResetShadowmap ();

		GXVoid StartShadowmapPass ();
		GXVoid FinishShadowmapPass ();

		GLuint GetShadowmapFrom ( TSSpotlight* spot );

		GXVoid IncludeShadowCaster ( GXMesh* caster );
		GXVoid ExcludeShadowCaster ( GXMesh* caster );

	private:
		GXVoid LoadShaders ();
		GXUInt FindShadowCasterIndex ( const GXMesh* caster );
};

#endif //TS_SHADOWMAP