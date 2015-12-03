//version 1.1

#ifndef GX_SKELETAL_MESH_MAT
#define GX_SKELETAL_MESH_MAT


#include "GXSkeletalMesh.h"

struct GXGXMatPose					// L - локальное, G -глобальное
{
	GXChar	Name [ MAX_BONES ][ 64 ];
	GXMat4	L [ MAX_BONES ];
	GXMat4	G [ MAX_BONES ];
};

class GXSkeletalMeshMat : public GXSkeletalMesh
{
	protected:
		GLuint		meshVAO;

		GXGXMatPose	RP;					//Reference pose
		GXGXMatPose	P;					//Pose
		GXMat4		BT [ MAX_BONES ];
		GXMat4		VT [ MAX_BONES ];
	
	public:
		virtual ~GXSkeletalMeshMat ();

		virtual	GXVoid GetBoneGlobalLocation ( GXVec3 &out, const GXChar* bone );
		virtual	GXVoid GetAttacmentGlobalLocation ( GXVec3 &out, const GXVec3 &offset, const GXChar* bone );

	protected:
		virtual GXVoid InitUniforms ();
		virtual GXVoid InitReferencePose ();
		virtual GXVoid InitPose ( GXUInt anim_number, GXUInt frame_number );
		virtual GXVoid InitDrawableSkeleton ();
		virtual GXVoid DrawSkeleton ( GXCamera* cam );
};


#endif //GX_SKELETAL_MESH_MAT