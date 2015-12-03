//version 1.0

#ifndef TS_WEAPON_AI_CONTROLLER
#define TS_WEAPON_AI_CONTROLLER


#include "TSAIController.h"
#include <GXEngine/GXAttachment.h>
#include "TSCar.h"


class TSWeaponAIController : public TSAIController
{
	protected:
		GXInt			ammo;		//	< 0 -> infinite
		GXInt			maxAmmo;		
		GXFloat			fireTimer;
		GXFloat			fireDelay;
		GXFloat			damage;

		TSCar*			target;

		GXBool			isFire;
		GXUChar			fireType;

		GXWChar*		attachmentName;
		GXAttachment*	attachment;

	public:
		TSWeaponAIController ( const GXWChar* attachmentName );
		virtual ~TSWeaponAIController ();

		GXInt GetAmmo ();
		GXVoid CollectAmmo ( GXInt ammo );

		GXVoid SetTarget ( TSCar* target );
		const GXWChar* GetAttachementName ();
		GXVoid SetAttachment ( GXAttachment* attachment );

		GXVoid Fire ();
		GXVoid SetFireType ( GXUChar type );
		GXVoid Stop ();

		virtual GXVoid OnUpdate ( GXFloat deltaTime ) = 0;
};


#endif //TS_WEAPON_AI_CONTROLLER