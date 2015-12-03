//version 1.1

#ifndef GX_BOSS_RAINBOW_DASH
#define GX_BOSS_RAINBOW_DASH


#include <GXEngine/GXPlayerBoss.h>


class GXHitReportRainbowDash : public GXHitReportBase
{
	public:
		virtual GXVoid onShapeHit ( const PxControllerShapeHit& hit );
};

class GXBossRainbowDash : public GXPlayerBoss
{
	public:
		GXBossRainbowDash ();

		virtual GXVoid SetMesh ( GXSkeletalMesh* mesh );
		virtual GXVoid Pick ();
		virtual GXVoid UnPick ();
};


#endif //GX_BOSS_RAINBOW_DASH