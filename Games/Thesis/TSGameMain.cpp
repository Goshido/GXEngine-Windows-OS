//version 1.6

#include "TSGameMain.h"
#include "TSGlobals.h"


GXBool GXCALL TSOnFrame ( GXFloat delta )
{
	return ts_SceneKeeper->OnFrame ( delta );
}

GXVoid GXCALL TSOnInitRenderableObjects ()
{
	ts_Renderer = gx_Core->GetRenderer ();
	ts_Physics = gx_Core->GetPhysics ();
	ts_Input = gx_Core->GetInput ();
	ts_SoundMixer = gx_Core->GetSoundMixer ();

	ts_ScreenCamera = new GXCameraOrthographic ( gx_EngineSettings.rendererWidth, gx_EngineSettings.rendererHeight, 0.1f, 5000.0f );

	ts_GBuffer = new TSGBuffer ( ts_ScreenCamera );

	ts_SceneKeeper = new TSSceneKeeper ();
}

GXVoid GXCALL TSOnDeleteRenderableObjects ()
{
	ts_SceneKeeper->Delete ();
	ts_SceneKeeper->OnFrame ( 0.0f );

	delete ts_GBuffer;
	delete ts_ScreenCamera;
}

GXVoid GXCALL TSOnUpdate ( GXFloat delta )
{
	ts_SceneKeeper->OnPhysics ( delta );
}

GXVoid GXCALL TSGameInit ()
{
	GXLogW ( L"====Магистерская диссертация на тему \"Игровой движок\"\n" );
	GXLogW ( L"Студент\t\tМатазимов Георгий Андреевич\nГруппа\t\tИТД-М1-21\n" );
	GXLogW ( L"Контакты:\n" );
	GXLogW ( L"\tEmail:\tGoshidoMatazuki@mail.ru\n\t\tGoshidoMatazuki@gmail.com\n" );

	gx_Core->GetPhysics ()->SetOnPhysicsFunc ( &TSOnUpdate );

	GXRenderer* renderer = gx_Core->GetRenderer ();
	renderer->SetOnFrameFunc ( &TSOnFrame );
	renderer->SetOnInitRenderableObjectsFunc ( &TSOnInitRenderableObjects );
	renderer->SetOnDeleteRenderableObjectsFunc ( &TSOnDeleteRenderableObjects );

	ts_MusicChannel = new GXSoundChannel ();
	ts_SoundMixer->AddChannel ( ts_MusicChannel );

	ts_EffectChannel = new GXSoundChannel ();
	ts_SoundMixer->AddChannel ( ts_EffectChannel );
}

GXVoid GXCALL TSGameClose ()
{	
	GXLogW ( L"Выход\n" );
}