//version 1.4

#ifndef GX_Q_SKELETAL_MLP_CHARACTER
#define GX_Q_SKELETAL_MLP_CHARACTER


#include <GXEngine/GXSkeletalMeshQuat.h>
#include <GXEngine/GXGlobals.h>


class GXQSkeletalMLPCharacter : public GXSkeletalMeshQuat
{
	protected:
		GXInt			currentFrame;
		GXDword			lasttime;
		GXDword			delayByFrame;
		GXUInt			frameStep;
		GXUInt			currenAnimation;
		GLuint			jointsLocation;
		GLuint			mod_view_proj_mat_Location;

	public:
		virtual ~GXQSkeletalMLPCharacter ();

		virtual GXVoid Draw ( GXVoid* arg );

		virtual GXVoid Pick () = 0;

		virtual GXVoid SetLocation ( GXFloat* Loc );
		virtual GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );

		virtual GXVoid SetRotation ( GXFloat* Rot_rad );
		virtual GXVoid SetRotation ( GXQuat qt );
		virtual GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );

	protected:
		virtual GXVoid InitUniforms ();
		virtual GXVoid Animate () = 0;
		GXVoid InitCharacterContent ();
};


#endif	//GX_Q_SKELETAL_MLP_CHARACTER