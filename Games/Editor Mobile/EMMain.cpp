#include "EMMain.h"
#include "EMUIButton.h"
#include "EMUIMenu.h"
#include "EMDirectedLightActor.h"
#include "EMUnitActor.h"
#include "EMGlobals.h"
#include "EMMoveTool.h"
#include <GXEngine/GXGlobals.h>
#include <GXEngine/GXOpenGL.h>
#include <GXEngine/GXCameraOrthographic.h>
#include <GXEngine/GXCameraPerspective.h>


GXCameraOrthographic*	em_HudCamera = 0;
GXCameraPerspective*	em_SceneCamera = 0;
EMUIButton*				em_Button1 = 0;
EMUIButton*				em_Button2 = 0;
EMUIMenu*				em_Menu = 0;
EMDirectedLightActor*	em_DirectedLight = 0;
EMUnitActor*			em_UnitActor = 0;
EMMoveTool*				em_MoveTool = 0;
GXInt					em_MouseX = 0;
GXInt					em_MouseY = 0;


GXVoid GXCALL EMOnLMB1 ( GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	if ( state == GX_MOUSE_BUTTON_DOWN ) return;

	GXLocale* locale = gx_Core->GetLocale ();
	locale->SetLanguage ( GX_LANGUAGE_RU );

	em_Button1->SetCaption ( locale->GetString ( L"russian" ) );
	em_Button2->SetCaption ( locale->GetString ( L"english" ) );

	em_Button1->Disable ();
	em_Button2->Enable ();
}

GXVoid GXCALL EMOnLMB2 ( GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	if ( state == GX_MOUSE_BUTTON_DOWN ) return;

	GXLocale* locale = gx_Core->GetLocale ();
	locale->SetLanguage ( GX_LANGUAGE_EN );

	em_Button1->SetCaption ( locale->GetString ( L"russian" ) );
	em_Button2->SetCaption ( locale->GetString ( L"english" ) );

	em_Button2->Disable ();
	em_Button1->Enable ();
}

GXVoid GXCALL EMOnSave ( GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	if ( state == GX_MOUSE_BUTTON_UP )
		GXLogW ( L"EMOnSave::Info - Just worked\n" );
}

GXVoid GXCALL EMOnExit ( GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	if ( state != GX_MOUSE_BUTTON_UP ) return;

	gx_Core->Exit ();
	GXLogA ( "Завершение\n" );
}

GXVoid GXCALL EMOnDummy ( GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	//NOTHING
}


GXVoid GXCALL EMOnMoveTool ( GXFloat x, GXFloat y, eGXMouseButtonState state )
{
	//TODO
}

//-----------------------------------------------------------------------------

GXVoid GXCALL EMOnMouseMove ( GXInt win_x, GXInt win_y )
{
	em_MouseX = win_x;
	em_MouseY = gx_Core->GetRenderer ()->GetHeight () - win_y;
}

GXVoid GXCALL EMOnMouseButton ( EGXInputMouseFlags mouseflags )
{
	if ( mouseflags.lmb )
		em_Renderer->GetObject ( em_MouseX, em_MouseY );
}

GXVoid GXCALL EMOnObject ( GXUInt object )
{
	if ( !object ) return;

	EMActor* actor = (EMActor*)object;
	GXLogW ( L"EMOnObject::Info - Объект %s (адрес 0x%08X)\n", actor->GetName (), object );
}

//-----------------------------------------------------------------------------

GXFloat em_Angle = 0.0f;

GXBool GXCALL EMOnFrame ( GXFloat deltatime )
{
	gx_Core->GetTouchSurface ()->ExecuteMessages ();

	gx_ActiveCamera = em_SceneCamera;
	em_Renderer->StartCommonPass ();
	
	em_UnitActor->OnDrawCommonPass ();
	
	em_Renderer->StartLightPass ();
	
	em_Renderer->StartHudColorPass ();
	em_MoveTool->OnDrawHudColorPass ();

	gx_ActiveCamera = em_HudCamera;
	
	EMDrawUI ();

	em_Renderer->StartHudMaskPass ();
	EMDrawUIMasks ();

	gx_ActiveCamera = em_SceneCamera;
	em_MoveTool->OnDrawHudMaskPass ();

	em_Renderer->PresentFrame ();
	
	GXMat4 r;
	GXSetMat4RotationXYZ ( r, 2.0f * em_Angle, 1.5f * em_Angle, 4.0f * em_Angle );

	em_UnitActor->SetOrigin ( r );
	
	em_Angle += deltatime * 0.5f;
	
	return GX_TRUE;
}

GXVoid GXCALL EMOnInitRenderableObjects ()
{
	GXFloat w = (GXFloat)gx_Core->GetRenderer ()->GetWidth ();
	GXFloat h = (GXFloat)gx_Core->GetRenderer ()->GetHeight ();

	em_Renderer = new EMRenderer ();
	em_Renderer->SetOnObjectCallback ( &EMOnObject );

	em_HudCamera = new GXCameraOrthographic ( w, h, 0.0f, 100.0f );
	em_SceneCamera = new GXCameraPerspective ( GXDegToRad ( 60.0f ), w / h, 0.1f, 1000.0f );
	em_SceneCamera->SetLocation ( 0.0f, 0.0f, -10.0f );

	GXLocale* locale = gx_Core->GetLocale ();
	locale->SetLanguage ( GX_LANGUAGE_RU );

	em_Button1 = new EMUIButton ();
	em_Button1->SetOnLeftMouseButtonCallback ( &EMOnLMB1 );
	em_Button1->Disable ();
	em_Button1->Resize ( 100.0f, 100.0f, gx_ui_Scale * 4.0f, gx_ui_Scale * 1.0f );
	em_Button1->SetLayer ( 50.0f );
	em_Button1->SetCaption ( locale->GetString ( L"russian" ) );

	em_Button2 = new EMUIButton ();
	em_Button2->SetOnLeftMouseButtonCallback ( &EMOnLMB2 );
	em_Button2->Enable ();
	em_Button2->Resize ( 100.0f + gx_ui_Scale * 4.2f, 100.0f, gx_ui_Scale * 4.0f, gx_ui_Scale * 0.5f );
	em_Button2->SetLayer ( 50.0f );
	em_Button2->SetCaption ( locale->GetString ( L"english" ) );

	em_Menu = new EMUIMenu ();
	em_Menu->AddItem ( locale->GetString ( L"Main menu->File" ) );
	em_Menu->AddSubitem ( 0, locale->GetString ( L"File->New" ), locale->GetString ( L"File->New (Hotkey)" ), &EMOnSave );
	em_Menu->AddSubitem ( 0, locale->GetString ( L"File->Open" ), locale->GetString ( L"File->Open (Hotkey)" ), &EMOnExit );
	em_Menu->AddSubitem ( 0, locale->GetString ( L"File->Close" ), locale->GetString ( L"File->Close (Hotkey)" ), &EMOnExit );
	em_Menu->AddSubitem ( 0, locale->GetString ( L"File->Save" ), locale->GetString ( L"File->Save (Hotkey)" ), &EMOnSave );
	em_Menu->AddSubitem ( 0, locale->GetString ( L"File->Exit" ), locale->GetString ( L"File->Exit (Hotkey)" ), &EMOnExit );

	em_Menu->AddItem ( locale->GetString ( L"Main menu->Create" ) );
	em_Menu->AddSubitem ( 1, locale->GetString ( L"Create->Unit Actor" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 1, locale->GetString ( L"Create->Static mesh" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 1, locale->GetString ( L"Create->Skeletal mesh" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 1, locale->GetString ( L"Create->Directed light" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 1, locale->GetString ( L"Create->Spot" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 1, locale->GetString ( L"Create->Bulp" ), 0, &EMOnDummy );

	em_Menu->AddItem ( locale->GetString ( L"Main menu->Tools" ) );
	em_Menu->AddSubitem ( 2, locale->GetString ( L"Tools->Select" ), locale->GetString ( L"Tools->Select (Hotkey)" ), &EMOnDummy );
	em_Menu->AddSubitem ( 2, locale->GetString ( L"Tools->Move" ), locale->GetString ( L"Tools->Move (Hotkey)" ), &EMOnMoveTool );
	em_Menu->AddSubitem ( 2, locale->GetString ( L"Tools->Rotate" ), locale->GetString ( L"Tools->Rotate (Hotkey)" ), &EMOnDummy );
	em_Menu->AddSubitem ( 2, locale->GetString ( L"Tools->Scale" ), locale->GetString ( L"Tools->Scale (Hotkey)" ), &EMOnDummy );

	em_Menu->AddItem ( locale->GetString ( L"Main menu->Utility" ) );
	em_Menu->AddSubitem ( 3, locale->GetString ( L"Utility->Particle system" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 3, locale->GetString ( L"Utility->Animation graph" ), 0, &EMOnDummy );

	em_Menu->AddItem ( locale->GetString ( L"Main menu->Language" ) );
	em_Menu->AddSubitem ( 4, locale->GetString ( L"Language->Русский" ), 0, &EMOnDummy );
	em_Menu->AddSubitem ( 4, locale->GetString ( L"Language->English" ), 0, &EMOnDummy );

	em_Menu->SetLayer ( 50.0f );
	em_Menu->SetLocation ( 0.0f, h - gx_ui_Scale * 0.5f );

	GXMat4 origin;
	GXSetMat4Identity ( origin );
	em_DirectedLight = new EMDirectedLightActor ( L"Directed light 01", origin );

	em_UnitActor = new EMUnitActor ( L"Unit actor 01", origin );

	em_MoveTool = new EMMoveTool ();
	em_MoveTool->Bind ();
	em_MoveTool->SetActor ( em_UnitActor );

	ShowCursor ( 1 );
}

GXVoid GXCALL EMOnDeleteRenderableObjects ()
{
	GXSafeDelete ( em_UnitActor );
	GXSafeDelete ( em_DirectedLight );

	GXSafeDelete ( em_Menu );
	GXSafeDelete ( em_Button1 );
	GXSafeDelete ( em_Button2 );
	GXSafeDelete ( em_HudCamera );
	GXSafeDelete ( em_SceneCamera );

	em_MoveTool->UnBind ();
	em_MoveTool->Delete ();
	em_MoveTool->OnDrawHudColorPass ();

	GXSafeDelete ( em_Renderer );
}

//-------------------------------------------------------------------

GXVoid GXCALL EMExit ()
{
	gx_Core->Exit ();
	GXLogA ( "Завершение\n" );
}

//-------------------------------------------------------------------

GXVoid GXCALL EMOnInit ()
{
	GXRenderer* renderer = gx_Core->GetRenderer ();
	renderer->SetOnInitRenderableObjectsFunc ( &EMOnInitRenderableObjects );
	renderer->SetOnFrameFunc ( &EMOnFrame );
	renderer->SetOnDeleteRenderableObjectsFunc ( &EMOnDeleteRenderableObjects );

	GXLocale* locale = gx_Core->GetLocale ();
	locale->LoadLanguage ( L"../Locale/Editor Mobile/RU.lng", GX_LANGUAGE_RU );
	locale->LoadLanguage ( L"../Locale/Editor Mobile/EN.lng", GX_LANGUAGE_EN );
	locale->SetLanguage ( GX_LANGUAGE_RU );

	GXInput* input = gx_Core->GetInput ();
	input->BindKeyFunc ( &EMExit, VK_ESCAPE, INPUT_UP );
	input->BindMouseMoveFunc ( &EMOnMouseMove );
	input->BindMouseButtonsFunc ( &EMOnMouseButton );
}

GXVoid GXCALL EMOnClose ()
{
	GXInput* input = gx_Core->GetInput ();
	input->UnBindMouseMoveFunc ();
	input->UnBindMouseButtonsFunc ();
	input->UnBindKeyFunc ( VK_ESCAPE, INPUT_UP );
}
