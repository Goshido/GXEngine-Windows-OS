//version 1.7

#ifndef GX_GAME_TEST
#define GX_GAME_TEST


//-------------Ядро
#include <GXEngine/GXCore.h>
//-------------Меши
#include "GXQSkeletalPinkiePie.h"
#include "GXQSkeletalRainbowDash.h"
#include "GXQSkeletalApplejack.h"
#include "GXQSkeletalTwilightSparkle.h"
#include "GXQSkeletalRarity.h"
#include "GXQSkeletalFluttershy.h"

#include "GXStaticMeshGround.h"
#include "GXStaticMeshBall.h"
#include "GXStaticMeshAJHat.h"
//-------------Анимация
#include <GXEngine/GXAnimSolverMovement.h>
#include <GXEngine/GXAnimSolverPlayer.h>
//-------------Постэффекты
#include "GXMLPRenderQuad.h"
#include <GXEngine/GXTextureOuter.h>
#include <GXEngine/GXDOFEffect.h>
#include <GXEngine/GXMotionBlurEffect.h>
//-------------Худ
#include <GXEngine/GXHudSurface.h>
//-------------Звук
#include <GXEngine/GXSoundStorage.h>
//-------------Небо
#include "GXSkyGlobeSunny.h"
//-------------Частицы
#include <GXEngine/GXRealParticles.h>
//-------------Контролы
#include "GXBossTwilightSparkle.h"
#include <GXEngine/GXPhysicsBoss.h>
#include "GXPlayerCameraman.h"
//-------------Камера
#include <GXEngine/GXCameraPerspective.h>


GXVoid GXCALL OnLevelInit ();
GXVoid GXCALL OnLevelClose ();


#endif	//GX_GAME_TEST