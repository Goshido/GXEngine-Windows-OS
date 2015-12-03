//version 1.1

#ifndef TS_PLAYER_CONTROLLER_EXT
#define TS_PLAYER_CONTROLLER_EXT


#include "TSWeaponAIController.h"


class TSPlayerControllerExt
{
	protected:
		TSCar*					target;

		TSWeaponAIController*	machineGun;
		TSWeaponAIController*	ordinary;
		TSWeaponAIController*	special;

	public:
		TSPlayerControllerExt ();
		virtual ~TSPlayerControllerExt ();

		virtual GXVoid Update ( GXFloat deltaTime ) = 0;
		virtual GXVoid GetLocation ( GXVec3 &location ) = 0;
		virtual GXVoid GetRotation ( GXMat4 &rotation ) = 0;

		virtual GXVoid SetOrdinaryWeapon ( TSWeaponAIController* weapon ) = 0;
		virtual GXVoid SetSpecialWeapon () = 0;

		virtual GXVoid Replicate () = 0;

		GXVoid FireMachineGun ();
		GXVoid StopMachineGun ();
		GXVoid FireOrdinary ();
		GXVoid StopOrdinary ();
		GXVoid FireSpecial ();
		GXVoid StopSpecial ();

		GXVoid SetTarget ( TSCar* target );
};


#endif //TS_PLAYER_CONTROLLER_EXT