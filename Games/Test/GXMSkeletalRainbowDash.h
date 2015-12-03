//version 1.0

#ifndef GX_M_SKELETAL_RAINBOW_DASH
#define GX_M_SKELETAL_RAINBOW_DASH


#include "GXMSkeletalMLPCharacter.h"

#define GX_RAINBOW_DASH_TOTAL_ANIM				3
#define GX_RAINBOW_DASH_ANIM_IDLE				0
#define GX_RAINBOW_DASH_ANIM_WALK_FORWARD		1
#define GX_RAINBOW_DASH_ANIM_RUN				2
#define GX_RAINBOW_DASH_ANIM_WALK_BACKWARD		3


class GXMSkeletalRainbowDash : public GXMSkeletalMLPCharacter
{
	protected:
		GXSkeletalMeshAnimationInfo*	animations [ GX_RAINBOW_DASH_TOTAL_ANIM ];

	public:
		GXMSkeletalRainbowDash ();
		virtual ~GXMSkeletalRainbowDash ();
		virtual GXVoid SetAnimation ( GXUShort anim_id );
		virtual GXVoid Pick ();
		virtual GXVoid SetLocation ( GXFloat* Loc );
		virtual GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid Animate ();
};


#endif	//GX_M_SKELETAL_RAINBOW_DASH