//version 1.5

#ifndef GX_Q_SKELETAL_RAINBOW_DASH
#define GX_Q_SKELETAL_RAINBOW_DASH


#include <GXEngine/GXSkeletalMeshQuat.h>
#include <GXEngine/GXGlobals.h>


class GXQSkeletalRainbowDash : public GXSkeletalMeshQuat
{
	protected:
		GLuint			jointsLocation;
		GLuint			mod_view_proj_mat_Location;

		GXDword			deltaTime;

	public:
		GXQSkeletalRainbowDash ();
		virtual ~GXQSkeletalRainbowDash ();

		virtual GXVoid Draw ();

		virtual GXVoid SetLocation ( GXFloat* Loc );
		virtual GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );

		virtual GXVoid SetRotation ( GXFloat* Rot_rad );
		virtual GXVoid SetRotation ( GXQuat qt );
		virtual GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );

		GXVoid Update ( GXDword deltaTime );

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
		GXVoid InitCharacterContent ();
};


#endif	//GX_Q_SKELETAL_RAINBOW_DASH