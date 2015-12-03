//version 1.2

#include "TSMarker.h"
#include <GXEngine/GXGlobals.h>


TSMarker::TSMarker ()
{
	Load3DModel ();
	InitUniforms ();

	SetDiffuseColor ( 0.2f, 0.2f, 0.2f, 1.0f );
	SetEmitColor ( 0.8906f, 0.0f, 1.0f );
	SetSpecColor ( 1.0f, 1.0f, 1.0f );
	SetMetallicFactor ( 1.0f );
	SetSpecIntensityFactor ( 0.3f );
	SetSpecPower ( 50 );
}

TSMarker::~TSMarker ()
{
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
	GXRemoveVAO ( vaoInfo );
}

GXVoid TSMarker::Draw ()
{
	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();
	GXMat4 mod_view_proj_mat;
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );
	
	const GXMat4* view_mat = gx_ActiveCamera->GetViewMatrixPtr ();
	GXMat4 mod_view_mat;
	GXMulMat4Mat4 ( mod_view_mat, mod_mat, *view_mat );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );

	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );
	glUniformMatrix4fv ( normal_matLocation, 1, GL_FALSE, mod_view_mat.A );

	glUniform4fv ( diffuseLocation, 1, diffuse.v );
	glUniform3fv ( emitLocation, 1, emit.v );
	glUniform3fv ( specColorLocation, 1, specColor.v );
	glUniform3fv ( specPowMetalIntensLocation, 1, specPowMetalIntens.v );

	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_TRIANGLES, 0, vaoInfo.numVertices );
}

GXVoid TSMarker::SetDiffuseColor ( GXFloat r, GXFloat g, GXFloat b, GXFloat a )
{
	diffuse.r = r;
	diffuse.g = g;
	diffuse.b = b;
	diffuse.a = a;
}

GXVoid TSMarker::SetEmitColor ( GXFloat r, GXFloat g, GXFloat b )
{
	emit.r = r;
	emit.g = g;
	emit.b = b;
}

GXVoid TSMarker::SetSpecColor ( GXFloat r, GXFloat g, GXFloat b )
{
	specColor.r = r;
	specColor.g = g;
	specColor.b = b;
}

GXVoid TSMarker::SetSpecPower ( GXUChar specFactor )
{
	#define TS_UCHAR2FLOAT 0.003922f
	specPowMetalIntens.r = specFactor * TS_UCHAR2FLOAT;
	#undef TS_UCHAR2FLOAT
}

GXVoid TSMarker::SetMetallicFactor ( GXFloat metallic )
{
	specPowMetalIntens.g = metallic;
}

GXVoid TSMarker::SetSpecIntensityFactor ( GXFloat intensity )
{
	specPowMetalIntens.b = intensity;
}

GXVoid TSMarker::Load3DModel ()
{
	GXLoadMTR ( L"../Materials/TSMeshSimple.mtr", mat_info );
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/Marker.obj", L"../3D Models/Cached Models/Marker.cache" );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
}

GXVoid TSMarker::InitUniforms ()
{
	mod_view_proj_matLocation = GXGetUniformLocation (  mat_info.shaders[ 0 ].uiId , "mod_view_proj_mat" );
	normal_matLocation = GXGetUniformLocation (  mat_info.shaders[ 0 ].uiId , "mod_view_mat" );

	diffuseLocation = GXGetUniformLocation (  mat_info.shaders[ 0 ].uiId, "diffuse" );
	emitLocation = GXGetUniformLocation (  mat_info.shaders[ 0 ].uiId, "emit" );
	specColorLocation = GXGetUniformLocation (  mat_info.shaders[ 0 ].uiId, "specColor" );
	specPowMetalIntensLocation = GXGetUniformLocation (  mat_info.shaders[ 0 ].uiId, "specPowMetalIntens" );
}