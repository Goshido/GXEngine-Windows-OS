//version 1.1

#include "TSSkyboxDesertSunny.h"


class TSDesertSunnyMesh : public GXMesh
{
	private:
		GLuint	mod_view_proj_matLocation;

	public:
		TSDesertSunnyMesh ();
		virtual ~TSDesertSunnyMesh ();

		virtual GXVoid Draw ();

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};

TSDesertSunnyMesh::TSDesertSunnyMesh ()
{
	Load3DModel ();
	InitUniforms ();
}

TSDesertSunnyMesh::~TSDesertSunnyMesh ()
{
	GXRemoveVAO ( vaoInfo );
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
	GXRemoveTexture ( mat_info.textures[ 0 ] );
}

GXVoid TSDesertSunnyMesh::Draw ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glActiveTexture ( GL_TEXTURE0 );
	glBindTexture ( GL_TEXTURE_2D, mat_info.textures[ 0 ].uiId );

	GXMat4 mod_view_proj_mat;
	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr (); 
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
}

GXVoid TSDesertSunnyMesh::Load3DModel ()
{
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/Thesis/Cube_Skybox.obj", L"../3D Models/Thesis/Cache/Cube_Skybox.cache" );
	GXLoadMTR ( L"../Materials/Thesis/Arizona_Skybox.mtr", mat_info );

	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
	GXGetTexture ( mat_info.textures[ 0 ] );
}

GXVoid TSDesertSunnyMesh::InitUniforms ()
{
	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	mod_view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_proj_mat" );
	glUniform1i ( GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "skySampler" ), 0 );

	glUseProgram ( 0 );
}

//-------------------------------------------------------------------------------------------------------


TSSkyboxDesertSunny::TSSkyboxDesertSunny ()
{
	mesh = new TSDesertSunnyMesh ();
}

TSSkyboxDesertSunny::~TSSkyboxDesertSunny ()
{
	//NOTHING 
}