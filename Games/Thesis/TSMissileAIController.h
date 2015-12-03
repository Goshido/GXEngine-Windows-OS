//version 1.0

#ifndef TS_MISSILE_AI_CONTROLLER
#define TS_MISSILE_AI_CONTROLLER


#include "TSWeaponAIController.h"
#include "TSMissile.h"


class TSMissileAIController : public TSWeaponAIController
{
	private:
		TSMissile*	missile;

	public:
		TSMissileAIController ();
		virtual ~TSMissileAIController ();

		virtual GXVoid OnUpdate ( GXFloat deltaTime );
		virtual GXVoid OnReplicate ();

	private:
		GXVoid ReplicateMe ();
		GXVoid ReplicateOrigin ();
		GXVoid ReplicateDelete ();
};


#endif //TS_MISSILE_AI_CONTROLLER