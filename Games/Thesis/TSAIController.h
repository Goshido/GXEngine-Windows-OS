//version 1.1

#ifndef TS_AI_CONTROLLER
#define TS_AI_CONTROLLER


#include <GXCommon/GXTypes.h>


class TSAIController
{
	friend GXVoid GXCALL TSAIControllerOnUpdate ( GXFloat deltaTime );
	friend GXVoid GXCALL TSAIControllerOnReplicate ();

	private:
		TSAIController*		prev;
		TSAIController*		next;		

	public:
		TSAIController ();
		virtual ~TSAIController ();

		virtual GXVoid OnUpdate ( GXFloat deltatime ) = 0;
		virtual GXVoid OnReplicate () = 0;
};

//====================================================================================================

GXVoid GXCALL TSAIControllerInit ();
GXVoid GXCALL TSAIControllerOnUpdate ( GXFloat deltaTime );
GXVoid GXCALL TSAIControllerOnReplicate ();
GXVoid GXCALL TSAIControllerDestroy ();


#endif //TS_AI_CONTROLLER