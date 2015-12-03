//version 1.1

#ifndef TS_GLOBALS
#define TS_GLOBALS


#include <GXEngine/GXGlobals.h>
#include "TSGBuffer.h"
#include "TSSceneKeeper.h"
#include "TSPlayerControllerExt.h"
#include <GXEngine/GXCameraOrthographic.h>
#include <GXEngine/GXInput.h>
#include <GXEngine/GXSoundMixer.h>
#include <GXEngine/GXSoundChannel.h>


extern TSGBuffer*				ts_GBuffer;
extern TSSceneKeeper*			ts_SceneKeeper;
extern GXCameraOrthographic*	ts_ScreenCamera;
extern GXRenderer*				ts_Renderer;
extern GXPhysics*				ts_Physics;
extern GXInput*					ts_Input;

extern GXSoundMixer*			ts_SoundMixer;
extern GXSoundChannel*			ts_MusicChannel;
extern GXSoundChannel*			ts_EffectChannel;

extern TSPlayerControllerExt*	ts_PlayerController;


GXVoid GXCALL TSOnExit ();


#endif //TS_GLOBALS