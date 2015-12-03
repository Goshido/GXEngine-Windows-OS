//version 1.1

#ifndef TS_PHYSICS_OBJECT
#define TS_PHYSICS_OBJECT


#include <GXEngineDLL/GXPhysX.h>


class TSPhysicsObject
{
	friend GXVoid GXCALL TSPhysicsObjectOnPhysics ( GXFloat deltaTime );
	friend GXVoid GXCALL TSPhysicsObjectDestroy ();

	private:
		TSPhysicsObject*		prev;
		TSPhysicsObject*		next;

	public:
		TSPhysicsObject ();
		virtual GXVoid Delete ();

		virtual GXVoid OnPhysics ( GXFloat deltaTime ) = 0;

	protected:
		virtual ~TSPhysicsObject ();
};

//====================================================================================================

GXVoid GXCALL TSPhysicsObjectInit ();
GXVoid GXCALL TSPhysicsObjectOnPhysics ( GXFloat deltaTime );
GXVoid GXCALL TSPhysicsObjectDestroy ();


#endif //TS_PHYSICS_OBJECT