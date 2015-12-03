//version 1.0

#ifndef	TS_SCENE_KEEPER
#define TS_SCENE_KEEPER


#include <GXEngineDLL/GXPhysX.h>
#include <GXEngine/GXRenderer.h>


enum eTSSceneID
{
	MAIN_MENU	= 0,
	ARIZONA		= 1
};


class TSSceneKeeper
{
	private:
		eTSSceneID		oldID;
		eTSSceneID		currentID;

		PFNGXONFRAMEPROC*					onFrameTable;
		PFNGXONINITRENDERABLEOBJECTSPROC*	onInitTable;
		PFNGXONDELETERENDERABLEOBJECTSPROC*	onDeleteTable;
		PLONPHYSICSPROC*					onPhysicsTable;

		GXBool								isDelete;

	public:
		TSSceneKeeper ();
		GXVoid Delete ();

		GXBool OnFrame ( GXFloat delta );
		GXVoid OnPhysics ( GXFloat delta );

		GXVoid SwitchScene ( eTSSceneID id );

	private:
		~TSSceneKeeper ();
};


#endif //TS_SCENE_KEEPER