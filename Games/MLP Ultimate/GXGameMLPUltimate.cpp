//version 1.0

#include "GXGameMLPUltimate.h"
#include <GXEngine/GXGlobals.h>


GXRenderer*			gx_game_mlp_ultimate_Renderer = 0;
GXInput*			gx_game_mlp_ultimate_Input = 0;



//------------------------------------------------------------------------------------------------------

GXVoid GXCALL MLPUltimateShutdown ()
{
	gx_Core->Exit ();
}

//------------------------------------------------------------------------------------------------------

GXBool GXCALL OnMLPUltimateFrame ( GXFloat delta )
{
	return GX_TRUE;
}

GXVoid GXCALL OnMLPUltimateInitRenderableObjects ()
{

}

GXVoid GXCALL OnMLPUltimateDeleteRenderableObjects ()
{

}

//------------------------------------------------------------------------------------------------------

GXVoid GXCALL OnMLPUltimateGameInit ()
{
	gx_game_mlp_ultimate_Renderer = gx_Core->GetRenderer ();
	gx_game_mlp_ultimate_Renderer->SetOnInitRenderableObjectsFunc ( &OnMLPUltimateInitRenderableObjects );
	gx_game_mlp_ultimate_Renderer->SetOnFrameFunc ( &OnMLPUltimateFrame );
	gx_game_mlp_ultimate_Renderer->SetOnDeleteRenderableObjectsFunc ( &OnMLPUltimateDeleteRenderableObjects );

	gx_game_mlp_ultimate_Input = gx_Core->GetInput ();
	gx_game_mlp_ultimate_Input->BindKeyFunc ( &MLPUltimateShutdown, VK_ESCAPE, INPUT_DOWN );
}

GXVoid GXCALL OnMLPUltimateGameClose ()
{
	gx_game_mlp_ultimate_Input->UnBindKeyFunc ( VK_ESCAPE, INPUT_DOWN );
}