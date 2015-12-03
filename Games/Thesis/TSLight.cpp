//version 1.7

#include "TSLight.h"
#include <GXEngine/GXGlobals.h>


TSShadowmap*			ts_light_Shadowmap = 0;
TSCascadeShadowmap*		ts_light_CascadeShadowmap = 0;


TSLightEmitter::TSLightEmitter ()
{
	type = TS_UNKNOWN;
	SetColor ( 1.0f, 1.0f, 1.0f );
	SetIntensity ( 1.0f );

	GXSetMat4Identity ( mod_mat );

	GXLoadMTR ( L"../Materials/Thesis/Light_Gismo.mtr", mat_info );
	GXGetShaderProgram ( mat_info.shaders[ 0 ] );
	mod_view_proj_matLocation = GXGetUniformLocation ( mat_info.shaders[ 0 ].uiId, "mod_view_proj_mat" );
}

TSLightEmitter::~TSLightEmitter ()
{
	GXRemoveShaderProgram ( mat_info.shaders[ 0 ] );
	mat_info.Cleanup ();
}

eTSLightEmitterType TSLightEmitter::GetType ()
{
	return type;
}

GXVoid TSLightEmitter::SetColor ( GXFloat r, GXFloat g, GXFloat b )
{
	color.r = r;
	color.g = g;
	color.b = b;
}

GXVoid TSLightEmitter::SetIntensity ( GXFloat intensity )
{
	if ( intensity <= 0.0f )
	{
		GXLogW ( L"TSLightEmitter::SetIntensity::Error - Параметр интенсивности должен быть строго больше нуля\n" );
		return;
	}

	this->intensity = intensity;
}

GXVoid TSLightEmitter::GetColor ( GXVec3 &color )
{
	color = this->color;
}

GXVoid TSLightEmitter::GetHDRColor ( GXVec3 &hdrColor )
{
	hdrColor.r = color.r * intensity;
	hdrColor.g = color.g * intensity;
	hdrColor.b = color.b * intensity;
}

GXFloat TSLightEmitter::GetIntensity ()
{
	return intensity;
}

GXVoid TSLightEmitter::GetHDRColorAndIntensity ( GXVec4 &hdrColorAndIntensity )
{
	hdrColorAndIntensity.r = color.r * intensity;
	hdrColorAndIntensity.g = color.g * intensity;
	hdrColorAndIntensity.b = color.b * intensity;
	hdrColorAndIntensity.a = intensity;
}

GXVoid TSLightEmitter::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	mod_mat.m41 = x;
	mod_mat.m42 = y;
	mod_mat.m43 = z;
}

GXVoid TSLightEmitter::SetLocation ( const GXVec3 &loc )
{
	mod_mat.wv = loc;
}

GXVoid TSLightEmitter::SetRotation ( const GXVec3 &rot_rad )
{
	GXVec3 pos = mod_mat.wv;
	GXSetMat4RotationXYZ ( mod_mat, rot_rad.pitch_rad, rot_rad.yaw_rad, rot_rad.roll_rad );
	mod_mat.wv = pos;
}

GXVoid TSLightEmitter::SetRotation ( const GXMat4 &rot )
{
	GXVec3 pos = mod_mat.wv;
	mod_mat = rot;
	mod_mat.wv = pos;
}

GXVoid TSLightEmitter::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXVec3 pos = mod_mat.wv;
	GXSetMat4RotationXYZ ( mod_mat, pitch_rad, yaw_rad, roll_rad );
	mod_mat.wv = pos;
}

GXVoid TSLightEmitter::SetRotation ( const GXQuat &q )
{
	GXVec3 pos = mod_mat.wv;
	mod_mat.SetRotation ( q );
	mod_mat.wv = pos;
}

GXVoid TSLightEmitter::GetLocation ( GXVec3 &loc )
{
	loc = mod_mat.wv;
}

GXVoid TSLightEmitter::GetRotation ( GXMat4 &rot )
{
	rot = mod_mat;
	rot.wv.x = rot.wv.y = rot.wv.z = 0.0f;
}

GXVoid TSLightEmitter::GetRotation ( GXQuat &rot )
{
	GXMat4 r;
	GetRotation ( r );
	rot = GXCreateQuat ( r );
}

const GXMat4& TSLightEmitter::GetModelMatrix ()
{
	return mod_mat;
}

GXVoid TSLightEmitter::GetLightVolume ( GLuint &vao, GLenum &topology, GXInt &numVertexes )
{
	vao = vaoInfo.vao;
	topology = GL_TRIANGLES;
	numVertexes = vaoInfo.numVertices;
}

GXVoid TSLightEmitter::DrawLightVolumeGismo ()
{
	if ( vaoInfo.vao == 0 )
	{
		GXLogW ( L"TSLightEmitter::DrawLightVolumeGismo::Error - Световой объём не инициализирован\n" );
		return;
	}

	GXMat4 mod_view_proj_mat;
	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();
	GXMulMat4Mat4 ( mod_view_proj_mat, mod_mat, *view_proj_mat );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );
	
	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_LINE_STRIP, 0, vaoInfo.numVertices );
}


GXVoid TSLightEmitter::LoadLightVolume ()
{
	//NOTHING
}

//---------------------------------------------------------------

TSBulp::TSBulp ()
{
	type = TS_BULP;
	SetInfluenceDistance ( 1.0f );

	LoadLightVolume ();
}

TSBulp::~TSBulp ()
{
	GXRemoveVAO ( vaoInfo );
}

GXFloat TSBulp::GetInfluenceDistance ()
{
	return distance;
}

GXVoid TSBulp::SetInfluenceDistance ( GXFloat distance )
{
	this->distance = distance;

	GXVec3 pos = mod_mat.wv;

	GXSetMat4Scale ( mod_mat, distance, distance, distance );
	mod_mat.wv = pos;
}

GXVoid TSBulp::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	mod_mat.m41 = x;
	mod_mat.m42 = y;
	mod_mat.m43 = z;
}

GXVoid TSBulp::SetLocation ( const GXVec3 &loc )
{
	mod_mat.wv = loc;
}

GXVoid TSBulp::SetRotation ( const GXVec3 &rot_rad )
{
	GXLogW ( L"TSBulp::SetRotation::Warning - Операция поворота бессмысленна\n" );
}

GXVoid TSBulp::SetRotation ( const GXMat4 &rot )
{
	GXLogW ( L"TSBulp::SetRotation::Warning - Операция поворота бессмысленна\n" );
}

GXVoid TSBulp::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXLogW ( L"TSBulp::SetRotation::Warning - Операция поворота бессмысленна\n" );
}

GXVoid TSBulp::SetRotation ( const GXQuat &q )
{
	GXLogW ( L"TSBulp::SetRotation::Warning - Операция поворота бессмысленна\n" );
}

GXVoid TSBulp::GetLocation ( GXVec3 &loc )
{
	loc = mod_mat.wv;
}

GXVoid TSBulp::GetRotation ( GXMat4 &rot )
{
	GXSetMat4Identity ( rot );
	GXLogW ( L"TSBulp::GetRotation::Warning - Операция поворота бессмысленна\n" );
}

GXVoid TSBulp::GetRotation ( GXQuat &rot )
{
	GXSetQuatIdentity ( rot );
	GXLogW ( L"TSBulp::GetRotation::Warning - Операция поворота бессмысленна\n" );
}

GXVoid TSBulp::LoadLightVolume ()
{
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/Thesis/OneSphere.obj", L"../3d Models/Thesis/Cache/OneSphere.cache" );
}

//---------------------------------------------------------------

TSSpotlight::TSSpotlight ( GXBool castShadows )
{
	type = TS_SPOT;
	isCastShadows = castShadows;

	coneAngle = GXDegToRad ( 90.0f );

	GXSetMat4Identity ( rot_mat );
	GXSetMat4Identity ( twicked_mod_mat );
	compressionXY = 1.0;

	SetInfluenceDistance ( 1.0f );
	LoadLightVolume ();

	UpdateViewProjectionMatrix ();
}

TSSpotlight::~TSSpotlight ()
{
	GXRemoveVAO ( vaoInfo );
}

GXFloat TSSpotlight::GetInfluenceDistance ()
{
	return distance;
}

GXFloat TSSpotlight::GetConeAngle ()
{
	return coneAngle;
}

GXVoid TSSpotlight::SetInfluenceDistance ( GXFloat distance )
{
	this->distance = distance;

	GXVec3 pos = mod_mat.wv;
	mod_mat = rot_mat;
	mod_mat.wv = pos;

	GXMat4 scale_mat;
	GXSetMat4Scale ( scale_mat, distance * compressionXY, distance * compressionXY, distance );

	GXMulMat4Mat4 ( twicked_mod_mat, scale_mat, rot_mat );
	twicked_mod_mat.wv = pos;

	UpdateViewProjectionMatrix ();
}

GXVoid TSSpotlight::SetConeAngle ( GXFloat angle_rad )
{
	coneAngle = angle_rad;
	compressionXY = tanf ( coneAngle * 0.5f );
	SetInfluenceDistance ( distance );
}

GXVoid TSSpotlight::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	mod_mat.wv.x = x;
	mod_mat.wv.y = y;
	mod_mat.wv.z = z;

	twicked_mod_mat.wv = mod_mat.wv;

	UpdateViewProjectionMatrix ();
}

GXVoid TSSpotlight::SetLocation ( const GXVec3 &loc )
{
	mod_mat.wv = loc;
	twicked_mod_mat.wv = mod_mat.wv;

	UpdateViewProjectionMatrix ();
}

GXVoid TSSpotlight::SetRotation ( const GXVec3 &rot_rad )
{
	GXSetMat4RotationXYZ ( rot_mat, rot_rad.pitch_rad, rot_rad.yaw_rad, rot_rad.roll_rad );
	SetInfluenceDistance ( distance );
}

GXVoid TSSpotlight::SetRotation ( const GXMat4 &rot )
{
	rot_mat = rot;
	SetInfluenceDistance ( distance );
}

GXVoid TSSpotlight::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXSetMat4RotationXYZ ( rot_mat, pitch_rad, yaw_rad, roll_rad );
	SetInfluenceDistance ( distance );
}

GXVoid TSSpotlight::SetRotation ( const GXQuat &q )
{
	rot_mat.SetRotation ( q );
	SetInfluenceDistance ( distance );
}

GXVoid TSSpotlight::GetLocation ( GXVec3 &loc )
{
	loc = mod_mat.wv;
}

const GXMat4& TSSpotlight::GetViewProjectionMatrix ()
{
	return view_proj_mat;
}

const GXMat4& TSSpotlight::GetModelMatrix ()
{
	return twicked_mod_mat;
}

GXBool TSSpotlight::IsCastedShadows ()
{
	return isCastShadows;
}

GXVoid TSSpotlight::DrawLightVolumeGismo ()
{
	if ( vaoInfo.vao == 0 )
	{
		GXLogW ( L"TSLightEmitter::DrawLightVolumeGismo::Error - Световой объём не инициализирован\n" );
		return;
	}

	GXMat4 mod_view_proj_mat;
	const GXMat4* view_proj_mat = gx_ActiveCamera->GetViewProjectionMatrixPtr ();
	GXMulMat4Mat4 ( mod_view_proj_mat, twicked_mod_mat, *view_proj_mat );

	glUseProgram ( mat_info.shaders[ 0 ].uiId );
	glUniformMatrix4fv ( mod_view_proj_matLocation, 1, GL_FALSE, mod_view_proj_mat.A );
	
	glBindVertexArray ( vaoInfo.vao );
	glDrawArrays ( GL_LINE_STRIP, 0, vaoInfo.numVertices );
}

GXVoid TSSpotlight::LoadLightVolume ()
{
	GXGetVAOFromOBJ ( vaoInfo, L"../3D Models/Thesis/OneCone.obj", L"../3D Models/Thesis/Cache/OneCone.cache" );
}

GXVoid TSSpotlight::UpdateViewProjectionMatrix ()
{
	if ( !isCastShadows ) return;

	GXMat4 proj_mat;
	GXSetMat4Perspective ( proj_mat, coneAngle, 1.0f, 0.5f, distance );

	GXMat4 view_mat;
	GXSetMat4Inverse ( view_mat, mod_mat );

	GXMulMat4Mat4 ( view_proj_mat, view_mat, proj_mat );
}

//---------------------------------------------------------------

TSDirectedLight::TSDirectedLight ( GXBool castShadows )
{
	isCastShadows =  castShadows;

	type = TS_DIRECTED;
	SetRotation ( GXDegToRad ( -90.0f ), 0.0, 0.0 );

	SetAmbientColor ( 0.0f, 0.0f, 0.0f );
	SetAmbientIntensity ( 0.0f );
}

TSDirectedLight::~TSDirectedLight ()
{
	//NOTHING
}

GXVoid TSDirectedLight::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	GXLogW ( L"TSDirectedLight::SetLocation::Warning - Операция перемещения бессмысленна\n" );
}

GXVoid TSDirectedLight::SetLocation ( const GXVec3 &loc )
{
	GXLogW ( L"TSDirectedLight::SetLocation::Warning - Операция перемещения бессмысленна\n" );
}

GXVoid TSDirectedLight::GetLocation ( GXFloat &x, GXFloat &y, GXFloat &z )
{
	GXLogW ( L"TSDirectedLight::SetLocation::Warning - Операция перемещения бессмысленна\n" );
}

GXVoid TSDirectedLight::GetLocation ( GXVec3 &loc )
{
	GXLogW ( L"TSDirectedLight::SetLocation::Warning - Операция перемещения бессмысленна\n" );
}

const GXMat4& TSDirectedLight::GetModelMatrix ()
{
	GXLogW ( L"TSDirectedLight::GetModelMatrix::Warning - Правельнее пользоваться функцией GetRotation\n" );
	return TSLightEmitter::GetModelMatrix ();
}

GXVoid TSDirectedLight::GetLightVolume ( GLuint &vao, GLenum &topology, GXInt &numVertexes )
{
	GXLogW ( L"TSDirectedLight::GetLightVolume::Warning - У направленного источника света нет светового объёма\n" );
}

GXBool TSDirectedLight::IsCastedShadows ()
{
	return isCastShadows;
}

GXVoid TSDirectedLight::DrawLightVolumeGismo ()
{
	GXLogW ( L"TSDirectedLight::DrawLightVolumeGismo::Warning - Операция бессмылслена\n" );
}

GXVoid TSDirectedLight::SetAmbientColor ( GXFloat r, GXFloat g, GXFloat b )
{
	ambient.r = r;
	ambient.g = g;
	ambient.b = b;
}

GXVoid TSDirectedLight::SetAmbientIntensity ( GXFloat intens )
{
	ambientIntens = intens;
}

GXVoid TSDirectedLight::GetHDRAmbientColorAndIntensity ( GXVec4 &hdrAmbientColorAndIntensity )
{
	hdrAmbientColorAndIntensity.r = ambient.r * ambientIntens;
	hdrAmbientColorAndIntensity.g = ambient.g * ambientIntens;
	hdrAmbientColorAndIntensity.b = ambient.b * ambientIntens;
	hdrAmbientColorAndIntensity.a = ambientIntens;
}

GXVoid TSDirectedLight::LoadLightVolume ()
{
	//NOTHING
}