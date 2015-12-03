//version 1.1

#ifndef GX_SKELETAL_MESH
#define GX_SKELETAL_MESH


#include "GXMesh.h"
#include "GXPSKLoader.h"
#include "GXPSALoader.h"
#include "GXCamera.h"


#define SKIN_INDEX			5
#define SKIN_WEIGHT			6
#define VAO_SKELETON_BONES	0
#define VAO_SKELETON_JOINTS	1
#define MAX_BONES			80


class GXSkeletalMesh : public GXMesh
{
	protected:
		GLuint							VAOskeleton[ 2 ];
		GLuint							VBOskeleton;
		GLuint							VBOxyz;
		GLuint							VBOuv;
		GLuint							VBOskin;
		GLuint							element_array_buffer;
		GLuint							skel_mod_view_proj_mat_Location;
		GXSkeletalMeshInfo				skminfo;
		GXSkeletalMeshAnimationInfo*	skmaniminfo;
		GXBool							bShowSkeleton;
		GXBool							bNeedUpdateSkeleton;
	
	public:
		GXSkeletalMesh ();
		GXVoid ShowSkeleton ( GXBool bShow );

		virtual	GXVoid GetBoneGlobalLocation ( GXVec3 &out, const GXChar* bone ) = 0;
		virtual	GXVoid GetAttacmentGlobalLocation ( GXVec3 &out, const GXVec3 &offset, const GXChar* bone ) = 0;

	protected:
		virtual GXVoid InitReferencePose () = 0;
		virtual GXVoid InitPose ( GXUInt anim_number, GXUInt frame_number ) = 0;
		virtual GXVoid InitDrawableSkeleton () = 0;
		virtual GXVoid DrawSkeleton ( GXCamera* cam ) = 0;
};


#endif //GX_SKELETAL_MESH