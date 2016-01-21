//version 1.3

#ifndef	GX_ANIM_SOLVER_PLAYER
#define	GX_ANIM_SOLVER_PLAYER


#include "GXAnimSolver.h"
#include "GXSkeletalMeshStorage.h"


class GXBoneFinderExt;
class GXAnimSolverPlayerExt : public GXAnimSolver
{
	private:
		GXBoneFinderExt*				finder;

		GXFloat							multiplier;
		GXFloat							animPos;
		const GXAnimationInfoExt*		animData;
		GXFloat							delta2PartFartor;

	public:
		GXAnimSolverPlayerExt ( GXUShort solver );
		virtual ~GXAnimSolverPlayerExt ();

		virtual GXVoid GetBone ( const GXUTF8* boneName, const GXQuat** rot, const GXVec3** loc );
		virtual GXVoid Update ( GXFloat delta );

		GXVoid SetAnimationSequence ( const GXAnimationInfoExt* animData );
		GXVoid SetAnimationMultiplier ( GXFloat multiplier );
};


#endif	//GX_ANIM_SOLVER_PLAYER