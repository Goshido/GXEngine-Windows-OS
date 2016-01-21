//version 1.1

#ifndef GX_SKELETAL_MESH_QUAT_EXT
#define GX_SKELETAL_MESH_QUAT_EXT


#include "GXSkeletalMesh.h"
#include "GXAnimSolver.h"
#include "GXPSKLoader.h"
#include <GXCommon/GXNativeSkeletalMesh.h>
#include <GXEngine/GXSkeletalMeshStorage.h>


#define GX_MAX_BONES_FLOATS			MAX_BONES * 7		//Так как одна кость это GXQuat + GXVec3, то MAX_BONES_FLOATS = MAX_BONES * 7

class GXBoneKeeperExt;
class GXSkeletalMeshQuatExt : public GXMesh
{
	protected:
		GXSkeletalMeshInfoExt		skmInfo;

		GXQuatLocJoint				localPose[ MAX_BONES ];
		GXQuatLocJoint				globalPose[ MAX_BONES ];
		GXQuatLocJoint				vertexTransform[ MAX_BONES ];

		GXAnimSolver*				animSolver;
		GXBoneKeeperExt*			keeper;

	public:
		GXSkeletalMeshQuatExt ();
		virtual ~GXSkeletalMeshQuatExt ();

		virtual	GXVoid GetBoneGlobalLocation ( GXVec3 &out, const GXUTF8* bone );
		virtual	GXVoid GetBoneGlobalPlace ( GXVec3 &loc, GXQuat &rot, const GXUTF8* bone );
		virtual	GXVoid GetAttacmentGlobalLocation ( GXVec3 &out, const GXVec3 &offset, const GXUTF8* bone );
		virtual	GXVoid GetAttacmentGlobalPlace ( GXVec3 &loc, GXQuat &rot, const GXVec3 &offset, const GXUTF8* bone );
		GXVoid SetAnimSolver ( GXAnimSolver* animSolver );

		GXVoid Update ( GXFloat deltaTime );

	protected:
		GXVoid InitReferencePose ();
		GXVoid InitPose ();
};


#endif //GX_SKELETAL_MESH_QUAT_EXT