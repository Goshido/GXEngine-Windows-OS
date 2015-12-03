//version 1.1

#include "TSLandCruiser.h"



TSLandCruiser::TSLandCruiser ()
{
	body = new TSMesh ( L"../3D Models/Thesis/Land_Cruiser.obj", L"../3D Models/Thesis/Cache/Land_Cruiser.cache", L"../Materials/Thesis/Land_Cruiser_Prado.mtr" );
	ts_light_Shadowmap->IncludeShadowCaster ( body );
	ts_light_CascadeShadowmap->IncludeShadowCaster ( body );

	for ( GXUChar i = 0; i < 4; i++ )
	{
		wheels[ i ] = new TSMesh ( L"../3D Models/Thesis/Land_Cruiser_Wheel.obj", L"../3D Models/Thesis/Cache/Land_Cruiser_Wheel.cache", L"../Materials/Thesis/Land_Cruiser_Prado.mtr" );
		ts_light_Shadowmap->IncludeShadowCaster ( wheels[ i ] );
		ts_light_CascadeShadowmap->IncludeShadowCaster ( wheels[ i ] );
	}

	GXMat4 offset;
	GXSetMat4RotationXYZ ( offset, 0.0f, -GX_MATH_HALFPI, 0.0f );
	GXConvert3DSMaxToGXEngine ( offset.wv, 0.0f, -2.641f, -0.227f );
	attachments[ 0 ] = new GXAttachment ( L"Machine Gun", body, offset );

	GXSetMat4Identity ( offset );
	GXConvert3DSMaxToGXEngine ( offset.wv, 0.0f, 1.052f, 0.975f );
	attachments[ 1 ] = new GXAttachment ( L"Red", body, offset );

	GXSetMat4RotationXYZ ( offset, 0.0f, -GX_MATH_HALFPI, 0.0f );
	GXConvert3DSMaxToGXEngine ( offset.wv, 0.0f, 2.067f, 0.632f );
	attachments[ 2 ] = new GXAttachment ( L"Blue", body, offset );

	GXSetMat4RotationXYZ ( offset, 0.0f, GX_MATH_HALFPI, 0.0f );
	GXConvert3DSMaxToGXEngine ( offset.wv, 0.0f, 2.102f, -0.256f );
	attachments[ 3 ] = new GXAttachment ( L"Green", body, offset );

	GXSetMat4RotationXYZ ( offset, 0.0f, -GX_MATH_HALFPI, 0.0f );
	GXConvert3DSMaxToGXEngine ( offset.wv, -0.988f, -1.745f, 0.108f );
	attachments[ 4 ] = new GXAttachment ( L"Black", body, offset );

	GXSetMat4RotationXYZ ( offset, 0.0f, -GX_MATH_HALFPI, 0.0f );
	GXConvert3DSMaxToGXEngine ( offset.wv, -0.988f, -1.856f, 0.412f );
	attachments[ 5 ] = new GXAttachment ( L"Special", body, offset );

	isDelete = GX_FALSE;
}

GXVoid TSLandCruiser::Delete ()
{
	isDelete = GX_TRUE;
}

GXVoid TSLandCruiser::OnDraw ( eTSGBufferPass pass )
{
	if ( isDelete )
	{
		delete this;
		return;
	}

	if ( pass != TS_COMMON ) return;

	body->Draw ();
	for ( GXUChar i = 0; i < 4; i++ )
		wheels[ i ]->Draw ();
}

GXVoid TSLandCruiser::SetBodyOrigin ( const GXVec3 &location, const GXMat4 &rotation )
{
	body->SetLocation ( location );
	body->SetRotation ( rotation );
}

GXVoid TSLandCruiser::SetWheelOrigin ( GXUShort id, const GXVec3 &location, const GXMat4 &rotation )
{
	wheels[ id ]->SetLocation ( location );
	wheels[ id ]->SetRotation ( rotation );
}

GXVoid TSLandCruiser::GetBodyOrigin ( GXVec3 &location, GXMat4 &rotation )
{
	if ( !body )
	{
		location = GXVec3 ( 0.0f, 0.0f, 0.0f );
		GXSetMat4Identity ( rotation );
		return;
	}

	body->GetLocation ( location );
	body->GetRotation ( rotation );
}

GXVoid TSLandCruiser::GetWheelOrigin ( GXUShort id, GXVec3 &location, GXMat4 &rotation )
{
	wheels[ id ]->GetLocation ( location );
	wheels[ id ]->GetRotation ( rotation );
}

GXAttachment* TSLandCruiser::GetAttachment ( const GXWChar* name )
{
	for ( GXUChar i = 0; i < 6; i++ )
	{
		if ( attachments[ i ] && wcscmp ( attachments[ i ]->GetName (), name ) == 0 )
			return attachments[ i ];
	}

	return 0;
}

TSLandCruiser::~TSLandCruiser ()
{
	ts_light_Shadowmap->ExcludeShadowCaster ( body );

	for ( GXUChar i = 0; i < 4; i++ )
		ts_light_Shadowmap->ExcludeShadowCaster ( wheels[ i ] );

	for ( GXUChar i = 0; i < 6; i++ )
		delete attachments[ i ];

	for ( GXUChar i = 0; i < 4; i++ )
	{
		wheels[ i ]->Delete ();
		wheels[ i ]->Draw ();
	}

	body->Delete ();
	body->Draw ();
}