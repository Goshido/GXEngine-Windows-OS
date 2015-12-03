//version 1.1


#include "TSGlobals.h"


TSGBuffer*				ts_GBuffer = 0;
GXCameraOrthographic*	ts_ScreenCamera = 0;
GXRenderer*				ts_Renderer = 0;
GXPhysics*				ts_Physics = 0;
GXInput*				ts_Input = 0;

GXSoundMixer*			ts_SoundMixer = 0;
GXSoundChannel*			ts_MusicChannel = 0;
GXSoundChannel*			ts_EffectChannel = 0;

TSPlayerControllerExt*	ts_PlayerController = 0;


GXVoid GXCALL TSOnExit ()
{
	GXLogW ( L"Завершение\n" );
	gx_Core->Exit ();
}