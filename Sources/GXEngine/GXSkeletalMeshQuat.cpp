//version 1.4

#include <GXEngine/GXSkeletalMeshQuat.h>
#include <GXEngine/GXAVLTree.h>
#include <new>


class GXBoneKeeperNode : public GXAVLTreeNode
{
	public:
		const GXChar*	boneName;
		GXUShort		boneIndex;

	public:
		GXBoneKeeperNode ( const GXChar* boneName, GXUShort boneIndex );
	
		virtual const GXVoid* GetKey ();

		static GXInt GXCALL Compare ( const GXVoid* a, const GXVoid* b );
};

GXBoneKeeperNode::GXBoneKeeperNode ( const GXChar* boneName, GXUShort boneIndex )
{
	this->boneName = boneName;
	this->boneIndex = boneIndex;
}

const GXVoid* GXBoneKeeperNode::GetKey ()
{
	return boneName;
}

GXInt GXCALL GXBoneKeeperNode::Compare ( const GXVoid* a, const GXVoid* b )
{
	return strcmp ( (const GXChar*)a, (const GXChar*)b );
}

//--------------------------------------------------------------------------------------------------

class GXBoneKeeper : public GXAVLTree
{
	private:
		GXBoneKeeperNode*		cacheFriendlyNodes;

	public:
		GXBoneKeeper ( const GXQuatLocReferencePose &refPose, GXUInt numBones );
		virtual ~GXBoneKeeper ();

		GXUShort FindBoneIndex ( const GXChar* boneName );
};

GXBoneKeeper::GXBoneKeeper ( const GXQuatLocReferencePose &refPose, GXUInt numBones ) :
GXAVLTree ( &GXBoneKeeperNode::Compare, GX_FALSE )
{
	cacheFriendlyNodes = (GXBoneKeeperNode*)malloc ( sizeof ( GXBoneKeeperNode ) * numBones );
	for ( GXUShort i = 0; i < numBones; i++ )
	{
		new ( cacheFriendlyNodes + i ) GXBoneKeeperNode ( refPose.boneName[ i ], i );
		Add ( cacheFriendlyNodes + i );
	}
}

GXBoneKeeper::~GXBoneKeeper ()
{
	free ( cacheFriendlyNodes );
}

GXUShort GXBoneKeeper::FindBoneIndex ( const GXChar* boneName )
{
	const GXBoneKeeperNode* node = (const GXBoneKeeperNode*)FindByKey ( boneName );

	if ( !node )
		return 0xFFFF;

	return node->boneIndex;
}

//-------------------------------------------------------------------------------------------------

GXSkeletalMeshQuat::GXSkeletalMeshQuat ()
{
	keeper = 0;
	animSolver = 0;
}

GXSkeletalMeshQuat::~GXSkeletalMeshQuat ()
{
	GXSafeDelete ( keeper );
	GXRemoveShaderProgram ( mat_info.shaders[ 1 ] );
}

GXVoid GXSkeletalMeshQuat::GetBoneGlobalLocation ( GXVec3 &out, const GXChar* bone )
{
	GXUShort i = keeper->FindBoneIndex ( bone );

	GXVec4 ans;
	GXVec4 buf ( pose.global[ i ].location, 1.0f );

	GXMulVec4Mat4 ( ans, buf, mod_mat );

	out = GXCreateVec3 ( ans.x, ans.y, ans.z );
}

GXVoid GXSkeletalMeshQuat::GetBoneGlobalPlace ( GXVec3 &loc, GXQuat &rot, const GXChar* bone )
{
	GXUShort i = keeper->FindBoneIndex ( bone );

	GXVec4 ans;
	GXVec4 buf ( pose.global[ i ].location, 1.0f );
	GXMulVec4Mat4 ( ans, buf, mod_mat );
	loc = GXCreateVec3 ( ans.x, ans.y, ans.z );

	GXQuat bufRot = GXCreateQuat ( mod_mat );
	GXMulQuatQuat ( rot, pose.global[ i ].rotation, bufRot );
}

GXVoid GXSkeletalMeshQuat::GetAttacmentGlobalLocation ( GXVec3 &out, const GXVec3 &offset, const GXChar* bone )
{
	GXVec3 ofst = GXCreateVec3 ( offset.y, offset.x, offset.z );
	GXUShort i = keeper->FindBoneIndex ( bone );

	GXVec3 buf;
	GXVec3 buf01;

	GXQuatTransform ( buf01, pose.global[ i ].rotation, ofst );

	GXSumVec3Vec3 ( buf, pose.global[ i ].location, buf01 );

	GXVec4 ans;
	GXVec4 buf02 ( buf, 1.0f );
	GXMulVec4Mat4 ( ans, buf02, mod_mat );

	out = GXCreateVec3 ( ans.x, ans.y, ans.z );
}

GXVoid GXSkeletalMeshQuat::GetAttacmentGlobalPlace ( GXVec3 &loc, GXQuat &rot, const GXVec3 &offset, const GXChar* bone )
{
	GXVec3 ofst = GXCreateVec3 ( offset.y, offset.x, offset.z );
	GXUShort i = keeper->FindBoneIndex ( bone );

	GXVec3 buf;
	GXVec3 buf01;

	GXQuatTransform ( buf01, pose.global[ i ].rotation, ofst );

	GXSumVec3Vec3 ( buf, pose.global[ i ].location, buf01 );

	GXVec4 ans;
	GXVec4 buf02 ( buf, 1.0f );
	GXMulVec4Mat4 ( ans, buf02, mod_mat );

	loc = GXCreateVec3 ( ans.x, ans.y, ans.z );

	GXQuat bufRot = GXCreateQuat ( mod_mat );
	GXMulQuatQuat ( rot, pose.global[ i ].rotation, bufRot );
}

GXVoid GXSkeletalMeshQuat::SetAnimSolver ( GXAnimSolver* animSolver )
{
	this->animSolver = animSolver;
}

GXVoid GXSkeletalMeshQuat::InitUniforms ()
{
	GXGetShaderProgram ( mat_info.shaders[ 1 ] );
	glUseProgram ( mat_info.shaders[ 1 ].uiId );	
	skel_mod_view_proj_mat_Location = GXGetUniformLocation ( mat_info.shaders[ 1 ].uiId, "mod_view_proj_mat" );
	glUseProgram ( 0 );
}

GXVoid GXSkeletalMeshQuat::DrawSkeleton ( GXCamera* cam )
{
	static GXBool fl = GX_FALSE;
	if ( bNeedUpdateSkeleton )
	{
		glBindBuffer ( GL_ARRAY_BUFFER, VBOskeleton );
		GXVec3 geom [ 70 * 2 ];
		for ( GXUChar i = 0 ; i < skminfo.totalBones; i++ )
		{
			memcpy ( geom + i * 2, pose.global[ i ].location.v, sizeof ( GXVec3 ) );
			memcpy ( geom + i * 2 + 1, pose.global[ skminfo.Bones[ i ].ParentIndex ].location.v, sizeof ( GXVec3 ) );
		}
		glBufferData ( GL_ARRAY_BUFFER, 2 * skminfo.totalBones * sizeof ( GXVec3 ), geom, GL_STATIC_DRAW );
		bNeedUpdateSkeleton = GX_FALSE;
		fl = GX_TRUE;
	}
	
	GXMat4 view_proj_mat;
	view_proj_mat = cam->GetViewProjectionMatrix ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, view_proj_mat );

	glUseProgram ( mat_info.shaders [ 1 ].uiId );	
	glUniformMatrix4fv ( skel_mod_view_proj_mat_Location, 1, GL_FALSE, mod_view_proj_mat.A );
	glDisable ( GL_DEPTH_TEST );
	glBindVertexArray ( VAOskeleton [ VAO_SKELETON_BONES ] );
	glDrawArrays ( GL_LINES, 0, skminfo.totalBones * 3 * 2 ); 
	glBindVertexArray ( VAOskeleton [ VAO_SKELETON_JOINTS ] );
	glDrawArrays ( GL_POINTS, 0, skminfo.totalBones * 3 ); 
	glBindVertexArray ( 0 );
	glUseProgram ( 0 );
	glEnable ( GL_DEPTH_TEST );
};

GXVoid GXSkeletalMeshQuat::InitDrawableSkeleton ()
{
	GXGetShaderProgram ( mat_info.shaders[ 1 ] );
	InitUniforms ();
	glGenVertexArrays ( 2, VAOskeleton );
	glBindVertexArray ( VAOskeleton[ VAO_SKELETON_BONES ] );
	//{
		glGenBuffers ( 1, &VBOskeleton );
		glBindBuffer ( GL_ARRAY_BUFFER, VBOskeleton );
		//{
			glVertexAttribPointer ( GX_VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, sizeof ( GXVec3 ), ( const GLvoid* ) 0 );
			glEnableVertexAttribArray ( GX_VERTEX_INDEX );
		//}
	//}
	glBindVertexArray ( VAOskeleton [ VAO_SKELETON_JOINTS ] );
	//{
		glVertexAttribPointer ( GX_VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, 2 * sizeof ( GXVec3 ), ( const GLvoid* ) 0 );
		glEnableVertexAttribArray ( GX_VERTEX_INDEX );
	//}
	glBindVertexArray ( 0 );
	glPointSize ( 3.0f );
	bNeedUpdateSkeleton = GX_TRUE;
}

GXVoid GXSkeletalMeshQuat::InitReferencePose ()
{
	for ( GXUInt i = 0; i < skminfo.totalBones; i++ )
	{
		memcpy ( refPose.boneName[ i ], skminfo.Bones[ i ].Name, 64 ); 
		refPose.local[ i ].rotation = skminfo.Bones[ i ].BonePos.Orientation;
		refPose.local[ i ].location = skminfo.Bones[ i ].BonePos.Position;
	}

	GXQuatLocJoint* global = (GXQuatLocJoint*)malloc ( skminfo.totalBones * sizeof ( GXQuatLocJoint ) );

	//Расчёт итоговой трансформации корневой кости. Корневая кость - особый случай (у неё нет родителя).

	global[ 0 ].rotation = skminfo.Bones[ 0 ].BonePos.Orientation;
	global[ 0 ].location = skminfo.Bones[ 0 ].BonePos.Position;

	GXInverseQuat ( refPose.bindTransform[ 0 ].rotation, global[ 0 ].rotation );
	refPose.bindTransform[ 0 ].location.x = -global[ 0 ].location.x;
	refPose.bindTransform[ 0 ].location.y = -global[ 0 ].location.y;
	refPose.bindTransform[ 0 ].location.z = -global[ 0 ].location.z;

	//Расчёт всех оставшихся костей.

	for ( GXUInt i = 1; i < skminfo.totalBones; i++ )
	{
		GXMulQuatQuat ( global[ i ].rotation, refPose.local[ i ].rotation, global[ skminfo.Bones[ i ].ParentIndex ].rotation );
		GXVec3 vTemp;
		GXQuatTransform ( vTemp, global[ skminfo.Bones[ i ].ParentIndex ].rotation, refPose.local[ i ].location );
		GXSumVec3Vec3 ( global[ i ].location, vTemp, global[ skminfo.Bones[ i ].ParentIndex ].location );

		GXInverseQuat ( refPose.bindTransform[ i ].rotation, global[ i ].rotation );
		refPose.bindTransform[ i ].location.x = -global[ i ].location.x;
		refPose.bindTransform[ i ].location.y = -global[ i ].location.y;
		refPose.bindTransform[ i ].location.z = -global[ i ].location.z;
	}

	free ( global );

	GXSafeDelete ( keeper );
	keeper = new GXBoneKeeper ( refPose, skminfo.totalBones );
}

GXVoid GXSkeletalMeshQuat::InitPose ( GXUInt anim_number, GXUInt frame_number )
{
	//NOTHING
}

GXVoid GXSkeletalMeshQuat::InitPose ()
{
	memcpy ( pose.local, refPose.local, sizeof ( GXQuatLocJoint ) * skminfo.totalBones );
	memcpy ( pose.boneName, refPose.boneName, sizeof ( GXChar ) * MAX_BONE_NAME_LENGTH * skminfo.totalBones );
	for ( GXUInt i = 0; i < skminfo.totalBones; i++ )
	{
		const GXQuat* rot = 0;
		const GXVec3* loc = 0;

		animSolver->GetBone ( refPose.boneName[ i ], &rot, &loc );

		if ( rot )
		{
			pose.local[ i ].rotation = *rot;
			pose.local[ i ].location = *loc;
		}
	}

	//Расчёт итоговой трансформации корневой кости. Корневая кость - особый случай (у неё нет родителя).

	pose.global[ 0 ].rotation = pose.local[ 0 ].rotation;
	pose.global[ 0 ].location = pose.local[ 0 ].location;

	GXMulQuatQuat ( vertexTransform[ 0 ].rotation, refPose.bindTransform[ 0 ].rotation, pose.global[ 0 ].rotation );
	GXVec3 vTemp2;
	GXQuatTransform ( vTemp2, vertexTransform[ 0 ].rotation, refPose.bindTransform[ 0 ].location );
	GXSumVec3Vec3 ( vertexTransform[ 0 ].location, vTemp2, pose.global[ 0 ].location );

	//Расчёт всех оставшихся костей.

	for ( GXUInt i = 1; i < skminfo.totalBones; i++ )
	{
		GXMulQuatQuat ( pose.global[ i ].rotation, pose.local[ i ].rotation, pose.global[ skminfo.Bones[ i ].ParentIndex ].rotation );
		GXVec3 vTemp;
		GXQuatTransform ( vTemp, pose.global[ skminfo.Bones[ i ].ParentIndex ].rotation, pose.local[ i ].location );
		GXSumVec3Vec3 ( pose.global[ i ].location, vTemp, pose.global[ skminfo.Bones[ i ].ParentIndex ].location );

		GXMulQuatQuat ( vertexTransform[ i ].rotation, refPose.bindTransform[ i ].rotation, pose.global[ i ].rotation );
		GXVec3 vTemp2;
		GXQuatTransform ( vTemp2, vertexTransform[ i ].rotation, refPose.bindTransform[ i ].location );
		GXSumVec3Vec3 ( vertexTransform[ i ].location, vTemp2, pose.global[ i ].location );
	}
}