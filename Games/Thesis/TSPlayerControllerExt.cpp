//version 1.1

#include "TSPlayerControllerExt.h"


TSPlayerControllerExt::TSPlayerControllerExt ()
{
	target = 0;

	machineGun = 0;
	ordinary = 0;
	special = 0;
}

TSPlayerControllerExt::~TSPlayerControllerExt ()
{
	GXSafeDelete ( machineGun );
	GXSafeDelete ( ordinary );
	GXSafeDelete ( special );
}

GXVoid TSPlayerControllerExt::FireMachineGun ()
{
	if ( machineGun )
		machineGun->Fire ();
}

GXVoid TSPlayerControllerExt::StopMachineGun ()
{
	if ( machineGun )
		machineGun->Stop ();
}

GXVoid TSPlayerControllerExt::FireOrdinary ()
{

	if ( ordinary )
		ordinary->Fire ();
}

GXVoid TSPlayerControllerExt::StopOrdinary ()
{
	if ( ordinary )
		ordinary->Stop ();
}

GXVoid TSPlayerControllerExt::FireSpecial ()
{
	if ( special )
		special->Fire ();
}

GXVoid TSPlayerControllerExt::StopSpecial ()
{
	if ( special )
		special->Stop ();
}


GXVoid TSPlayerControllerExt::SetTarget ( TSCar* target )
{
	this->target = target;

	if ( ordinary )
		ordinary->SetTarget ( target );

	if ( special )
		special->SetTarget ( target );
}