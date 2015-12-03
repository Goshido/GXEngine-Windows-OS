//version 1.0

#include "TSWeaponAIController.h"
#include <stdlib.h>



TSWeaponAIController::TSWeaponAIController ( const GXWChar* attachmentName )
{
	GXUInt size = sizeof ( GXWChar ) * ( wcslen ( attachmentName ) + 1 );
	this->attachmentName = (GXWChar*)malloc ( size );
	memcpy ( this->attachmentName, attachmentName, size );

	target = 0;
	attachment = 0;

	ammo = 20;
	maxAmmo = 20;
	fireDelay = 0.8f;
	fireTimer = 0.0f;
	damage = 100.0f;

	isFire = GX_FALSE;
	fireType = 0;
}

TSWeaponAIController::~TSWeaponAIController ()
{
	free ( attachmentName );
}

GXInt TSWeaponAIController::GetAmmo ()
{
	return ammo;
}

GXVoid TSWeaponAIController::CollectAmmo ( GXInt ammo )
{
	this->ammo += ammo;
	if ( this->ammo > maxAmmo )
		this->ammo = maxAmmo;
}

GXVoid TSWeaponAIController::SetTarget ( TSCar* target )
{
	this->target = target;
}

const GXWChar* TSWeaponAIController::GetAttachementName ()
{
	return attachmentName;
}

GXVoid TSWeaponAIController::SetAttachment ( GXAttachment* attachment )
{
	this->attachment = attachment;
}

GXVoid TSWeaponAIController::Fire ()
{
	isFire = GX_TRUE;
}

GXVoid TSWeaponAIController::SetFireType ( GXUChar type )
{
	fireType = type;
}

GXVoid TSWeaponAIController::Stop ()
{
	isFire = GX_FALSE;
}