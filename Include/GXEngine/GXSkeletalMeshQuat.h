//version 1.5

#ifndef GX_SKELETAL_MESH_QUAT
#define GX_SKELETAL_MESH_QUAT


#include "GXSkeletalMesh.h"
#include "GXAnimSolver.h"


#define MAX_BONES_FLOATS		MAX_BONES * 7		//Так как одна кость это GXQuat + GXVec3, то MAX_BONES_FLOATS = MAX_BONES * 7
#define MAX_BONE_NAME_LENGTH	64

struct GXQuatLocPose
{
	GXChar				boneName[ MAX_BONES ][ MAX_BONE_NAME_LENGTH ];
	GXQuatLocJoint		local[ MAX_BONES ];
	GXQuatLocJoint		global[ MAX_BONES ];
};

struct GXQuatLocReferencePose
{
	GXChar				boneName[ MAX_BONES ][ MAX_BONE_NAME_LENGTH ];
	GXQuatLocJoint		local[ MAX_BONES ];
	GXQuatLocJoint		bindTransform[ MAX_BONES ];
};


class GXBoneKeeper;
class GXSkeletalMeshQuat : public GXSkeletalMesh
{
	protected:
		GLuint						meshVAO;

		GXQuatLocReferencePose		refPose;						//Reference pose
		GXQuatLocPose				pose;							//Pose
		GXQuatLocJoint				vertexTransform[ MAX_BONES ];

		GXAnimSolver*				animSolver;
		GXBoneKeeper*				keeper;
	
	public:
		GXSkeletalMeshQuat ();
		virtual ~GXSkeletalMeshQuat ();

		virtual	GXVoid GetBoneGlobalLocation ( GXVec3 &out, const GXChar* bone );
		virtual	GXVoid GetBoneGlobalPlace ( GXVec3 &loc, GXQuat &rot, const GXChar* bone );
		virtual	GXVoid GetAttacmentGlobalLocation ( GXVec3 &out, const GXVec3 &offset, const GXChar* bone );
		virtual	GXVoid GetAttacmentGlobalPlace ( GXVec3 &loc, GXQuat &rot, const GXVec3 &offset, const GXChar* bone );
		GXVoid SetAnimSolver ( GXAnimSolver* animSolver );

	protected:
		virtual GXVoid InitUniforms ();
		virtual GXVoid InitReferencePose ();
		virtual GXVoid InitPose ( GXUInt anim_number, GXUInt frame_number );
		virtual GXVoid InitPose ();
		virtual GXVoid InitDrawableSkeleton ();
		virtual GXVoid DrawSkeleton ( GXCamera* cam );
};


#endif //GX_SKELETAL_MESH_QUAT