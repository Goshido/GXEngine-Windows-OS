//version 1.1


#include "TSPhysicsObject.h"
#include <GXCommon/GXMutex.h>
#include <GXCommon/GXCommon.h>


TSPhysicsObject*	ts_physics_objectTop = 0;
GXMutex*			ts_physics_objectMutex = 0;


TSPhysicsObject::TSPhysicsObject ()
{
	ts_physics_objectMutex->Lock ();

	if ( ts_physics_objectTop )
		ts_physics_objectTop->prev = this;

	next = ts_physics_objectTop;
	prev = 0;

	ts_physics_objectTop = this;

	ts_physics_objectMutex->Release ();
}

GXVoid TSPhysicsObject::OnPhysics ( GXFloat deltaTime )
{
	//NOTHING
}

GXVoid TSPhysicsObject::Delete ()
{
	/*
	Все ресурсы PhysX необходимо удалять и создавать ТОЛЬКО В ПОТОКЕ физики.
	В метод OnPhysics вызывает исключительно физика и там возможен вызов деструктора.
	В методе Delete необходимо сказать объекту,	чтобы он удалил себя при следующем
	вызове физикой метода OnPhysics.
	*/
}

TSPhysicsObject::~TSPhysicsObject ()
{
	ts_physics_objectMutex->Lock ();

	if ( next ) next->prev = prev;

	if ( prev )
		prev->next = next;
	else
		ts_physics_objectTop = next;

	ts_physics_objectMutex->Release ();
}

//====================================================================================================

GXVoid GXCALL TSPhysicsObjectInit ()
{
	ts_physics_objectMutex = new GXMutex ();
}

GXVoid GXCALL TSPhysicsObjectOnPhysics ( GXFloat deltaTime )
{
	if ( !ts_physics_objectMutex ) return;

	ts_physics_objectMutex->Lock ();

	TSPhysicsObject* p = ts_physics_objectTop;
	while ( p )
	{
		TSPhysicsObject* current = p;
		p = p->next;
		current->OnPhysics ( deltaTime );
	}

	ts_physics_objectMutex->Release ();
}

GXVoid GXCALL TSPhysicsObjectDestroy ()
{
	while ( ts_physics_objectTop )
	{
		TSPhysicsObject* current = ts_physics_objectTop;
		ts_physics_objectTop = ts_physics_objectTop->next;
		current->Delete ();
		current->OnPhysics ( 0 );
	}

	GXSafeDelete ( ts_physics_objectMutex );
}