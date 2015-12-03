//version 1.5

#include "GXQSkeletalPinkiePie.h"


GXQSkeletalPinkiePie::GXQSkeletalPinkiePie ()
{
	Load3DModel ();
	InitCharacterContent ();
}

GXVoid GXQSkeletalPinkiePie::InitCharacterContent ()
{
	ShowSkeleton ( GX_FALSE );
	InitReferencePose ();
	glGenVertexArrays ( 1, &meshVAO );
	glBindVertexArray ( meshVAO );
	//{
		glGenBuffers ( 1, &VBOxyz );
		glBindBuffer ( GL_ARRAY_BUFFER, VBOxyz );
		//{
			glBufferData ( GL_ARRAY_BUFFER, skminfo.totalVBOxyz * sizeof ( GXVec3 ), skminfo.VBOxyz, GL_STATIC_DRAW );
			glVertexAttribPointer ( GX_VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, sizeof ( GXVec3 ), (const GLvoid*)0 );
			glEnableVertexAttribArray ( GX_VERTEX_INDEX );
		//}
		glGenBuffers ( 1, &VBOuv );
		glBindBuffer ( GL_ARRAY_BUFFER, VBOuv );
		//{
			glBufferData ( GL_ARRAY_BUFFER, skminfo.totalVBOuv * sizeof ( GXVec2 ), skminfo.VBOuv, GL_STATIC_DRAW );
			glVertexAttribPointer ( GX_UV_COORD_INDEX, 2, GL_FLOAT, GL_FALSE, sizeof ( GXVec2 ), (const GLvoid*)0 );
			glEnableVertexAttribArray ( GX_UV_COORD_INDEX );
		//}
		glGenBuffers ( 1, &VBOskin );
		glBindBuffer ( GL_ARRAY_BUFFER, VBOskin );
		//{
			glBufferData ( GL_ARRAY_BUFFER, skminfo.totalVBOskin * sizeof ( GXVBOSkinStruct ), skminfo.VBOskin, GL_STATIC_DRAW );
			glVertexAttribPointer ( SKIN_INDEX, 4, GL_FLOAT, GL_FALSE, sizeof ( GXVBOSkinStruct ), (const GLvoid* )0 );
			glVertexAttribPointer ( SKIN_WEIGHT, 4, GL_FLOAT, GL_FALSE, sizeof ( GXVBOSkinStruct ), (const GLvoid*)sizeof ( GXVec4 ) );
			glEnableVertexAttribArray ( SKIN_INDEX );
			glEnableVertexAttribArray ( SKIN_WEIGHT );
		//}
		glGenBuffers ( 1, &element_array_buffer );
		glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, element_array_buffer );
			glBufferData ( GL_ELEMENT_ARRAY_BUFFER, skminfo.totalelements * sizeof ( GLushort ), skminfo.ElementArray, GL_STATIC_DRAW );
	//}
	glBindVertexArray ( 0 );
	glBindBuffer ( GL_ARRAY_BUFFER, 0 );
	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );

	free ( skminfo.VBOxyz );
	free ( skminfo.VBOuv );
	free ( skminfo.ElementArray );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
	InitUniforms ();
	InitDrawableSkeleton ();
}

GXQSkeletalPinkiePie::~GXQSkeletalPinkiePie ()
{
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
	GXRemoveTexture ( mat_info.textures[ 0 ] );
	GXRemoveTexture ( mat_info.textures[ 1 ] );
	free ( skminfo.Bones );
	glDeleteVertexArrays ( 1, &meshVAO );
	glDeleteBuffers ( 1, &VBOxyz );
	glDeleteBuffers ( 1, &VBOuv );
	glDeleteBuffers ( 1, &VBOskin );
	glDeleteVertexArrays ( 2, VAOskeleton );
	glDeleteBuffers ( 1, &VBOskeleton );
}

GXVoid GXQSkeletalPinkiePie::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "textureDiffuse" ), 0 );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "textureFiguration" ), 1 );
	jointsLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "bones" );
	mod_view_proj_mat_Location = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_proj_mat" ); 
	glUseProgram ( 0 );
}

GXVoid GXQSkeletalPinkiePie::Draw ()
{
	animSolver->Update ( deltaTime * 0.001f );
	InitPose ();
	
	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniformMatrix4fv ( mod_view_proj_mat_Location, 1, GL_FALSE, mod_view_proj_mat.A );
	glUniform1fv ( jointsLocation, MAX_BONES_FLOATS, (GLfloat*)vertexTransform );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 0 ].uiId );
	glActiveTexture ( GL_TEXTURE1 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 1 ].uiId );

	glBindVertexArray ( meshVAO );
	glDrawElements ( GL_TRIANGLES, skminfo.totalelements, GL_UNSIGNED_SHORT, 0 );

	glBindTexture ( GL_TEXTURE_2D, 0 );
	glBindVertexArray ( 0 );
	glUseProgram ( 0 );

	if ( bShowSkeleton )
		DrawSkeleton ( gx_ActiveCamera );
}

GXVoid GXQSkeletalPinkiePie::Load3DModel ()
{
	GXLoadPSK ( L"../3D Models/Test/Rigged PinkiePie/PinkiePie.PSK", &skminfo );
	GXLoadMTR ( L"../Materials/Test/Pinkie_Pie.mtr", mat_info );
	GXGetTexture ( mat_info.textures[ 0 ] );
	GXGetTexture ( mat_info.textures[ 1 ] );
}

GXVoid GXQSkeletalPinkiePie::SetLocation ( GXFloat* Loc )
{
	GXMesh::SetLocation ( Loc );
}

GXVoid GXQSkeletalPinkiePie::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	GXMesh::SetLocation ( x, y, z );
}

GXVoid GXQSkeletalPinkiePie::SetRotation ( GXFloat* Rot_rad )
{
	GXMesh::SetRotation ( Rot_rad );
}

GXVoid GXQSkeletalPinkiePie::SetRotation ( const GXVec3 &rot_rad )
{
	GXMesh::SetRotation ( rot_rad );
}

GXVoid GXQSkeletalPinkiePie::SetRotation ( GXQuat qt )
{
	GXMesh::SetRotation ( qt );
}

GXVoid GXQSkeletalPinkiePie::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXMesh::SetRotation ( pitch_rad, yaw_rad, roll_rad );
}

GXVoid GXQSkeletalPinkiePie::Update ( GXDword deltaTime )
{
	this->deltaTime = deltaTime;
}