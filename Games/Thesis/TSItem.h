//version 1.0

#ifndef TS_ITEM
#define TS_ITEM


#include "TSMesh.h"


class TSItem
{
	private:
		TSMesh* mesh;

		GXBool	isActive;
		GXFloat	respawnDelay;
		GXFloat respawnTime;

		GXFloat angle;


	public:
		TSItem ( TSMesh* mesh, GXFloat respawnDelay );
		~TSItem ();

		GXVoid Update ( GXFloat deltaTime );
		GXVoid OnPickUp ();
};


#endif //TS_ITEM