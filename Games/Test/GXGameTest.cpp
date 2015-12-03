//version 1.7

#include "GXGameTest.h"


GXVec4							gx_game_test_ClearColor;
GXRenderer*						gx_game_test_Renderer = 0;
GXInput*						gx_game_test_Input = 0;
GXSoundMixer*					gx_game_test_SoundMixer = 0;

GXCameraPerspective*			gx_game_test_SceneCamera = 0;

GXSoundChannel*					gx_game_test_AmbientSoundChannel = 0;
GXOGGSoundTrack*				gx_game_test_AmbientMusic = 0;
GXSoundEmitter*					gx_game_test_MusicEmitter = 0;

GXPlayerBoss*					gx_game_test_ctrCurrent = 0;
	
GXBossTwilightSparkle*			gx_game_test_ctrTwilightSparkleExt = 0;

GXPhysicsBoss*					gx_game_test_physActor = 0;

PxMaterial*						gx_game_test_GroundMaterial = 0;

GXPlayerCameraman*				gx_game_test_ctrCamera = 0;
GXMLPRenderQuad*				gx_game_test_MLPQuad = 0;

GXSkeletalMeshAnimationInfo*	gx_game_test_skaiBattle_Stance = 0;

GXQSkeletalPinkiePie*			gx_game_test_skmPinkiePie = 0;
GXAnimSolverPlayer*				gx_game_test_aspPinkiePie = 0;

GXQSkeletalRarity*				gx_game_test_skmRarity = 0;
GXAnimSolverPlayer*				gx_game_test_aspRarity = 0;

GXQSkeletalFluttershy*			gx_game_test_skmFluttershy = 0;
GXAnimSolverPlayer*				gx_game_test_aspFluttershy = 0;

GXQSkeletalRainbowDash*			gx_game_test_skmRainbowDash = 0;
GXAnimSolverPlayer*				gx_game_test_aspRainbowDash = 0;

GXQSkeletalApplejack*			gx_game_test_skmApplejack = 0;
GXAnimSolverPlayer*				gx_game_test_aspApplejack = 0;

GXQSkeletalTwilightSparkle*		gx_game_test_skmTwilightSparkle = 0;
GXSkeletalMeshAnimationInfo*	gx_game_test_skaiTwilightSparkle[ 3 ] = { 0 };
GXAnimSolverPlayer*				gx_game_test_aspTwilightSparkle[ 3 ] = { 0 };
GXAnimSolverMovement*			gx_game_test_asmTwilightSparkle = 0;

GXStaticMeshGround*				gx_game_test_stmGround = 0;
GXStaticMeshBall*				gx_game_test_stmBall = 0;
GXStaticMeshAJHat*				gx_game_test_stmAJ_Hat = 0;

GXInt							gx_game_test_iWinCursorPosX;
GXInt							gx_game_test_iWinCursorPosY;
EGXInputMouseFlags				gx_game_test_mouseFlags;

GXSkyGlobeSunny*				gx_game_test_Sky = 0;

GXVec3							gx_game_test_CameraRotOffset;
GXFloat							gx_game_test_MouseRotationFactor;

GXTextureOuter*					gx_game_test_TextureOuter = 0;
GXDoFEffect*					gx_game_test_DofEffect = 0;
GXMotionBlurEffect*				gx_game_test_MotionBlurEffect = 0;

GXFloat							gx_game_test_fDoFFocus = 150.0f;
GXFloat							gx_game_test_fDoFRange = 150.0f;
GXFloat							gx_game_test_fDoFBlurness = 0.9f;

GXFloat							gx_game_test_Fov = 60.0f;

GXBool							gx_game_test_PostprocessingEffects[ GX_RENDERER_SUPPORTED_POSTEFFECTS ] = { GX_FALSE }; // Индексы: 0 - Motion Blur, 1 - DoF
GXBool							gx_game_test_bNeedUpdateLastCursor = GX_FALSE;

GXRealParticles*				gx_game_test_ParticlesEmitter = 0;

GXHudSurface*					gx_game_test_HudSurface = 0;
GXMaterialInfo					gx_game_test_HudBackground;

FT_Face							gx_game_test_Font = 0;

//----------------------------------------------------------------------------------------------------

GXVoid GXCALL WalkForward ();
GXVoid GXCALL WalkBackward ();
GXVoid GXCALL StopWalk ();
GXVoid GXCALL RotateLeft ();
GXVoid GXCALL RotateRight ();
GXVoid GXCALL StopRotate ();
GXVoid GXCALL StrafeLeft ();
GXVoid GXCALL StrafeRight ();
GXVoid GXCALL StopStrafe ();
GXVoid GXCALL Run ();
GXVoid GXCALL StopRun ();
GXVoid GXCALL Jump ();
GXVoid GXCALL StickMoving ( GXFloat x, GXFloat y );
GXVoid GXCALL StickRotating ( GXFloat x, GXFloat y );

GXVoid GXCALL MouseMoveFunc ( GXInt win_x, GXInt win_y );
GXVoid GXCALL MouseButtonsFunc ( EGXInputMouseFlags mouseFlags );
GXVoid GXCALL MouseWheelFunc ( GXInt steps );

GXBool GXCALL OnFrame ( GXFloat deltatime );
GXVoid GXCALL OnInitRenderableObjects ();
GXVoid GXCALL OnDeleteRenderableObjects ();

GXVoid GXCALL OnPhysics ( GXFloat deltatime );

//----------------------------------------------------------------------------------------------------

GXVoid GXCALL ShutDownAll ()
{
	gx_Core->Exit ();
	GXLogW ( L"Завершение\n" );
}

//---------------------------------------------------------------------------------------

GXVoid GXCALL IncreaseFov ()
{
	if ( gx_game_test_Fov > 170.0f ) return;
	gx_game_test_Fov += 5.0f;
	gx_game_test_SceneCamera->SetFov ( GXDegToRad ( gx_game_test_Fov ) );
}

GXVoid GXCALL DecreaseFov ()
{
	if ( gx_game_test_Fov < 10.0f ) return;
	gx_game_test_Fov -= 5.0f;
	gx_game_test_SceneCamera->SetFov ( GXDegToRad ( gx_game_test_Fov ) );
}

GXVoid GXCALL ToggleDoF ()
{
	gx_game_test_PostprocessingEffects[ 1 ] = !gx_game_test_PostprocessingEffects[ 1 ];
}

//----------------------------------------------------------

GXVoid GXCALL OnLevelInit ()
{
	GXLogW ( L"Управление:\nWASD - управление\nSHIFT - бег\ne - поворот направо\nq - поворот налево\n8 - включить DOF\nколёсико - изменить фокусное расстояние\n0 - уменьшить угол обзора\n9 - увеличить угол обзора\nesc - выход\n\n\n" );	
	gx_game_test_Renderer = gx_Core->GetRenderer ();
	
	gx_game_test_CameraRotOffset.pitch_rad = gx_game_test_CameraRotOffset.yaw_rad = gx_game_test_CameraRotOffset.roll_rad = 0.0f;
 
	gx_game_test_SceneCamera = new GXCameraPerspective ( GXDegToRad ( gx_game_test_Fov ), gx_EngineSettings.rendererWidth / (GXFloat)gx_EngineSettings.rendererHeight, 0.1f, 1000.0f );

	gx_ActiveCamera = gx_game_test_SceneCamera;

	gx_game_test_ctrCamera = new GXPlayerCameraman ( gx_ActiveCamera );

	gx_game_test_PostprocessingEffects [ 0 ] = gx_EngineSettings.motionBlur;
	gx_game_test_PostprocessingEffects [ 1 ] = gx_EngineSettings.dof;
	
	//Управление персонажами
	GXInput* gx_game_test_Input = gx_Core->GetInput ();

	gx_game_test_Input->BindKeyFunc ( &WalkForward, VK_KEY_W, INPUT_DOWN );
	gx_game_test_Input->BindKeyFunc ( &StopWalk, VK_KEY_W, INPUT_UP );

	gx_game_test_Input->BindKeyFunc ( &WalkBackward, VK_KEY_S, INPUT_DOWN );
	gx_game_test_Input->BindKeyFunc ( &StopWalk, VK_KEY_S, INPUT_UP );

	gx_game_test_Input->BindKeyFunc ( &RotateLeft, VK_KEY_Q, INPUT_DOWN );
	gx_game_test_Input->BindKeyFunc ( &StopRotate, VK_KEY_Q, INPUT_UP );

	gx_game_test_Input->BindKeyFunc ( &RotateRight, VK_KEY_E, INPUT_DOWN );
	gx_game_test_Input->BindKeyFunc ( &StopRotate, VK_KEY_E, INPUT_UP );

	gx_game_test_Input->BindKeyFunc ( &StrafeRight, VK_KEY_D, INPUT_DOWN );
	gx_game_test_Input->BindKeyFunc ( &StopStrafe, VK_KEY_D, INPUT_UP );

	gx_game_test_Input->BindKeyFunc ( &StrafeLeft, VK_KEY_A, INPUT_DOWN );
	gx_game_test_Input->BindKeyFunc ( &StopStrafe, VK_KEY_A, INPUT_UP );

	gx_game_test_Input->BindKeyFunc ( &Run, VK_SHIFT, INPUT_DOWN );
	gx_game_test_Input->BindKeyFunc ( &StopRun, VK_SHIFT, INPUT_UP );

	gx_game_test_Input->BindKeyFunc ( &Jump, VK_SPACE, INPUT_DOWN );

	//Кнопки
	gx_game_test_Input->BindGamepadKeyFunc ( &Run, GX_INPUT_XBOX_SHOULDER_LEFT, INPUT_DOWN );
	gx_game_test_Input->BindGamepadKeyFunc ( &StopRun, GX_INPUT_XBOX_SHOULDER_LEFT, INPUT_UP );
	gx_game_test_Input->BindGamepadKeyFunc ( &Jump, GX_INPUT_XBOX_A, INPUT_DOWN );

	//Аналоги
	gx_game_test_Input->BindLeftStickFunc ( &StickMoving );
	gx_game_test_Input->BindRightStickFunc ( &StickRotating );

	//Параметры изображения
	gx_game_test_Input->BindKeyFunc ( &IncreaseFov, VK_KEY_9, INPUT_DOWN );
	gx_game_test_Input->BindKeyFunc ( &DecreaseFov, VK_KEY_0, INPUT_DOWN );
	gx_game_test_Input->BindKeyFunc ( &ToggleDoF, VK_KEY_8, INPUT_UP );


	//Отключение
	gx_game_test_Input->BindKeyFunc ( &ShutDownAll, VK_ESCAPE, INPUT_DOWN );

	//Мышь
	gx_game_test_Input->BindMouseMoveFunc ( &MouseMoveFunc );
	gx_game_test_Input->BindMouseButtonsFunc ( &MouseButtonsFunc );
	gx_game_test_Input->BindMouseWheelFunc ( &MouseWheelFunc );
	
	GXPhysics* physics = gx_Core->GetPhysics ();
	gx_game_test_physActor = new GXPhysicsBoss ();
	PxRigidDynamic* rigidDinamic = physics->CreateRigidDynamic ( gx_game_test_physActor, PxTransform ( PxVec3 ( 10.0f, 0.0f, 10.0f ) ) );
	rigidDinamic->setAngularVelocity ( PxVec3 ( -3.0f, 0.1f, 6.0f ) );
	PxMaterial* mat = physics->CreateMaterial ( 0.5f, 0.5f, 0.3f );
	rigidDinamic->createShape ( PxSphereGeometry ( 2.3f ), *mat );
	physics->AddActor ( *rigidDinamic );
	rigidDinamic->addForce ( PxVec3 ( 0.0f, 70.0f, 0.0f ), PxForceMode::eIMPULSE );

	gx_game_test_GroundMaterial = physics->CreateMaterial ( 0.5f, 0.5f, 0.1f );

	PxTransform trs ( PxVec3 ( 0.0f, 0.0f, 0.0f ) );
	PxRigidStatic* ground = physics->CreateRigidStatic ( 0, trs );
	PxTriangleMesh* trm = physics->CreateTriangleMesh ( L"../Physics/Test/Triangle Meshes/Ground.bke" );
	ground->createShape ( PxTriangleMeshGeometry ( trm ), *gx_game_test_GroundMaterial );
	physics->AddActor ( *ground );
	
	gx_game_test_ctrTwilightSparkleExt = new GXBossTwilightSparkle ();
	gx_game_test_ctrTwilightSparkleExt->SetLocation ( 10.0f, 0.0f, 0.0f );
	gx_game_test_ctrTwilightSparkleExt->SetRotation ( 0.0f, 0.0f, 0.0f );

	gx_game_test_ctrCurrent = gx_game_test_ctrTwilightSparkleExt;

	physics->SetOnPhysicsFunc ( OnPhysics );

	gx_game_test_Renderer->SetOnFrameFunc ( &OnFrame );
	gx_game_test_Renderer->SetOnInitRenderableObjectsFunc ( &OnInitRenderableObjects );
	gx_game_test_Renderer->SetOnDeleteRenderableObjectsFunc ( &OnDeleteRenderableObjects );

	gx_game_test_SoundMixer = gx_Core->GetSoundMixer ();

	gx_game_test_AmbientSoundChannel = new GXSoundChannel ();
	gx_game_test_AmbientMusic = (GXOGGSoundTrack*)GXGetSoundTrack ( L"../Sounds/Test/Music/Sonic-Generations-Title-Screen.ogg" );
	gx_game_test_MusicEmitter = new GXSoundEmitter ( gx_game_test_AmbientMusic, GX_TRUE, GX_TRUE, GX_TRUE );
	gx_game_test_AmbientSoundChannel->AddEmitter ( gx_game_test_MusicEmitter );
	gx_game_test_SoundMixer->AddChannel ( gx_game_test_AmbientSoundChannel );

	gx_game_test_Font = GXGetFont ( L"../Fonts/trebuc.ttf" );
}

GXVoid GXCALL OnLevelClose ()
{
	GXRemoveFont ( gx_game_test_Font );

	GXSafeDelete ( gx_game_test_AmbientSoundChannel )

	GXSafeDelete ( gx_game_test_physActor );

	gx_game_test_AmbientMusic->Release ();

	//Управление персонажами
	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_W, INPUT_DOWN );
	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_W, INPUT_UP );

	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_S, INPUT_DOWN );
	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_S, INPUT_UP );

	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_Q, INPUT_DOWN );
	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_Q, INPUT_UP );

	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_E, INPUT_DOWN );
	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_E, INPUT_UP );

	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_D, INPUT_DOWN );
	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_D, INPUT_UP );

	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_A, INPUT_DOWN );
	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_A, INPUT_UP );

	gx_game_test_Input->UnBindKeyFunc ( VK_SHIFT, INPUT_DOWN );
	gx_game_test_Input->UnBindKeyFunc ( VK_SHIFT, INPUT_UP );

	gx_game_test_Input->UnBindKeyFunc ( VK_SPACE, INPUT_DOWN );

	//Кнопки
	gx_game_test_Input->UnBindGamepadKeyFunc (GX_INPUT_XBOX_SHOULDER_LEFT, INPUT_DOWN );
	gx_game_test_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_SHOULDER_LEFT, INPUT_UP );
	gx_game_test_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_A, INPUT_DOWN );

	//Аналоги
	gx_game_test_Input->UnBindLeftStickFunc ();
	gx_game_test_Input->UnBindRightStickFunc ();

	//Внешний вид
	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_3, INPUT_DOWN );

	//Параметры изображения
	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_9, INPUT_DOWN );
	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_0, INPUT_DOWN );
	gx_game_test_Input->UnBindKeyFunc ( VK_KEY_8, INPUT_UP );

	//Отключение
	gx_game_test_Input->UnBindKeyFunc ( VK_ESCAPE, INPUT_DOWN );

	//Мышь
	gx_game_test_Input->UnBindMouseMoveFunc ();
	gx_game_test_Input->UnBindMouseButtonsFunc ();
	gx_game_test_Input->UnBindMouseWheelFunc ();

	GXSafeDelete ( gx_game_test_ctrTwilightSparkleExt );
	GXSafeDelete ( gx_game_test_ctrCamera );
	GXSafeDelete ( gx_game_test_SceneCamera );

	gx_game_test_GroundMaterial->release ();
}

//------------------------------------------------------------------

GXUInt gx_game_test_lastFPS = 0xFFFFFFFF;

GXMat4 gx_game_test_Rot;
GXMat4 gx_game_test_RelativeRot;
GXMat4 gx_game_test_ResultRot;

GXVoid GXCALL UpdateCommonDependent ( GXDword deltatime )
{
	gx_game_test_MLPQuad->StartCommonPass ();
	
	gx_game_test_Sky->Draw ();
	gx_game_test_stmGround->Draw ();
	gx_game_test_stmBall->Draw ();
}

GXVoid GXCALL UpdateHUDDependent ( GXDword deltatime )
{
	GXUInt temp = gx_game_test_Renderer->GetCurrentFPS ();
	if ( temp != gx_game_test_lastFPS ) 
	{
		gx_game_test_lastFPS = temp;

		gx_game_test_HudSurface->Reset ();

		#define GX_SIZE	60

		GXGLTextureStruct ts;
		ts.internalFormat = gx_game_test_HudBackground.textures[ 0 ].bIsAlpha ? GL_RGBA8 : GL_RGB;
		ts.width = gx_game_test_HudBackground.textures[ 0 ].usWidth;
		ts.height = gx_game_test_HudBackground.textures[ 0 ].usHeight;

		GXImageInfo imageInfo;
		imageInfo.insertX = 0;
		imageInfo.insertY = 0;
		imageInfo.insertWidth = gx_game_test_HudBackground.textures[ 0 ].usWidth;
		imageInfo.insertHeight = gx_game_test_HudBackground.textures[ 0 ].usHeight;
		imageInfo.overlayType = GX_SIMPLE_REPLACE;
		imageInfo.texture = &ts;
		imageInfo.textureID = gx_game_test_HudBackground.textures[ 0 ].uiId;

		gx_game_test_HudSurface->AddImage ( imageInfo );

		imageInfo.insertX = 50;
		imageInfo.insertY = 180;
		GXFloat aspect = 580.0f / 267.0f;
		imageInfo.insertWidth = (GXInt)( GX_SIZE * aspect );
		imageInfo.insertHeight = GX_SIZE;
		imageInfo.overlayType = GX_ALPHA_MULTIPLY;
		ts.internalFormat = gx_game_test_HudBackground.textures[ 1 ].bIsAlpha ? GL_RGBA8 : GL_RGB;
		ts.width = gx_game_test_HudBackground.textures[ 1 ].usWidth;
		ts.height = gx_game_test_HudBackground.textures[ 1 ].usHeight;
		imageInfo.textureID = gx_game_test_HudBackground.textures[ 1 ].uiId;

		gx_game_test_HudSurface->AddImage ( imageInfo );

		imageInfo.insertX = 200;
		imageInfo.insertY = 180;
		aspect = 400.0f / 142.0f;
		imageInfo.insertWidth = (GXInt)( GX_SIZE * aspect );
		imageInfo.overlayType = GX_ALPHA_TRANSPARENCY;
		ts.internalFormat = gx_game_test_HudBackground.textures[ 2 ].bIsAlpha ? GL_RGBA8 : GL_RGB;
		ts.width = gx_game_test_HudBackground.textures[ 2 ].usWidth;
		ts.height = gx_game_test_HudBackground.textures[ 2 ].usHeight;
		imageInfo.textureID = gx_game_test_HudBackground.textures[ 2 ].uiId;

		gx_game_test_HudSurface->AddImage ( imageInfo );

		imageInfo.insertX = 390;
		imageInfo.insertY = 180;
		aspect = 491.0f / 266.0f;
		imageInfo.insertWidth = (GXInt)( GX_SIZE * aspect );
		imageInfo.overlayType = GX_ALPHA_TRANSPARENCY;
		ts.internalFormat = gx_game_test_HudBackground.textures[ 3 ].bIsAlpha ? GL_RGBA8 : GL_RGB;
		ts.width = gx_game_test_HudBackground.textures[ 3 ].usWidth;
		ts.height = gx_game_test_HudBackground.textures[ 3 ].usHeight;
		imageInfo.textureID = gx_game_test_HudBackground.textures[ 3 ].uiId;

		gx_game_test_HudSurface->AddImage ( imageInfo );

		#undef GX_SIZE
		
		GXVec4 fontColorGreen;
		GXVec4 fontColorWhite;
		GXColorToVec4 ( fontColorGreen, 115, 185, 0, 255 );
		GXColorToVec4 ( fontColorWhite, 255, 255, 255, 255 );
		GXPenInfo info;

		info.fontColor = &fontColorGreen;
		info.font = gx_game_test_Font;
		info.fontSize = 30;
		info.insertX = 80;
		info.insertY = 130;
		info.kerning = GX_TRUE;

		info.insertX = gx_game_test_HudSurface->AddText ( info, L"DevMode" ) + 102;
		info.fontColor = &fontColorWhite;
		gx_game_test_HudSurface->AddText ( info, L"On" );

		info.fontColor = &fontColorGreen;
		info.insertX = 80;
		info.insertY = 90;
		info.insertX = gx_game_test_HudSurface->AddText ( info, L"Пользователь" ) + 10;
		info.fontColor = &fontColorWhite;
		gx_game_test_HudSurface->AddText ( info, L"Goshido" );

		info.fontColor = &fontColorGreen;
		info.insertX = 80;
		info.insertY = 50;
		info.insertX = gx_game_test_HudSurface->AddText ( info, L"Текущий FPS" ) + 25;
		info.fontColor = &fontColorWhite;
		gx_game_test_HudSurface->AddText ( info, L"%i", gx_game_test_lastFPS );
	}

	gx_game_test_skmTwilightSparkle->GetRotation ( gx_game_test_Rot );

	GXMulMat4Mat4 ( gx_game_test_ResultRot, gx_game_test_RelativeRot, gx_game_test_Rot );

	gx_game_test_HudSurface->SetRotation ( gx_game_test_ResultRot );

	GXVec3 delta ( -8.0f, 4.0f, -3.0f );
	GXVec3 world;

	GXQuat q = GXCreateQuat ( gx_game_test_Rot );
	GXQuatTransform ( world, q, delta );

	GXVec3 loc;
	gx_game_test_skmTwilightSparkle->GetLocation ( loc );

	gx_game_test_HudSurface->SetLocation ( loc.x + world.x, loc.y + world.y, loc.z + world.z );
}

GXVoid GXCALL UpdateFigurationDependent ( GXDword deltatime )
{
	gx_game_test_MLPQuad->StartFigurationPass ();

	gx_game_test_skmRarity->Update ( deltatime );
	gx_game_test_skmRarity->Draw ();

	gx_game_test_skmFluttershy->Update ( deltatime );
	gx_game_test_skmFluttershy->Draw ();

	gx_game_test_skmRainbowDash->Update ( deltatime );
	gx_game_test_skmRainbowDash->Draw ();

	gx_game_test_skmApplejack->Update ( deltatime );
	gx_game_test_skmApplejack->Draw ();

	gx_game_test_skmTwilightSparkle->Update ( deltatime );
	gx_game_test_skmTwilightSparkle->Draw ();

	gx_game_test_skmPinkiePie->Update ( deltatime );
	gx_game_test_skmPinkiePie->Draw ();

	GXVec3 bufBoneLoc;
	GXQuat bufBoneRot;

	gx_game_test_skmApplejack->GetBoneGlobalPlace ( bufBoneLoc, bufBoneRot, "b_AJ_Hat                       " );
	gx_game_test_stmAJ_Hat->SetLocation ( bufBoneLoc.v );
	gx_game_test_stmAJ_Hat->SetRotation ( bufBoneRot );
	gx_game_test_stmAJ_Hat->Draw ();
	
	gx_game_test_MLPQuad->CombineLayers ();
}

GXVoid GXCALL UpdateTransparencyDependent ( GXDword deltatime )
{
	GXVec3 bufBoneLoc;
	GXQuat bufBoneRot;

	gx_game_test_MLPQuad->StartTransparencyPass ();

	GXVec3 offset ( -1.5f, 3.8f, 0.0f );
	gx_game_test_skmTwilightSparkle->GetAttacmentGlobalLocation ( bufBoneLoc, offset, "b_Skull                        " );
	gx_game_test_ParticlesEmitter->SetLocation ( bufBoneLoc.v );
	gx_game_test_ParticlesEmitter->Simulate ( deltatime );
	gx_game_test_ParticlesEmitter->Draw ();

	gx_game_test_HudSurface->Draw ();
}

GXBool GXCALL UpdatePostEffects ( GXDword deltatime )
{
	GXVec3 dofParams;
	gx_game_test_DofEffect->GetDoFDepthParams ( dofParams );

	gx_game_test_MLPQuad->SetDofParams ( dofParams );
	gx_game_test_MLPQuad->Draw ();

	GLuint preoutTexture = gx_game_test_MLPQuad->GetDiffuseTexture ();
	GLuint depthTexture = gx_game_test_MLPQuad->GetDepthTexture ();

	GXChar currentPostprocessEffectIndex = 0;
	GXChar lastPostprocessEffectIndex = GX_RENDERER_SUPPORTED_POSTEFFECTS - 1;
	while ( lastPostprocessEffectIndex >= 0 )
	{
		if ( gx_game_test_PostprocessingEffects[ lastPostprocessEffectIndex ] )	break;
		lastPostprocessEffectIndex--;
	}
	for ( ; lastPostprocessEffectIndex >= 0 && !gx_game_test_PostprocessingEffects[ lastPostprocessEffectIndex ]; lastPostprocessEffectIndex-- );
	if ( lastPostprocessEffectIndex < 0 )
	{
		gx_game_test_TextureOuter->SetTexture ( preoutTexture );
		gx_game_test_TextureOuter->Draw ();
		return GX_TRUE;
	}
	GLuint motionBlurTexture;
	if ( gx_game_test_PostprocessingEffects[ currentPostprocessEffectIndex ] )
	{
		GXPointer m [] = { (GXPointer)&preoutTexture, (GXPointer)&depthTexture };
		if ( currentPostprocessEffectIndex == lastPostprocessEffectIndex )
		{
			gx_game_test_MotionBlurEffect->SetTextures ( preoutTexture, depthTexture );
			gx_game_test_MotionBlurEffect->Draw ();

			motionBlurTexture = gx_game_test_MotionBlurEffect->GetOutTexture ();

			gx_game_test_TextureOuter->SetTexture ( motionBlurTexture );
			gx_game_test_TextureOuter->Draw ();
		}
		else
		{
			gx_game_test_MotionBlurEffect->SetTextures ( preoutTexture, depthTexture );
			gx_game_test_MotionBlurEffect->Draw ();

			motionBlurTexture = gx_game_test_MotionBlurEffect->GetOutTexture ();
			preoutTexture = motionBlurTexture;
		}
	}
	currentPostprocessEffectIndex++;
	if ( gx_game_test_PostprocessingEffects [ currentPostprocessEffectIndex ] )
	{
		gx_game_test_DofEffect->SetPreoutTexture ( preoutTexture );
		gx_game_test_DofEffect->Draw ();
		GLuint dofTexture = gx_game_test_DofEffect->GetOutTexture ();
		gx_game_test_TextureOuter->SetTexture ( dofTexture );
		gx_game_test_TextureOuter->Draw ();
	}

	return GX_TRUE;
}

GXBool GXCALL OnFrame ( GXFloat deltatime )
{	
	GXVec3 loc;
	GXVec3 rot;
	gx_game_test_ctrCurrent->GetLocation ( &loc );
	gx_game_test_ctrCurrent->GetRotation ( &rot );
	if ( gx_game_test_mouseFlags.rmb )
	{
		rot.yaw_rad -= gx_game_test_MouseRotationFactor;
		gx_game_test_ctrCurrent->SetRotation ( rot.pitch_rad, rot.yaw_rad, rot.roll_rad );
		gx_game_test_MouseRotationFactor = 0.0f;
	}

	GXPointer cam_arg [] = { (GXPointer)&gx_game_test_mouseFlags, (GXPointer)&gx_game_test_CameraRotOffset };
	gx_game_test_ctrCamera->Action ( loc, rot, cam_arg );
	
	gx_game_test_SoundMixer->SetListenerLocation ( loc );
	gx_game_test_SoundMixer->SetListenerRotation ( rot );

	gx_game_test_MLPQuad->ClearAllBuffers ();

	glViewport ( 0, 0, gx_EngineSettings.potWidth, gx_EngineSettings.potHeight );

	UpdateCommonDependent ( (GXDword)( deltatime * 1000 ) );
	UpdateHUDDependent ( (GXDword)( deltatime * 1000 ) );
	UpdateFigurationDependent ( (GXDword)( deltatime * 1000 ) );
	UpdateTransparencyDependent ( (GXDword)( deltatime * 1000 ) );

	return UpdatePostEffects ( (GXDword)( deltatime * 1000 ) );
}

GXVoid GXCALL OnInitRenderableObjects ()
{
	gx_game_test_skaiBattle_Stance = new GXSkeletalMeshAnimationInfo;
	GXLoadPSA ( L"../Animations/Test/Default_Pony_Anim_Battle_Stance.psa", gx_game_test_skaiBattle_Stance );

	gx_game_test_skmPinkiePie = new GXQSkeletalPinkiePie ();
	gx_game_test_skmPinkiePie->SetLocation ( 10.0f, 0.0f, -20.0f );

	gx_game_test_aspPinkiePie = new GXAnimSolverPlayer ( 0 );
	gx_game_test_aspPinkiePie->SetAnimationMultiplier ( 1.0f );
	gx_game_test_aspPinkiePie->SetAnimationSequence ( gx_game_test_skaiBattle_Stance );

	gx_game_test_skmPinkiePie->SetAnimSolver ( gx_game_test_aspPinkiePie );


	gx_game_test_skmRarity = new GXQSkeletalRarity ();
	gx_game_test_skmRarity->SetLocation ( 30.0f, 0.0f, -20.0f );

	gx_game_test_aspRarity = new GXAnimSolverPlayer ( 0 );
	gx_game_test_aspRarity->SetAnimationMultiplier ( 1.0f );
	gx_game_test_aspRarity->SetAnimationSequence ( gx_game_test_skaiBattle_Stance );

	gx_game_test_skmRarity->SetAnimSolver ( gx_game_test_aspRarity );


	gx_game_test_skmFluttershy = new GXQSkeletalFluttershy ();
	gx_game_test_skmFluttershy->SetLocation ( 30.0f, 0.0f, -30.0f );
	gx_game_test_skmFluttershy->SetRotation ( 0.0f, GXDegToRad ( 180.0f ), 0.0f );

	gx_game_test_aspFluttershy = new GXAnimSolverPlayer ( 0 );
	gx_game_test_aspFluttershy->SetAnimationMultiplier ( 1.0f );
	gx_game_test_aspFluttershy->SetAnimationSequence ( gx_game_test_skaiBattle_Stance );

	gx_game_test_skmFluttershy->SetAnimSolver ( gx_game_test_aspFluttershy );


	gx_game_test_skmRainbowDash = new GXQSkeletalRainbowDash ();
	gx_game_test_skmRainbowDash->SetLocation ( 10.0f, 0.0f, -30.0f );
	gx_game_test_skmRainbowDash->SetRotation ( 0.0f, GXDegToRad ( 180.0f ), 0.0f );

	gx_game_test_aspRainbowDash = new GXAnimSolverPlayer ( 0 );
	gx_game_test_aspRainbowDash->SetAnimationMultiplier ( 1.0f );
	gx_game_test_aspRainbowDash->SetAnimationSequence ( gx_game_test_skaiBattle_Stance );

	gx_game_test_skmRainbowDash->SetAnimSolver ( gx_game_test_aspRainbowDash );


	gx_game_test_skmApplejack = new GXQSkeletalApplejack ();
	gx_game_test_skmApplejack->SetLocation ( -10.0f, 0.0f, -30.0f );
	gx_game_test_skmApplejack->SetRotation ( 0.0f, GXDegToRad ( 180.0f ), 0.0f );

	gx_game_test_aspApplejack = new GXAnimSolverPlayer ( 0 );
	gx_game_test_aspApplejack->SetAnimationMultiplier ( 1.0f );
	gx_game_test_aspApplejack->SetAnimationSequence ( gx_game_test_skaiBattle_Stance );

	gx_game_test_skmApplejack->SetAnimSolver ( gx_game_test_aspApplejack );


	gx_game_test_skmTwilightSparkle = new GXQSkeletalTwilightSparkle ();
	gx_game_test_skmTwilightSparkle->SetLocation ( -25.0f, 0.0f, 0.0f );
	gx_game_test_skmTwilightSparkle->SetRotation ( 0.0f, 0.0f, 0.0f );

	gx_game_test_skaiTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_WALK ] = new GXSkeletalMeshAnimationInfo;
	GXLoadPSA ( L"../Animations/Test/Twily_Anim_Walk.psa", gx_game_test_skaiTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_WALK ] );
	gx_game_test_aspTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_WALK ] = new GXAnimSolverPlayer ( 0 );
	gx_game_test_aspTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_WALK ]->SetAnimationMultiplier ( 1.0f );
	gx_game_test_aspTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_WALK ]->SetAnimationSequence ( gx_game_test_skaiTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_WALK ] );

	gx_game_test_skaiTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_RUN ] = new GXSkeletalMeshAnimationInfo;
	GXLoadPSA ( L"../Animations/Test/Twily_Anim_Run.psa", gx_game_test_skaiTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_RUN ] );
	gx_game_test_aspTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_RUN ] = new GXAnimSolverPlayer ( 1 );
	gx_game_test_aspTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_RUN ]->SetAnimationMultiplier ( 1.0f );
	gx_game_test_aspTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_RUN ]->SetAnimationSequence ( gx_game_test_skaiTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_RUN ] );

	gx_game_test_skaiTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_IDLE ] = new GXSkeletalMeshAnimationInfo;
	GXLoadPSA ( L"../Animations/Test/Twily_Anim_Fun_Jump.psa", gx_game_test_skaiTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_IDLE ] );
	gx_game_test_aspTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_IDLE ] = new GXAnimSolverPlayer ( 2 );
	gx_game_test_aspTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_IDLE ]->SetAnimationMultiplier ( 2.0f );
	gx_game_test_aspTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_IDLE ]->SetAnimationSequence ( gx_game_test_skaiTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_IDLE ] );

	gx_game_test_asmTwilightSparkle = new GXAnimSolverMovement ( 3 );
	gx_game_test_asmTwilightSparkle->SetAnimStream ( GX_ANIM_SOLVER_MOVEMENT_WALK, gx_game_test_aspTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_WALK ] );
	gx_game_test_asmTwilightSparkle->SetAnimStream ( GX_ANIM_SOLVER_MOVEMENT_RUN, gx_game_test_aspTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_RUN ] );
	gx_game_test_asmTwilightSparkle->SetAnimStream ( GX_ANIM_SOLVER_MOVEMENT_IDLE, gx_game_test_aspTwilightSparkle[ GX_ANIM_SOLVER_MOVEMENT_IDLE ] );

	gx_game_test_skmTwilightSparkle->SetAnimSolver ( gx_game_test_asmTwilightSparkle );


	gx_game_test_stmGround = new GXStaticMeshGround ();

	gx_game_test_stmBall = new GXStaticMeshBall ();
	GXFloat factor = 0.6f;
	gx_game_test_stmBall->SetScale ( factor, factor, factor );
	gx_game_test_physActor->SetMesh ( gx_game_test_stmBall );

	gx_game_test_stmAJ_Hat = new GXStaticMeshAJHat ();

	gx_game_test_Sky = new GXSkyGlobeSunny ();

	gx_game_test_MLPQuad = new GXMLPRenderQuad ();
	gx_game_test_MLPQuad->SetClearColor ( gx_game_test_ClearColor );

	gx_game_test_TextureOuter = new GXTextureOuter ();
	gx_game_test_DofEffect = new GXDoFEffect ();
	gx_game_test_DofEffect->SetDoFParams ( gx_game_test_fDoFFocus, gx_game_test_fDoFRange, gx_game_test_fDoFBlurness );
	gx_game_test_MotionBlurEffect = new GXMotionBlurEffect ();

	gx_game_test_ctrTwilightSparkleExt->SetMesh ( gx_game_test_skmTwilightSparkle );
	gx_game_test_ctrTwilightSparkleExt->SetAnimSolver ( gx_game_test_asmTwilightSparkle );

	gx_game_test_MusicEmitter->Play ();

	GXParticleSystemParams psParams;
	psParams.SetDefault ();

	gx_game_test_ParticlesEmitter = new GXRealParticles ( psParams );
	gx_game_test_ParticlesEmitter->SetLocation ( 0.0f, 50.0f, 20.0f );
	gx_game_test_ParticlesEmitter->SetScale ( 0.2f, 0.2f, 0.2f );

	gx_game_test_HudSurface = new GXHudSurface ( 512, 256 );
	gx_game_test_HudSurface->SetScale ( 5.0f );

	GXLoadMTR ( L"../Materials/Test/Character_Hud_Surface.mtr", gx_game_test_HudBackground );
	GXGetTexture ( gx_game_test_HudBackground.textures[ 0 ] );
	GXGetTexture ( gx_game_test_HudBackground.textures[ 1 ] );
	GXGetTexture ( gx_game_test_HudBackground.textures[ 2 ] );
	GXGetTexture ( gx_game_test_HudBackground.textures[ 3 ] );

	GXSetMat4RotationXYZ ( gx_game_test_RelativeRot, GXDegToRad ( 20.0f ), GXDegToRad ( 210.0f ), GXDegToRad ( 0.0f ) );
}

GXVoid GXCALL OnDeleteRenderableObjects ()
{
	GXRemoveTexture ( gx_game_test_HudBackground.textures[ 0 ] );
	GXRemoveTexture ( gx_game_test_HudBackground.textures[ 1 ] );
	GXRemoveTexture ( gx_game_test_HudBackground.textures[ 2 ] );
	GXRemoveTexture ( gx_game_test_HudBackground.textures[ 3 ] );
	gx_game_test_HudBackground.Cleanup ();			//Удалит свои текстуры

	GXSafeDelete ( gx_game_test_HudSurface );

	GXSafeDelete ( gx_game_test_ParticlesEmitter );

	GXSafeDelete ( gx_game_test_Sky );

	GXSafeDelete ( gx_game_test_stmAJ_Hat );
	GXSafeDelete ( gx_game_test_stmBall );
	GXSafeDelete ( gx_game_test_stmGround );

	GXSafeDelete ( gx_game_test_skmPinkiePie );
	GXSafeDelete ( gx_game_test_aspPinkiePie );

	GXSafeDelete ( gx_game_test_skmRarity );
	GXSafeDelete ( gx_game_test_aspRarity );

	GXSafeDelete ( gx_game_test_skmFluttershy );
	GXSafeDelete ( gx_game_test_aspFluttershy );

	GXSafeDelete ( gx_game_test_skmRainbowDash  );
	GXSafeDelete ( gx_game_test_aspRainbowDash );

	GXSafeDelete ( gx_game_test_skmApplejack );
	GXSafeDelete ( gx_game_test_aspApplejack );

	GXSafeDelete ( gx_game_test_skmTwilightSparkle );
	GXSafeDelete ( gx_game_test_asmTwilightSparkle );
	for ( GXUChar i = 0; i < GX_ANIM_SOLVER_MOVEMENT_STATES; i++ )
		GXSafeDelete ( gx_game_test_aspTwilightSparkle[ i ] );

	GXSafeDelete ( gx_game_test_MotionBlurEffect );
	GXSafeDelete ( gx_game_test_DofEffect );
	GXSafeDelete ( gx_game_test_TextureOuter );
	GXSafeDelete ( gx_game_test_MLPQuad );
}

//------------------------------------------------------------------------------------

GXVoid GXCALL OnPhysics ( GXFloat deltatime )
{
	gx_game_test_ctrTwilightSparkleExt->Perform ( deltatime );
}

//------------------------------------------------------------------------------------

GXVoid GXCALL WalkForward ()
{
	if ( !gx_game_test_ctrCurrent ) return;
	gx_game_test_ctrCurrent->WalkForward ();
}

GXVoid GXCALL WalkBackward ()
{
	if ( !gx_game_test_ctrCurrent ) return;
	gx_game_test_ctrCurrent->WalkBackward ();
}

GXVoid GXCALL StopWalk ()
{
	if ( !gx_game_test_ctrCurrent ) return;
	gx_game_test_ctrCurrent->StopWalk ();
}

GXVoid GXCALL RotateLeft ()
{
	if ( !gx_game_test_ctrCurrent ) return;
	gx_game_test_ctrCurrent->RotateLeft ();
}

GXVoid GXCALL RotateRight ()
{
	if ( !gx_game_test_ctrCurrent ) return;
	gx_game_test_ctrCurrent->RotateRight ();
}

GXVoid GXCALL StopRotate ()
{
	if ( !gx_game_test_ctrCurrent ) return;
	gx_game_test_ctrCurrent->StopRotate ();
}

GXVoid GXCALL StrafeLeft ()
{
	if ( !gx_game_test_ctrCurrent ) return;
	gx_game_test_ctrCurrent->StrafeLeft ();
}

GXVoid GXCALL StrafeRight ()
{
	if ( !gx_game_test_ctrCurrent ) return;
	gx_game_test_ctrCurrent->StrafeRight ();
}

GXVoid GXCALL StopStrafe ()
{
	if ( !gx_game_test_ctrCurrent ) return;
	gx_game_test_ctrCurrent->StopStrafe ();
}

GXVoid GXCALL Run ()
{
	if ( !gx_game_test_ctrCurrent ) return;
	gx_game_test_ctrCurrent->Run ();
}

GXVoid GXCALL StopRun()
{
	if ( !gx_game_test_ctrCurrent ) return;
	gx_game_test_ctrCurrent->StopRun ();
}

GXVoid GXCALL Jump ()
{
	if ( !gx_game_test_ctrCurrent ) return;
	gx_game_test_ctrCurrent->Jump ();
}

GXVoid GXCALL MouseMoveFunc ( GXInt win_x, GXInt win_y )
{
	static GXInt lastX;
	static GXInt lastY;
	GXFloat speed = 0.01f;
	gx_game_test_iWinCursorPosX = win_x;
	gx_game_test_iWinCursorPosY = gx_EngineSettings.rendererHeight - win_y;
	if ( gx_game_test_mouseFlags.lmb )
	{
		if ( gx_game_test_bNeedUpdateLastCursor )
		{
			lastX = win_x;
			lastY = win_y;
			gx_game_test_bNeedUpdateLastCursor = GX_FALSE;
		}
		gx_game_test_CameraRotOffset.pitch_rad += ( lastX - win_x ) * speed;
		gx_game_test_CameraRotOffset.yaw_rad += ( lastY - win_y ) * speed;
		lastX = win_x;
		lastY = win_y;
	}
	if ( gx_game_test_mouseFlags.rmb )
	{
		gx_game_test_MouseRotationFactor = ( lastX - win_x ) * speed;
		lastX = win_x;
		lastY = win_y;
	}
}

GXVoid GXCALL MouseButtonsFunc ( EGXInputMouseFlags mouseFlags )
{
	gx_game_test_mouseFlags.allflags = mouseFlags.allflags;
	if ( gx_game_test_mouseFlags.lmb )
	{
		gx_game_test_bNeedUpdateLastCursor = GX_TRUE;
		gx_game_test_CameraRotOffset.yaw_rad = GX_MATH_HALFPI;
	}
	else
		gx_game_test_CameraRotOffset.yaw_rad = 0.0f;
	gx_game_test_CameraRotOffset.pitch_rad = 0.0f;
}

GXVoid GXCALL MouseWheelFunc ( GXInt steps )
{
	GXFloat factor = gx_game_test_fDoFFocus * 0.001f * 80.0f; 
	gx_game_test_fDoFFocus += steps * factor;
	if ( gx_game_test_fDoFFocus > 100000.0f ) 
		gx_game_test_fDoFFocus = 100000.0f;
	else if ( gx_game_test_fDoFFocus < 0.0f )
		gx_game_test_fDoFFocus = 300.0f;
	gx_game_test_DofEffect->SetDoFParams ( gx_game_test_fDoFFocus, gx_game_test_fDoFRange, gx_game_test_fDoFBlurness );
}

#define DEAD_ZONE  0.1f
GXVoid GXCALL StickMoving ( GXFloat x, GXFloat y )
{
	if ( x > DEAD_ZONE )
		StrafeRight ();
	else if ( x < -DEAD_ZONE )
		StrafeLeft ();
	/*else
		StopStrafe ();*/

	if ( y > DEAD_ZONE )
		WalkForward ();
	else if ( y < -DEAD_ZONE )
		WalkBackward ();
	/*else
		StopWalk ();*/
}

GXVoid GXCALL StickRotating ( GXFloat x, GXFloat y )
{
	if ( x > DEAD_ZONE )
		RotateRight ();
	else if ( x < -DEAD_ZONE )
		RotateLeft ();
	/*else
		StopRotate ();*/
}
#undef DEAD_ZONE