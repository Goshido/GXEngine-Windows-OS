//version 1.7

#ifndef TS_LIGHT_EMITTER
#define TS_LIGHT_EMITTER


#include "TSShadowmap.h"
#include "TSCascadeShadowmap.h"


extern TSShadowmap*			ts_light_Shadowmap;
extern TSCascadeShadowmap*	ts_light_CascadeShadowmap;


enum eTSLightEmitterType
{
	TS_BULP,
	TS_DIRECTED,
	TS_SPOT,
	TS_UNKNOWN
};

class TSLightEmitter
{
	protected:
		eTSLightEmitterType type;
		GXVec3				color;			// [ 0.0f, 1.0f ]
		GXFloat				intensity;		// ( 0.0f, +inf ]

		GXMat4				mod_mat;

		GXVAOInfo			vaoInfo;

		GXMaterialInfo		mat_info;
		GLuint				mod_view_proj_matLocation;

	public:
		TSLightEmitter ();
		virtual ~TSLightEmitter ();

		eTSLightEmitterType GetType ();
		GXVoid SetColor ( GXFloat r, GXFloat g, GXFloat b );
		GXVoid SetIntensity ( GXFloat intensity );

		GXVoid GetColor ( GXVec3 &color );
		GXFloat GetIntensity ();

		GXVoid GetHDRColor ( GXVec3 &hdrColor );
		GXVoid GetHDRColorAndIntensity ( GXVec4 &hdrColorAndIntensity );

		virtual GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		virtual GXVoid SetLocation ( const GXVec3 &loc );

		virtual GXVoid SetRotation ( const GXVec3 &rot_rad );
		virtual GXVoid SetRotation ( const GXMat4 &rot );
		virtual GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );
		virtual GXVoid SetRotation ( const GXQuat &q );

		virtual GXVoid GetLocation ( GXVec3 &loc );

		virtual GXVoid GetRotation ( GXMat4 &rot );
		virtual GXVoid GetRotation ( GXQuat &rot );

		virtual const GXMat4& GetModelMatrix ();
		virtual GXVoid GetLightVolume ( GLuint &vao, GLenum &topology, GXInt &numVertexes );

		virtual GXVoid DrawLightVolumeGismo ();

	protected:
		virtual GXVoid LoadLightVolume () = 0;
};

class TSBulp : public TSLightEmitter
{
	private:
		GXFloat		distance;

	public:
		TSBulp ();
		virtual ~TSBulp ();

		GXFloat GetInfluenceDistance ();
		GXVoid SetInfluenceDistance ( GXFloat distance );

		virtual GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		virtual GXVoid SetLocation ( const GXVec3 &loc );

		virtual GXVoid SetRotation ( const GXVec3 &rot_rad );
		virtual GXVoid SetRotation ( const GXMat4 &rot );
		virtual GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );
		virtual GXVoid SetRotation ( const GXQuat &q );

		virtual GXVoid GetLocation ( GXVec3 &loc );

		virtual GXVoid GetRotation ( GXMat4 &rot );
		virtual GXVoid GetRotation ( GXQuat &rot );

	protected:
		virtual GXVoid LoadLightVolume ();
};

class TSSpotlight : public TSLightEmitter
{
	private:
		GXFloat		distance;
		GXFloat		coneAngle;

		GXBool		isCastShadows;
		GXFloat		compressionXY;

		GXMat4		rot_mat;
		GXMat4		twicked_mod_mat;
		GXMat4		view_proj_mat;

	public:
		TSSpotlight ( GXBool castShadows );
		virtual ~TSSpotlight ();

		GXFloat GetInfluenceDistance ();
		GXFloat GetConeAngle ();

		GXVoid SetInfluenceDistance ( GXFloat distance );
		GXVoid SetConeAngle ( GXFloat angle_rad );

		virtual GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		virtual GXVoid SetLocation ( const GXVec3 &loc );

		virtual GXVoid SetRotation ( const GXVec3 &rot_rad );
		virtual GXVoid SetRotation ( const GXMat4 &rot );
		virtual GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );
		virtual GXVoid SetRotation ( const GXQuat &q );

		virtual GXVoid GetLocation ( GXVec3 &loc );

		virtual const GXMat4& GetModelMatrix ();

		const GXMat4& GetViewProjectionMatrix ();

		GXBool IsCastedShadows ();

		virtual GXVoid DrawLightVolumeGismo ();

	protected:
		virtual GXVoid LoadLightVolume ();
		GXVoid UpdateViewProjectionMatrix ();
};

class TSDirectedLight : public TSLightEmitter
{
	private:
		GXVec3	direction;

		GXVec3	ambient;		//[ 0.0f, 1.0f ]
		GXFloat	ambientIntens;	//[ 0.0f, +inf )

		GXBool	isCastShadows;

	public:
		TSDirectedLight ( GXBool castShadows );
		virtual ~TSDirectedLight ();

		virtual GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );
		virtual GXVoid SetLocation ( const GXVec3 &loc );

		virtual GXVoid GetLocation ( GXFloat &x, GXFloat &y, GXFloat &z );
		virtual GXVoid GetLocation ( GXVec3 &loc );

		virtual const GXMat4& GetModelMatrix ();
		virtual GXVoid GetLightVolume ( GLuint &vao, GLenum &topology, GXInt &numVertexes );

		GXBool IsCastedShadows ();

		virtual GXVoid DrawLightVolumeGismo ();

		GXVoid SetAmbientColor ( GXFloat r, GXFloat g, GXFloat b );
		GXVoid SetAmbientIntensity ( GXFloat intens );

		GXVoid GetHDRAmbientColorAndIntensity ( GXVec4 &hdrAmbientColorAndIntensity );

	protected:
		virtual GXVoid LoadLightVolume ();
};

#endif //TS_LIGHT_EMITTER