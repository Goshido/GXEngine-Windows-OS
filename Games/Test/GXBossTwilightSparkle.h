//version 1.1

#ifndef GX_BOSS_TWILIGHT_SPARKLE_EXT
#define GX_BOSS_TWILIGHT_SPARKLE_EXT


#include <GXEngine/GXPlayerBoss.h>


class GXHitReportTwilightSparkle : public GXHitReportBase
{
	public:
		virtual GXVoid onShapeHit ( const PxControllerShapeHit& hit );
};

class GXBossTwilightSparkle : public GXPlayerBoss
{
	public:
		GXBossTwilightSparkle ();

		virtual GXVoid SetMesh ( GXSkeletalMesh* mesh );
		virtual GXVoid Pick ();
		virtual GXVoid UnPick ();
};


#endif //GX_BOSS_TWILIGHT_SPARKLE_EXT