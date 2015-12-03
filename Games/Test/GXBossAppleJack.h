//version 1.2

#ifndef GX_BOSS_APPLE_JACK
#define GX_BOSS_APPLE_JACK


#include <GXEngine/GXPlayerBoss.h>


class GXHitReportAppleJack : public GXHitReportBase
{
	public:
		virtual GXVoid onShapeHit ( const PxControllerShapeHit& hit );
};

class GXBossAppleJack : public GXPlayerBoss
{
	public:
		GXBossAppleJack ();

		virtual GXVoid SetMesh ( GXSkeletalMesh* mesh );
		virtual GXVoid Pick ();
		virtual GXVoid UnPick ();
};


#endif //GX_BOSS_APPLE_JACK