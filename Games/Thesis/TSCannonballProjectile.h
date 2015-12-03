//version 1.0

#ifndef TS_CANNONBALL_PROJECTILE
#define TS_CANNONBALL_PROJECTILE


#include "TSRenderObject.h"
#include <GXEngine/GXAttachment.h>
#include "TSExplosionParticles.h"
#include "TSBillboard.h"
#include "TSSmokeParticles.h"
#include <GXEngine/GXSoundEmitter.h>
#include <GXEngine/GXOGGSoundProvider.h>


class TSCannonballProjectile : public TSRenderObject
{
	private:
		TSBillboard*			cannonball;
		TSBillboard*			explosionFlash;

		GXBool					isExplosed;
		GXFloat					deleteTimer;

		TSExplosionParticles*	explosionParticles;
		TSSmokeParticles*		smokeParticles;

		GXSoundEmitter*			explosionSoundEmitter;
		GXSoundEmitter*			launchSoundEmitter;

		GXVec3					startLocation;
		GXMat4					startRotation;

		TSBulp*					light;

	public:
		TSCannonballProjectile ( const GXVec3 &location, const GXMat4 &rotation );

		virtual GXVoid OnUpdate ( GXFloat deltaTime );
		virtual GXVoid OnDraw ( eTSGBufferPass pass );

		GXVoid GetLocation ( GXVec3 &location );
		GXVoid SetOrigin ( const GXVec3 &location, const GXMat4 &rotation );
		GXVoid Explose ();

	protected:
		virtual ~TSCannonballProjectile ();

	private:
		GXVoid InitExplosion ();
		GXVoid InitSmoke ();

		GXVoid InitRenderResources ();
};


#endif //TS_CANNONBALL_PROJECTILE