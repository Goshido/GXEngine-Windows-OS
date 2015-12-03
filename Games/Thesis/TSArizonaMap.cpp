//version 1.0

#include "TSArizonaMap.h"
#include "TSArizonaPhysics.h"

#include <GXCommon/GXTime.h>

#include "TSGlobals.h"
#include "TSConfig.h"

#include <GXEngine/GXCameraPerspective.h>

#include <GXEngine/GXOGGSoundProvider.h>
#include <GXEngine/GXSoundStorage.h>

#include "TSScene.h"
#include "TSArizonaHud.h"

#include "TSCameraman.h"
#include "TSItem.h"

#include "TSCannonAIController.h"
#include "TSMissileAIController.h"

#include "TSServer.h"
#include "TSClient.h"

#include "TSSkyboxDesertSunny.h"
#include "TSMeshTangent.h"
#include "TSMesh.h"

#include "TSLoading.h"
#include "TSPhysicsObject.h"

#include "TSSkeletalMesh.h"
#include <GXEngine/GXSkeletalMeshStorage.h>
#include <GXEngine/GXAnimSolverPlayerExt.h>

#include <GXEngine/GXVAOStorage.h>


TSScene*				ts_Scene = 0;
TSSkyboxDesertSunny*	ts_Skybox = 0;

TSItem*					ts_AmmoBox[ 5 ] = { 0 };
TSItem*					ts_Wrench[ 4 ] = { 0 };
TSItem*					ts_Missile[ 3 ] = { 0 };

//TSLandCruiser*			ts_Target = 0;
TSMeshTangent*			ts_Tan = 0;

GXSoundEmitter*			ts_Music = 0;

GXCamera*				ts_SceneCamera = 0;
TSCameraman*			ts_Cameraman = 0;

TSSkeletalMesh*			ts_SkeletalMesh = 0;
GXAnimationInfoExt		ts_AnimBattle;
GXAnimSolverPlayerExt*	ts_AnimPlayerFluttershy;

//--------------------------------------------------

GXVoid GXCALL TSOnFire ()
{
	if ( ts_LandCruiserPlayerController )
		ts_LandCruiserPlayerController->FireOrdinary ();
}

GXVoid GXCALL TSOnStopFire ()
{
	if ( ts_LandCruiserPlayerController )
		ts_LandCruiserPlayerController->StopOrdinary ();
}

GXVoid GXCALL TSOnGamepadRightTrigger ( GXFloat value )
{
	if ( value >= 0.5f )
		TSOnFire ();
	else
		TSOnStopFire ();
}

GXVoid GXCALL TSOnSetCannon ()
{
	if ( ts_LandCruiserPlayerController )
	{
		TSCannonAIController* cannon = new TSCannonAIController ();
		ts_LandCruiserPlayerController->SetOrdinaryWeapon ( cannon );
	}
}

GXVoid GXCALL TSOnSetMissile ()
{
	if ( ts_LandCruiserPlayerController )
	{
		TSMissileAIController* missile = new TSMissileAIController ();
		ts_LandCruiserPlayerController->SetOrdinaryWeapon ( missile );
	}
}

//-------------------------------------------------

GXFloat t = 0.0f;

GXDouble ts_Check1;
GXDouble ts_Check2;

GXBool GXCALL TSArizonaOnFrame ( GXFloat delta )
{
	GXVec3 location;
	GXMat4 rotation;
	ts_LandCruiserPlayerController->GetLocation ( location );
	ts_LandCruiserPlayerController->GetRotation ( rotation );

	gx_ActiveCamera = ts_SceneCamera;
	ts_Cameraman->Action ( location, rotation );

	ts_GBuffer->UpdateExposure ( delta );
	ts_GBuffer->ClearGBuffer ();

	ts_GBuffer->StartSkyboxPass ();
	ts_Skybox->Draw ();

	ts_GBuffer->StartCommonPass ();

	ts_Scene->DrawMeshes ();

	ts_Tan->Draw ();
	ts_Tan->SetRotation ( 0.0f, t, 0.0f );

	ts_SkeletalMesh->Draw ();
	ts_SkeletalMesh->SetRotation ( 0.0f, t, 0.0f );
	ts_SkeletalMesh->Update ( delta );

	t += delta * 0.2f;

	TSRenderObjectOnUpdate ( delta );
	TSRenderObjectOnDraw ( TS_COMMON );

	for ( GXUChar i = 0; i < 4; i++ )
		ts_AmmoBox[ i ]->Update ( delta );
	for ( GXUChar i = 0; i < 3; i++ )
		ts_Missile[ i ]->Update ( delta );
	for ( GXUChar i = 0; i < 4; i++ )
		ts_Wrench[ i ]->Update ( delta );

	ts_GBuffer->StartLightUpPass ();
	gx_ActiveCamera = ts_SceneCamera;

	ts_Scene->DrawLights ();
	TSRenderObjectOnDraw ( TS_ALBEDO );

	ts_GBuffer->CombineSlots ();
	ts_GBuffer->AddBloom ();

	ts_GBuffer->StartHudDepthDependentPass ();
	TSRenderObjectOnDraw ( TS_HUD_DEPTH_DEPENDENT );

	ts_GBuffer->StartHudDepthIndependentPass ();

	TSArizonaHudUpdate ( delta );
	
	ts_GBuffer->ShowTexture ( TS_OUT );
	return GX_TRUE;
}

GXVoid GXCALL TSArizonaOnInit ()
{
	ts_Check1 = GXGetProcessorTicks ();

	ts_Input->BindKeyFunc ( &TSStartAccelerate, VK_KEY_W, INPUT_DOWN );
	ts_Input->BindKeyFunc ( &TSStartBreak, VK_KEY_S, INPUT_DOWN );
	ts_Input->BindKeyFunc ( &TSStartTurnLeft, VK_KEY_A, INPUT_DOWN );
	ts_Input->BindKeyFunc ( &TSStartTurnRight, VK_KEY_D, INPUT_DOWN );

	ts_Input->BindKeyFunc ( &TSStopAccelerate, VK_KEY_W, INPUT_UP );
	ts_Input->BindKeyFunc ( &TSStopBreak, VK_KEY_S, INPUT_UP );
	ts_Input->BindKeyFunc ( &TSStopTurnLeft, VK_KEY_A, INPUT_UP );
	ts_Input->BindKeyFunc ( &TSStopTurnRight, VK_KEY_D, INPUT_UP );

	ts_Input->BindKeyFunc ( &TSOnFire, VK_NUMPAD0, INPUT_DOWN );
	ts_Input->BindKeyFunc ( &TSOnStopFire, VK_NUMPAD0, INPUT_UP );

	ts_Input->BindKeyFunc ( &TSOnSetCannon, VK_KEY_1, INPUT_UP );
	ts_Input->BindKeyFunc ( &TSOnSetMissile, VK_KEY_2, INPUT_UP );
	
	ts_Input->BindLeftStickFunc ( &TSLeftStickProc );
	ts_Input->BindRightStickFunc ( &TSRightStickProc );

	ts_Input->BindRightTriggerFunc ( &TSOnGamepadRightTrigger );

	ts_Input->BindGamepadKeyFunc ( &TSOnSetCannon, GX_INPUT_XBOX_SHOULDER_LEFT, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSOnSetMissile, GX_INPUT_XBOX_SHOULDER_RIGHT, INPUT_DOWN );

	TSConfig config;
	TSLoadConfig ( config );

	GXSoundTrack* track = (GXOGGSoundTrack*)GXGetSoundTrack ( L"../Sounds/Thesis/Music/Breaking Benjamin - Blow me away.ogg" );
	ts_Music = new GXSoundEmitter ( track, GX_TRUE, GX_TRUE, GX_TRUE );
	track->Release ();
	ts_MusicChannel->AddEmitter ( ts_Music );
	ts_MusicChannel->SetVolume ( config.musicVolume * 0.01f );
	ts_EffectChannel->SetVolume ( config.effectVolume * 0.01f );

	TSRenderObjectInit ();
	TSPhysicsObjectInit ();
	TSAIControllerInit ();
	
	//

	TSOnLoading ();

	GXFloat aspectRatio = gx_EngineSettings.rendererWidth / (GXFloat)gx_EngineSettings.rendererHeight;
	ts_SceneCamera = new GXCameraPerspective ( GXDegToRad ( 55.0f ), aspectRatio, 0.5f, 5000.0f );
	ts_Cameraman = new TSCameraman ( ts_SceneCamera );
	gx_ActiveCamera = ts_SceneCamera;

	ts_GBuffer->SetObserverCamera ( ts_SceneCamera );

	GXVec3 loc;
	TSArizonaHudInit ();

	ts_Scene = new TSScene ( ts_GBuffer );
	ts_Scene->LoadScene ( L"../Scenes/Demo/Demo.scn" );

	TSDirectedLight* dl = (TSDirectedLight*)ts_Scene->FindLight ( L"l_Sun" );
	dl->SetAmbientColor ( 247.0f / 255.0f, 175.0f / 255.0f, 123.0f / 255.0f );
	dl->SetAmbientIntensity ( 0.07f );

	ts_Skybox = new TSSkyboxDesertSunny ();

	GXMat4 rot;
	GXSetMat4RotationX ( rot, GX_MATH_HALFPI );

	GXSetMat4Identity ( rot );
	GXConvert3DSMaxToGXEngine ( loc, -25.842f, -1.475f, -36.166f );
	ts_LandCruiserPlayerController = new TSLandCruiserPlayerController ( loc, rot );
	ts_LandCruiserPlayerController->SetTarget ( 0 );
	ts_PlayerController = ts_LandCruiserPlayerController;
	

	TSMesh* mesh = (TSMesh*)ts_Scene->FindMesh ( L"r_Ammo_Box001" );
	ts_AmmoBox[ 0 ] = new TSItem ( mesh, 5.0f );
	mesh = (TSMesh*)ts_Scene->FindMesh ( L"r_Ammo_Box002" );
	ts_AmmoBox[ 1 ] = new TSItem ( mesh, 5.0f );
	mesh = (TSMesh*)ts_Scene->FindMesh ( L"r_Ammo_Box003" );
	ts_AmmoBox[ 2 ] = new TSItem ( mesh, 5.0f );
	mesh = (TSMesh*)ts_Scene->FindMesh ( L"r_Ammo_Box004" );
	ts_AmmoBox[ 3 ] = new TSItem ( mesh, 5.0f );

	mesh = (TSMesh*)ts_Scene->FindMesh ( L"r_Wrench001" );
	ts_Wrench[ 0 ] = new TSItem ( mesh, 15.0f );
	mesh = (TSMesh*)ts_Scene->FindMesh ( L"r_Wrench002" );
	ts_Wrench[ 1 ] = new TSItem ( mesh, 15.0f );
	mesh = (TSMesh*)ts_Scene->FindMesh ( L"r_Wrench003" );
	ts_Wrench[ 2 ] = new TSItem ( mesh, 15.0f );
	mesh = (TSMesh*)ts_Scene->FindMesh ( L"r_Wrench004" );
	ts_Wrench[ 3 ] = new TSItem ( mesh, 15.0f );

	mesh = (TSMesh*)ts_Scene->FindMesh ( L"r_Missile001" );
	ts_Missile[ 0 ] = new TSItem ( mesh, 30.0f );
	mesh = (TSMesh*)ts_Scene->FindMesh ( L"r_Missile002" );
	ts_Missile[ 1 ] = new TSItem ( mesh, 30.0f );
	mesh = (TSMesh*)ts_Scene->FindMesh ( L"r_Missile003" );
	ts_Missile[ 2 ] = new TSItem ( mesh, 30.0f );

	GXVec3 v;
	GXConvert3DSMaxToGXEngine ( v, -57.0f, -25.0f , -40.0f );

	ts_SkeletalMesh = new TSSkeletalMesh ( L"../3D Models/Thesis/Book.skm", L"../Materials/Thesis/Fluttershy.mtr" );
	ts_SkeletalMesh->SetLocation ( v );
	ts_SkeletalMesh->SetScale ( 0.01f, 0.01f, 0.01f );

	ts_Tan = new TSMeshTangent ( L"../3D Models/Thesis/Pot.stm", L"../Materials/Thesis/Brick_Wall.mtr", GX_FALSE );
	//ts_Tan = new TSMeshTangent ( L"../3D Models/Thesis/1.stm", L"../Materials/Thesis/Brick_Wall.mtr", GX_FALSE );
	//ts_Tan->SetScale ( 0.01f, 0.01f, 0.01f );
	v.x += 10.0f;
	ts_Tan->SetLocation ( v );
	
	//GXGetAnimation ( "../Animations/Thesis/Battle.ani", ts_AnimBattle );
	GXGetAnimation ( L"../Animations/Thesis/Book.ani", ts_AnimBattle );

	ts_AnimPlayerFluttershy = new GXAnimSolverPlayerExt ( 0 );
	ts_AnimPlayerFluttershy->SetAnimationMultiplier ( 0.2f );
	ts_AnimPlayerFluttershy->SetAnimationSequence ( &ts_AnimBattle );

	ts_SkeletalMesh->SetAnimSolver ( ts_AnimPlayerFluttershy );

	ts_light_CascadeShadowmap->IncludeShadowCaster ( ts_Tan );

	glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

	ts_Music->Play ();

	GXDouble ts_Check2 = GXGetProcessorTicks ();
	GXDouble initTime = ( ts_Check2 - ts_Check1 ) / GXGetProcessorTicksPerSecond ();
	GXLogW ( L"Время загрузки %f\n", (GXFloat)initTime );
}

GXVoid GXCALL TSArizonaOnDelete ()
{
	ts_Input->UnBindKeyFunc ( VK_KEY_W, INPUT_DOWN );
	ts_Input->UnBindKeyFunc ( VK_KEY_S, INPUT_DOWN );
	ts_Input->UnBindKeyFunc ( VK_KEY_A, INPUT_DOWN );
	ts_Input->UnBindKeyFunc ( VK_KEY_D, INPUT_DOWN );

	ts_Input->UnBindKeyFunc ( VK_KEY_W, INPUT_UP );
	ts_Input->UnBindKeyFunc ( VK_KEY_S, INPUT_UP );
	ts_Input->UnBindKeyFunc ( VK_KEY_A, INPUT_UP );
	ts_Input->UnBindKeyFunc ( VK_KEY_D, INPUT_UP );

	ts_Input->UnBindKeyFunc ( VK_NUMPAD0, INPUT_DOWN );
	ts_Input->UnBindKeyFunc ( VK_NUMPAD0, INPUT_UP );

	ts_Input->UnBindKeyFunc ( VK_KEY_1, INPUT_UP );
	ts_Input->UnBindKeyFunc ( VK_KEY_2, INPUT_UP );

	ts_Input->UnBindLeftStickFunc ();
	ts_Input->UnBindRightStickFunc ();

	ts_Input->UnBindRightTriggerFunc ();

	ts_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_SHOULDER_LEFT, INPUT_DOWN );
	ts_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_SHOULDER_RIGHT, INPUT_DOWN );

	delete ts_Music;

	ts_light_CascadeShadowmap->ExcludeShadowCaster ( ts_Tan );

	ts_Tan->Delete ();
	ts_Tan->Draw ();
	ts_Tan = 0;

	for ( GXUChar i = 0; i < 5; i++ )
		GXSafeDelete ( ts_AmmoBox[ i ] );
	for ( GXUChar i = 0; i < 3; i++ )
		GXSafeDelete ( ts_Missile[ i ] );
	for ( GXUChar i = 0; i < 4; i++ )
		GXSafeDelete ( ts_Wrench[ i ] );

	ts_SkeletalMesh->Delete ();
	ts_SkeletalMesh->Draw ();

	GXSafeDelete ( ts_AnimPlayerFluttershy );
	GXRemoveAnimation ( ts_AnimBattle );

	ts_PlayerController = 0;
	GXSafeDelete ( ts_LandCruiserPlayerController );

	TSRenderObjectDestroy ();

	TSArizonaHudDestroy ();

	GXSafeDelete ( ts_Skybox );
	GXSafeDelete ( ts_Scene );

	GXSafeDelete ( ts_Cameraman );
	GXSafeDelete ( ts_SceneCamera );

	TSOnUnLoading ();

	//

	TSAIControllerDestroy ();
	TSPhysicsObjectDestroy ();

	TSClientDestroy ();
	TSServerDestroy ();
}

GXVoid GXCALL TSArizonaOnPhysics ( GXFloat delta )
{
	TSOnPhysics ( delta );
}