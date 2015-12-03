//version 1.1

#ifndef TS_CANNON_AI_CONTROLLER
#define TS_CANNON_AI_CONTROLLER


#include "TSWeaponAIController.h"
#include "TSCannon.h"


class TSCannonAIController : public TSWeaponAIController
{
	private:
		TSCannon*	cannon;

	public:
		TSCannonAIController ();
		virtual ~TSCannonAIController ();
		
		virtual GXVoid OnUpdate ( GXFloat deltaTime );
		virtual GXVoid OnReplicate ();

	private:
		GXVoid ReplicateMe ();
		GXVoid ReplicateOrigin ();
		GXVoid ReplicateFire ();
		GXVoid ReplicateDelete ();
};


#endif //TS_CANNON_AI_CONTROLLER