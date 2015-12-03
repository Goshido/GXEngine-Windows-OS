//version 1.0

#ifndef GX_M_SKELETAL_APPLE_JACK
#define GX_M_SKELETAL_APPLE_JACK


#include "GXMSkeletalMLPCharacter.h"


#define GX_APPLE_JACK_TOTAL_ANIM				3
#define GX_APPLE_JACK_ANIM_IDLE					0
#define GX_APPLE_JACK_ANIM_WALK_FORWARD			1
#define GX_APPLE_JACK_ANIM_RUN					2
#define GX_APPLE_JACK_ANIM_WALK_BACKWARD		3


class GXMSkeletalAppleJack : public GXMSkeletalMLPCharacter
{
	protected:
		GXSkeletalMeshAnimationInfo* animations [ GX_APPLE_JACK_TOTAL_ANIM ];

	public:
		GXMSkeletalAppleJack ();
		virtual ~GXMSkeletalAppleJack ();
		virtual GXVoid SetAnimation ( GXUShort anim_id );
		virtual GXVoid Pick ();
		virtual GXVoid SetLocation ( GXFloat* Loc );
		virtual GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid Animate ();
};


#endif	//GX_M_SKELETAL_APPLE_JACK