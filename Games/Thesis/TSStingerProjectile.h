//version 1.3

#ifndef TS_STINGER_PROJECTILE
#define TS_STINGER_PROJECTILE


#include "TSRenderObject.h"
#include "TSMesh.h"
#include <GXEngine/GXAttachment.h>
#include "TSExhaustParticles.h"
#include "TSExplosionParticles.h"
#include "TSBillboard.h"
#include "TSSprite.h"
#include "TSSmokeParticles.h"
#include <GXEngine/GXSoundEmitter.h>


class TSStingerProjectile : public TSRenderObject
{
	private:
		TSMesh*					stinger;
		GXAttachment*			burn;

		TSSprite*				exhaustFlash;
		TSBillboard*			explosionFlash;

		GXBool					isExplosed;
		GXFloat					deleteTimer;

		TSExhaustParticles*		exhaustParticles;
		TSExplosionParticles*	explosionParticles;
		TSSmokeParticles*		smokeParticles;

		GXSoundEmitter*			exhaustSoundEmitter;	
		GXSoundEmitter*			explosionSoundEmitter;

		GXFloat					exhaustFlashSize[ 2 ];
		GXFloat					exhaustFlashTimer;
		GXFloat					exhaustFlashTime;
		GXFloat					exhaustFlashInvTime;

		GXVec3					startLocation;
		GXMat4					startRotation;

		TSBulp*					light;

	public:
		TSStingerProjectile ( const GXVec3 &location, const GXMat4 &rotation );

		virtual GXVoid OnUpdate ( GXFloat deltaTime );
		virtual GXVoid OnDraw ( eTSGBufferPass pass );

		GXVoid GetLocation ( GXVec3 &location );
		GXVoid GetOrigin ( GXVec3 &location, GXQuat &rotation );
		GXVoid SetOrigin ( const GXVec3 &location, const GXMat4 &rotation );
		GXVoid Explose ();

	protected:
		virtual ~TSStingerProjectile ();

	private:
		GXVoid InitExhaust ();
		GXVoid InitExplosion ();
		GXVoid InitSmoke ();
};


#endif //TS_STINGER_PROJECTILE