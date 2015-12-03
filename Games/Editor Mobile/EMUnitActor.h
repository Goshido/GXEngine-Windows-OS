#ifndef EM_UNIT_ACTOR
#define EM_UNIT_ACTOR


#include "EMActor.h"
#include <GXEngine/GXMesh.h>


#define EM_UNIT_ACTOR_CLASS		0


class EMUnitActorMesh;

class EMUnitActor : public EMActor
{
	private:
		EMUnitActorMesh*	mesh;

	public:
		EMUnitActor ( const GXWChar* name, const GXMat4 &origin );
		virtual ~EMUnitActor ();

		virtual GXVoid OnDrawCommonPass ();
		virtual GXVoid OnOriginChanged ();
};


#endif //EM_UNIT_ACTOR
