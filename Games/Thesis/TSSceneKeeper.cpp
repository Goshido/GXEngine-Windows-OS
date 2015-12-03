//version 1.0

#include "TSSceneKeeper.h"
#include "TSMainMenu.h"
#include "TSArizonaMap.h"


TSSceneKeeper* ts_SceneKeeper = 0;


TSSceneKeeper::TSSceneKeeper ()
{
	oldID = currentID = MAIN_MENU;

	onFrameTable = (PFNGXONFRAMEPROC*)malloc ( 2 * sizeof ( PFNGXONFRAMEPROC ) );
	onInitTable = (PFNGXONINITRENDERABLEOBJECTSPROC*)malloc ( 2 * sizeof ( PFNGXONINITRENDERABLEOBJECTSPROC ) );
	onDeleteTable = (PFNGXONDELETERENDERABLEOBJECTSPROC*)malloc ( 2 * sizeof ( PFNGXONDELETERENDERABLEOBJECTSPROC ) );
	onPhysicsTable = (PLONPHYSICSPROC*)malloc ( 2 * sizeof ( PLONPHYSICSPROC ) );

	onFrameTable[ MAIN_MENU ] = &TSMainMenuOnFrame;
	onInitTable[ MAIN_MENU ] = &TSMainMenuOnInit;
	onDeleteTable[ MAIN_MENU ] = &TSMainMenuOnDelete;
	onPhysicsTable[ MAIN_MENU ] = &TSMainMenuOnPhysics;

	onFrameTable[ ARIZONA ] = &TSArizonaOnFrame;
	onInitTable[ ARIZONA ] = &TSArizonaOnInit;
	onDeleteTable[ ARIZONA ] = &TSArizonaOnDelete;
	onPhysicsTable[ ARIZONA ] = &TSArizonaOnPhysics;

	TSMainMenuOnInit ();

	isDelete = GX_FALSE;
}

GXVoid TSSceneKeeper::Delete ()
{
	isDelete = GX_TRUE;
}

GXBool TSSceneKeeper::OnFrame ( GXFloat delta )
{
	if ( isDelete )
	{
		delete this;
		return GX_FALSE;
	}

	if ( currentID != oldID )
	{
		onDeleteTable[ oldID ] ();
		onInitTable[ currentID ] ();
		
		oldID = currentID;
	}

	return onFrameTable[ currentID ] ( delta );
}

GXVoid TSSceneKeeper::OnPhysics ( GXFloat delta )
{
	onPhysicsTable[ currentID ] ( delta );
}

GXVoid TSSceneKeeper::SwitchScene ( eTSSceneID id )
{
	currentID = id;
}

TSSceneKeeper::~TSSceneKeeper ()
{
	onDeleteTable[ currentID ] ();

	free ( onFrameTable );
	free ( onInitTable );
	free ( onDeleteTable );
	free ( onPhysicsTable );
}