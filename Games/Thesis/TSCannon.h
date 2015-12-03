//version 1.2

#ifndef TS_CANNON
#define TS_CANNON


#include "TSRenderObject.h"
#include "TSSmokeParticles.h"
#include "TSSprite.h"
#include "TSMesh.h"
#include "TSBillboard.h"
#include <GXEngine/GXAttachment.h>


class TSCannon : public TSRenderObject
{
	private:
		TSMesh*				cannon;
		TSSprite*			muzzleFlash01;
		TSBillboard*		muzzleFlash02;

		TSSmokeParticles*	launchParticles;

		GXAttachment*		flashAttachment;
		GXAttachment*		smokeAttachment;

		GXFloat				timer;
		GXFloat				delay;

		GXBool				isDelete;

	public:
		TSCannon ();
		virtual GXVoid Delete ();

		virtual GXVoid OnUpdate ( GXFloat deltaTime );
		virtual GXVoid OnDraw ( eTSGBufferPass pass );
		
		GXVoid SetOrigin ( const GXVec3 &location, const GXMat4 &rotation );
		GXVoid GetOrigin ( GXVec3 &location, GXQuat &rotation );

		GXVoid Fire ();
		GXVoid GetMuzzleFlashOrigin ( GXVec3 &location, GXMat4 &rotation );

	protected:
		virtual ~TSCannon ();
};


#endif //TS_CANNON