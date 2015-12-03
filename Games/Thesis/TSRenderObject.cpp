//version 1.3

#include "TSRenderObject.h"


TSRenderObject*		ts_renderObjectTop = 0;
GXMutex*			ts_renderObjectMutex = 0;


TSRenderObject::TSRenderObject ()
{
	ts_renderObjectMutex->Lock ();

	if ( ts_renderObjectTop )
		ts_renderObjectTop->prev = this;

	next = ts_renderObjectTop;
	prev = 0;

	ts_renderObjectTop = this;

	ts_renderObjectMutex->Release ();
}

GXVoid TSRenderObject::OnUpdate ( GXFloat deltaTime )
{
	//NOTHING
}

GXVoid TSRenderObject::Delete ()
{
	/*
	Все ресурсы OpenGL необходимо удалять и создавать ТОЛЬКО В ПОТОКЕ рендерера.
	Метод OnDraw вызывает исключительно рендерер. Тут возможен вызов деструктора,
	а так же инициализация ресурсов. В методе Delete необходимо сказать объекту,
	чтобы он удалил себя при следующем вызове ренедрером метода OnDraw.
	*/
}

TSRenderObject::~TSRenderObject ()
{
	ts_renderObjectMutex->Lock ();

	if ( next ) next->prev = prev;

	if ( prev )
		prev->next = next;
	else
		ts_renderObjectTop = next;

	ts_renderObjectMutex->Release ();
}

//====================================================================================================

GXVoid GXCALL TSRenderObjectInit ()
{
	ts_renderObjectMutex = new GXMutex ();
}

GXVoid GXCALL TSRenderObjectOnUpdate ( GXFloat deltaTime )
{
	ts_renderObjectMutex->Lock ();

	TSRenderObject* p = ts_renderObjectTop;
	while ( p )
	{
		TSRenderObject* current = p;
		p = p->next;
		current->OnUpdate ( deltaTime );
	}

	ts_renderObjectMutex->Release ();
}

GXVoid GXCALL TSRenderObjectOnDraw ( eTSGBufferPass pass )
{
	ts_renderObjectMutex->Lock ();

	TSRenderObject* p = ts_renderObjectTop;
	while ( p )
	{
		TSRenderObject* current = p;
		p = p->next;
		current->OnDraw ( pass );
	}

	ts_renderObjectMutex->Release ();
}

GXVoid GXCALL TSRenderObjectDestroy ()
{
	while ( ts_renderObjectTop )
	{
		TSRenderObject* current = ts_renderObjectTop;
		ts_renderObjectTop = ts_renderObjectTop->next;
		//current->Delete ();
		//current->OnDraw ( TS_COMMON );
		delete current;
	}

	GXSafeDelete ( ts_renderObjectMutex );
}