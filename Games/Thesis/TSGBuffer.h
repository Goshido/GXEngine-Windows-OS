//version 1.8

#ifndef TS_GBUFFER
#define TS_GBUFFER


#include <GXEngine/GXTextureOuter.h>
#include <GXEngine/GXCamera.h>
#include "TSLight.h"
#include "TSBloomEffect.h"
#include "TSAmbientOcclusion.h"
#include "TSShadowmap.h"


enum eTSGBufferSlot
{
	TS_DIFFUSE,
	TS_NORMALS,
	TS_DEPTH,
	TS_LIGHTUP,
	TS_EMISSION,
	TS_SPEC_COLOR,
	TS_SPEC_PARAMS,
	TS_OUT
};

enum eTSGBufferPass
{
	TS_SKYBOX,
	TS_COMMON,
	TS_ALBEDO,
	TS_HUD_DEPTH_DEPENDENT,
	TS_HUD_DEPTH_INDEPENDENT,
	TS_COMPOSE,
	TS_BLOOM
};

struct TSBulpUniforms
{
	GLint		viewBulpPosition;
	GLint		invQuadMaxDistance;
	GLint		inv_proj_mat;
	GLint		hdrColorAndIntensity;
	GLint		mod_view_proj_mat;
	GLint		invScreenResolution;
};

struct TSSpotUniforms
{
	GLint		viewSpotLampPosition;
	GLint		viewSpotLampDirection;
	GLint		invQuadMaxDistance;
	GLint		cutoffCosine;
	GLint		inv_proj_mat;
	GLint		hdrColorAndIntensity;	//rgb - HDR Color, a - intensity
	GLint		mod_view_proj_mat;
	GLint		invScreenResolution;
};

struct TSSpotShadowUniforms
{
	GLint		viewSpotLampPosition;
	GLint		viewSpotLampDirection;
	GLint		invQuadMaxDistance;
	GLint		cutoffCosine;
	GLint		inv_proj_mat;
	GLint		hdrColorAndIntensity;	//rgb - HDR Color, a - intensity
	GLint		inv_view_mat;
	GLint		lamp_view_proj_mat;
	GLint		mod_view_proj_mat;
	GLint		invScreenResolution;
};

struct TSDirectedUniforms
{
	GLint		viewLightDirection;
	GLint		inv_proj_mat;
	GLint		hdrColorAndIntensity;			//rgb - HDR Color, a - intensity
	GLint		hdrAmbientColorAndIntensity;	//rgb - HDR Color, a - intensity
};

struct TSDirectedShadowUniforms
{
	GLint	viewLightDirection;
	GLint	viewFrustumSplits;
	GLint	inv_proj_mat;
	GLint	inv_view_mat;
	GLint	light_view_proj_crop_bias_mat;
	GLint	hdrColorAndIntensity;
	GLint	hdrAmbientColorAndIntensity;
};

class TSGBuffer
{
	private:
		GLuint						fbo;

		GLuint						diffuseTexture;				//диффузия материала - RGBA8
		GLuint						normalTexture;				//нормали в видовом пространстве - RGB16
		GLuint						emitTexture;				//эмиссия материала - RGB16F
		GLuint						specColorTexture;			//цвет блика матариала - RGB8	
		GLuint						specPowMetalIntensTexture;	//R степень блика и G интенсивность блика - RG8
		GLuint						objectTexture;				//ID (адрес) объекта - UINT
		GLuint						depthTexture;				//глубина со стенсилем - UINT24_8
		GLuint						lightUpTexture;				//RGB альбедо и A номинальная интенсивность блика - RGBA16F
		GLuint						hdrOutTexture;				//выходное изображение в HDR - RGB16F
		GLuint						outTexture;					//выходное изображение - RGBA8

		TSAmbientOcclusion*			ambientOcclusion;

		GLuint						resampleFBO;
		GLuint						resampleTexture[ 2 ];
		GXUShort					maxMipmapLevel;
		GXUShort					resampleX;
		GXUShort					resampleY;
		GXUChar						activeResampleTexture;

		TSBloomEffect*				bloom;

		GXBool						needUpdateLum;
		GXBool						exposureInit;
		GXFloat						exposureDelta;
		GXFloat						exposureTimer;

		GXMat4						observer_inv_proj_mat;
		const GXMat4*				observer_view_mat;
		GXCamera*					observerCamera;

		GLuint						tone_lerpFactorLocation;
		GLuint						light_vol_mod_view_proj_matLocation;

		TSBulpUniforms				bulpUniforms;
		TSSpotUniforms				spotUniforms;
		TSSpotShadowUniforms		spotShadowUniforms;
		TSDirectedUniforms			directedUniforms;
		TSDirectedShadowUniforms	directedShadowUniforms;

		GXTextureOuter				textureOuter;

		GXVAOInfo					screenQuadVAO;
		GXVec2						invScreenResolution;

		eTSGBufferPass				currentPass;
		GXMaterialInfo				matInfo;

	public:
		TSGBuffer ( GXCamera* observerCamera );
		~TSGBuffer ();

		GLuint GetFrameBuffer ();

		GLuint GetColorTexture ();
		GLuint GetDepthTexture ();
		GLuint GetObjectTexture ();

		GXVoid SetObserverCamera ( GXCamera* observerCamera );

		GXVoid ShowTexture ( eTSGBufferSlot slot );

		GXVoid ClearGBuffer ();

		GXVoid StartSkyboxPass ();
		GXVoid StartCommonPass ();
		GXVoid StartLightUpPass ();
		GXVoid StartHudDepthDependentPass ();
		GXVoid StartHudDepthIndependentPass ();

		GXVoid LightBy ( TSLightEmitter* emitter );

		GXVoid CombineSlots ();
		GXVoid AddBloom ();
		GXVoid UpdateExposure ( GXFloat delta );

	private:
		GXVoid InitFBOs ();
		GXVoid InitShaders ();

		GXVoid LightByBulp ( TSBulp* bulp );
		GXVoid LightBySpotlight ( TSSpotlight* spot );
		GXVoid LightByDirectedLight ( TSDirectedLight* light );

		GXVoid MarkLightVolume ( TSLightEmitter* emitter );

		GXVoid DoSpotWithShadows ( TSSpotlight* spot );
		GXVoid DoSpotWithoutShadows ( TSSpotlight* spot );

		GXVoid DoDirectedLightWithShadows ( TSDirectedLight* light );
		GXVoid DoDirectedLightWithoutShadows ( TSDirectedLight* light );
};


#endif //TS_GBUFFER