//version 1.0

#include "TSItem.h"


TSItem::TSItem ( TSMesh* mesh, GXFloat respawnDelay )
{
	this->mesh = mesh;
	isActive = GX_TRUE;
	this->respawnDelay = respawnDelay;
	respawnTime = respawnDelay;

	angle = 0.0f;
}

TSItem::~TSItem ()
{
	//NOTHING
}

GXVoid TSItem::Update ( GXFloat deltaTime )
{
	if ( isActive && mesh )
	{
		angle += deltaTime * 4.2f;
		GXQuat q;
		GXSetQuatRotationAxis ( q, GXCreateVec3 ( 0.0f, 1.0f, 0.0f ), angle );
		mesh->SetRotation ( q );
	}
	else
	{
		respawnTime -= deltaTime;
		if ( respawnTime <= 0 )
		{
			isActive = GX_TRUE;
			mesh->Hide ( GX_FALSE );
		}
	}
}

GXVoid TSItem::OnPickUp ()
{
	isActive = GX_FALSE;
	respawnTime = respawnDelay;
	mesh->Hide ( GX_TRUE );
}