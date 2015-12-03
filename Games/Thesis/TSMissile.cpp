//version 1.1

#include "TSMissile.h"


TSMissile::TSMissile ()
{
	missile = new TSMesh ( L"../3D Models/Thesis/Missile_Pack.obj", L"../3D Models/Thesis/Cache/Missile_Pack.cache", L"../Materials/Thesis/Missile.mtr" );
	missile->SetScale ( 0.65f, 0.65f, 0.65f );

	isDelete = GX_FALSE;
}

GXVoid TSMissile::Delete ()
{
	isDelete = GX_TRUE;
}

GXVoid TSMissile::OnDraw ( eTSGBufferPass pass )
{
	if ( isDelete )
	{
		delete this;
		return;
	}

	if ( pass != TS_COMMON ) return;

	missile->Draw ();
}

GXVoid TSMissile::SetOrigin ( const GXVec3 &location, const GXMat4 &rotation )
{
	missile->SetLocation ( location );
	missile->SetRotation ( rotation );
}

GXVoid TSMissile::GetOrigin ( GXVec3 &location, GXQuat &rotation )
{
	missile->GetLocation ( location );
	missile->GetRotation ( rotation );
}

GXVoid TSMissile::GetFireOrigin ( GXVec3 &location, GXMat4 &rotation )
{
	missile->GetLocation ( location );
	missile->GetRotation ( rotation );
	location.y += 1.0f;
}

TSMissile::~TSMissile ()
{
	missile->Delete ();
	missile->Draw ();
}