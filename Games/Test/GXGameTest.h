//version 1.7

#ifndef GX_GAME_TEST
#define GX_GAME_TEST


//-------------����
#include <GXEngine/GXCore.h>
//-------------����
#include "GXQSkeletalPinkiePie.h"
#include "GXQSkeletalRainbowDash.h"
#include "GXQSkeletalApplejack.h"
#include "GXQSkeletalTwilightSparkle.h"
#include "GXQSkeletalRarity.h"
#include "GXQSkeletalFluttershy.h"

#include "GXStaticMeshGround.h"
#include "GXStaticMeshBall.h"
#include "GXStaticMeshAJHat.h"
//-------------��������
#include <GXEngine/GXAnimSolverMovement.h>
#include <GXEngine/GXAnimSolverPlayer.h>
//-------------�����������
#include "GXMLPRenderQuad.h"
#include <GXEngine/GXTextureOuter.h>
#include <GXEngine/GXDOFEffect.h>
#include <GXEngine/GXMotionBlurEffect.h>
//-------------���
#include <GXEngine/GXHudSurface.h>
//-------------����
#include <GXEngine/GXSoundStorage.h>
//-------------����
#include "GXSkyGlobeSunny.h"
//-------------�������
#include <GXEngine/GXRealParticles.h>
//-------------��������
#include "GXBossTwilightSparkle.h"
#include <GXEngine/GXPhysicsBoss.h>
#include "GXPlayerCameraman.h"
//-------------������
#include <GXEngine/GXCameraPerspective.h>


GXVoid GXCALL OnLevelInit ();
GXVoid GXCALL OnLevelClose ();


#endif	//GX_GAME_TEST