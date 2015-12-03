//version 1.1

#include "TSAIController.h"
#include <GXCommon/GXMutex.h>
#include <GXCommon/GXCommon.h>


TSAIController*		ts_aicontrollerTop = 0;
GXMutex*			ts_aicontrollerMutex = 0;



TSAIController::TSAIController ()
{
	ts_aicontrollerMutex->Lock ();

	if ( ts_aicontrollerTop )
		ts_aicontrollerTop->prev = this;

	next = ts_aicontrollerTop;
	prev = 0;

	ts_aicontrollerTop = this;

	ts_aicontrollerMutex->Release ();
}

TSAIController::~TSAIController ()
{
	ts_aicontrollerMutex->Lock ();

	if ( next ) next->prev = prev;

	if ( prev )
		prev->next = next;
	else
		ts_aicontrollerTop = next;

	ts_aicontrollerMutex->Release ();
}

//====================================================================================================

GXVoid GXCALL TSAIControllerInit ()
{
	ts_aicontrollerMutex = new GXMutex ();
}

GXVoid GXCALL TSAIControllerOnUpdate ( GXFloat deltaTime )
{
	if ( !ts_aicontrollerMutex ) return;

	ts_aicontrollerMutex->Lock ();

	TSAIController* p = ts_aicontrollerTop;

	while ( p )
	{
		TSAIController* current = p;
		p = p->next;
		current->OnUpdate ( deltaTime );
	}

	ts_aicontrollerMutex->Release ();
};

GXVoid GXCALL TSAIControllerOnReplicate ()
{
	ts_aicontrollerMutex->Lock ();

	TSAIController* p = ts_aicontrollerTop;

	for ( TSAIController* p = ts_aicontrollerTop; p; p = p->next )
		p->OnReplicate ();

	ts_aicontrollerMutex->Release ();
}

GXVoid GXCALL TSAIControllerDestroy ()
{
	GXSafeDelete ( ts_aicontrollerMutex );
}