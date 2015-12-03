//version 1.1

#ifndef TS_TARGET_MANAGER
#define TS_TARGET_MANAGER


#include "TSCar.h"


struct TSTargetList
{
	TSTargetList*	next;
	TSTargetList*	prev;
	TSCar*			target;
};


class TSTargetManager
{
	private:
		TSTargetList*	top;

	public:
		TSTargetManager ();
		~TSTargetManager ();

		GXVoid CreateTarget ( TSCar* target );
		TSCar* GetNextTarget ( TSCar* currentTarget );
		GXBool IsValid ( TSCar* target );
		GXVoid DeleteTarget ( TSCar* target );
};

extern TSTargetManager	ts_TargetManager;


#endif //TS_TARGET_MANAGER