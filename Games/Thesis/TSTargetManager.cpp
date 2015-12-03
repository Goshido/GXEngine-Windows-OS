//version 1.1

#include "TSTargetManager.h"
#include <GXCommon/GXMutex.h>


TSTargetManager		ts_targetManager;
GXMutex*			ts_targetManagerMutex = 0;


TSTargetManager::TSTargetManager ()
{
	top = 0;
	ts_targetManagerMutex = new GXMutex ();
}

TSTargetManager::~TSTargetManager ()
{
	ts_targetManagerMutex->Lock ();
	GXSafeDelete ( top );
	ts_targetManagerMutex->Release ();
	GXSafeDelete ( ts_targetManagerMutex );
}

GXVoid TSTargetManager::CreateTarget ( TSCar* target )
{
	ts_targetManagerMutex->Lock ();

	TSTargetList* t = new TSTargetList;
	t->target = target;
	t->prev = 0;

	t->next = top;

	if ( top )
		top->prev = t;

	ts_targetManagerMutex->Release ();
}

TSCar* TSTargetManager::GetNextTarget ( TSCar* currentTarget )
{
	ts_targetManagerMutex->Lock ();

	for ( TSTargetList* p = top; p; p = p->next )
	{
		if ( p->target == currentTarget )
		{
			if ( p->next )
			{
				ts_targetManagerMutex->Release ();
				return p->next->target;
			}
			else if ( top )
			{
				ts_targetManagerMutex->Release ();
				return top->target;
			}
			
			ts_targetManagerMutex->Release ();
			return 0;
		}
	}

	ts_targetManagerMutex->Release ();
	return 0;
}

GXBool TSTargetManager::IsValid ( TSCar* target )
{
	ts_targetManagerMutex->Lock ();

	for ( TSTargetList* p = top; p; p = p->next )
	{
		if ( p->target == target )
		{
			ts_targetManagerMutex->Release ();
			return GX_TRUE;
		}
	}

	ts_targetManagerMutex->Release ();
	return GX_FALSE;
}

GXVoid TSTargetManager::DeleteTarget ( TSCar* target )
{
	ts_targetManagerMutex->Lock ();

	TSTargetList* p = top;

	while ( p && p->target != target )
	{
		p = p->next;
	}

	if ( !p )
	{
		ts_targetManagerMutex->Release ();
		return;
	}

	if ( p->next ) p->next->prev = p->prev;
	if ( p->prev ) 
		p->prev->next = p->next;
	else
		top = p->next;

	delete p;

	ts_targetManagerMutex->Release ();
}