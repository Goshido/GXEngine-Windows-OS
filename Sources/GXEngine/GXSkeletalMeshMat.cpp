//version 1.1

#include <GXEngine/GXSkeletalMeshMat.h>


GXSkeletalMeshMat::~GXSkeletalMeshMat ()
{
	GXRemoveShaderProgram ( mat_info.shaders [ 1 ] );
}

GXVoid GXSkeletalMeshMat::GetBoneGlobalLocation ( GXVec3 &out, const GXChar* bone )
{
	GXUShort i = 0;
	for ( ; i < MAX_BONES && strcmp ( RP.Name[ i ], bone ); i++ );

	GXVec4 ans;
	GXVec4 buf ( P.G[ i ].m41, P.G[ i ].m42, P.G[ i ].m43, 1.0f );

	GXMulVec4Mat4 ( ans, buf, mod_mat );

	out = GXCreateVec3 ( ans.x, ans.y, ans.z );
}

GXVoid GXSkeletalMeshMat::GetAttacmentGlobalLocation ( GXVec3 &out, const GXVec3 &offset, const GXChar* bone )
{
	//TODO

	GXUShort i = 0;
	for ( ; i < MAX_BONES && strcmp ( RP.Name[ i ], bone ); i++ );

	GXVec4 ans;
	GXVec4 buf ( P.G[ i ].m41 + offset.x, P.G[ i ].m42 + offset.y, P.G[ i ].m43 + offset.z, 1.0f );

	GXMulVec4Mat4 ( ans, buf, mod_mat );

	out = GXCreateVec3 ( ans.x, ans.y, ans.z );
}

GXVoid GXSkeletalMeshMat::InitUniforms ()
{
	GXGetShaderProgram ( mat_info.shaders [ 1 ] );
	glUseProgram ( mat_info.shaders [ 1 ].uiId );	
	skel_mod_view_proj_mat_Location = GXGetUniformLocation ( mat_info.shaders [ 1 ].uiId, "mod_view_proj_mat" );
	glUseProgram ( 0 );
}

GXVoid GXSkeletalMeshMat::DrawSkeleton ( GXCamera* cam )
{
	static GXBool fl = GX_FALSE;
	if ( bNeedUpdateSkeleton )
	{
		glBindBuffer ( GL_ARRAY_BUFFER, VBOskeleton );
		GXVec3 geom [ 70 * 2 ];
		for ( GXUChar i = 0 ; i < skminfo.totalBones; i++ )
		{
			memcpy ( geom + i * 2, P.G [ i ].A + 12, sizeof ( GXVec3 ) );
			memcpy ( geom + i * 2 + 1, P.G [ skminfo.Bones [ i ].ParentIndex ].A + 12, sizeof ( GXVec3 ) );
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

GXVoid GXSkeletalMeshMat::InitDrawableSkeleton ()
{
	GXGetShaderProgram ( mat_info.shaders [ 1 ] );
	InitUniforms ();
	glGenVertexArrays ( 2, VAOskeleton );
	glBindVertexArray ( VAOskeleton [ VAO_SKELETON_BONES ] );
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

GXVoid GXSkeletalMeshMat::InitReferencePose ()
{
	for ( GXUInt i = 0; i < skminfo.totalBones; i++ )
	{
		memcpy ( RP.Name [ i ], skminfo.Bones [ i ].Name, 64 ); 
		if ( i == 0 )
			RP.G [ 0 ].From ( skminfo.Bones[ i ].BonePos.Orientation, skminfo.Bones[ i ].BonePos.Position );
		else
			RP.L [ i ].From ( skminfo.Bones[ i ].BonePos.Orientation, skminfo.Bones[ i ].BonePos.Position );
	}
	for ( GXUInt i = 0; i < skminfo.totalBones; i++ )
	{
		if ( i != 0 )
			GXMulMat4Mat4 ( RP.G [ i ], RP.L [ i ], RP.G [ skminfo.Bones[ i ].ParentIndex ] );
		GXSetMat4Inverse ( BT [ i ], RP.G [ i ] );
	}
}

GXVoid GXSkeletalMeshMat::InitPose ( GXUInt anim_number, GXUInt frame_number )
{
	#define GET_RAW_FRAME(anim,frame) \
	( ( skmaniminfo->Animations [ anim ].FirstRawFrame + frame ) * skmaniminfo->totalbones )

	memcpy ( P.L, RP.L, sizeof( GXMat4 ) * skminfo.totalBones );
	for ( GXUInt i = 0; i < skmaniminfo->totalbones; i++ )
	{
		for ( GXUInt j = 0; j < skminfo.totalBones; j++ )
		{
			if ( 0 == strcmp ( RP.Name [ j ], skmaniminfo->Bones [ i ].Name ) )
			{
				P.L [ j ].From ( skmaniminfo->RawKeys [ GET_RAW_FRAME ( anim_number, frame_number ) + i ].Orientation, skmaniminfo->RawKeys [ GET_RAW_FRAME ( anim_number, frame_number ) + i ].Position );
				break;
			}
		}
	}
	for ( GXUInt i = 0; i < skminfo.totalBones; i++ )
	{
		if ( 0 == strcmp ( RP.Name [ i ], skmaniminfo->Bones [ 0 ].Name ) )
			P.G [ i ] = P.L [ i ];
		else
			GXMulMat4Mat4 ( P.G [ i ], P.L [ i ],P.G [ skminfo.Bones[ i ].ParentIndex ] );

		GXMulMat4Mat4 ( VT[ i ], BT[ i ], P.G[ i ] ); 
	}

	#undef GET_RAW_FRAME
}