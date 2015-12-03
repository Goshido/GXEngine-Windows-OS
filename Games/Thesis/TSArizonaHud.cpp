//version 1.1

#include "TSArizonaHud.h"
#include "TSHud2DPen.h"
#include "TSGismoTransform.h"
#include <GXEngine/GXMenu.h>
#include <GXEngine/GXCameraOrthographic.h>
#include <GXEngine/GXCameraPerspective.h>
#include "TSGlobals.h"


GXCameraPerspective*		ts_hud_ViewCamera = 0;

TSGismoTransform*			ts_hud_GismoTransform = 0;

GXHudSurface*				ts_hud_Caption = 0;
GXHudSurface*				ts_hud_Contacts = 0;
GXHudSurface*				ts_hud_FPS = 0;
GXHudSurface*				ts_hud_Engine = 0;

FT_Face						ts_hud_Font = 0;

GXMaterialInfo				ts_hud_ContactsGXMaterial;
GXUInt						ts_hud_LastFPS = 0xFFFFFFFF;

#define		TS_HUD_VISIBLE	0
#define		TS_HUD_HIDE		1
#define		TS_HUD_APPEAR	2
#define		TS_HUD_VANISH	3

GXMenu*						ts_hud_Menu = 0;
GXUChar						ts_hud_MenuState = TS_HUD_HIDE;
GXFloat						ts_hud_MenuTimer = 0.3f;

#define TS_HUD_Z			10.0f



GXVoid GXCALL TSOnUp ()
{
	if ( !ts_hud_Menu || ts_hud_MenuState == TS_HUD_HIDE ) return;

	ts_hud_Menu->OnFocusPrevious ();
}

GXVoid GXCALL TSOnDown ()
{
	if ( !ts_hud_Menu || ts_hud_MenuState == TS_HUD_HIDE ) return;

	ts_hud_Menu->OnFocusNext ();
}

GXVoid GXCALL TSOnSelect ()
{
	if ( !ts_hud_Menu || ts_hud_MenuState == TS_HUD_HIDE ) return;

	ts_hud_Menu->OnSelect ();
}

GXVoid GXCALL TSOnMenu ()
{
	switch ( ts_hud_MenuState )
	{
		case TS_HUD_VISIBLE:
		{
			ts_hud_MenuState = TS_HUD_VANISH;
			ts_hud_MenuTimer = 0.3f;
		}
		break;
		
		case TS_HUD_HIDE:
		{
			ts_hud_MenuState = TS_HUD_APPEAR;
			ts_hud_MenuTimer = 0.3f;
		}
		break;

		default:
		break;
	};
}

GXVoid GXCALL TSHudOnExit ()
{
	ts_SceneKeeper->SwitchScene ( MAIN_MENU );
}

GXVoid GXCALL TSHudOnUpdateMenu ( GXFloat delta )
{
	if ( ts_hud_MenuState == TS_HUD_VISIBLE || ts_hud_MenuState == TS_HUD_HIDE ) return;

	ts_hud_MenuTimer -= delta;

	GXVec3	point1 ( 0.0f, 0.0f, 1000.0f );
	GXVec3	point2 ( 1800.0f, 500.0f, 600.0f );
	GXVec3	point3 ( -250.0f, 1000.0f, 150.0f );

	GXVec3 now;
	GXFloat interpolate = cosf ( GX_MATH_HALFPI * ts_hud_MenuTimer * 3.3333f );

	if ( ts_hud_MenuState == TS_HUD_APPEAR )
	{
		now.x = point2.x + ( point1.x - point2.x ) * interpolate;
		now.y = point2.y + ( point1.y - point2.y ) * interpolate;
		now.z = point2.z + ( point1.z - point2.z ) * interpolate;

		if ( ts_hud_MenuTimer <= 0.0f )
			ts_hud_MenuState = TS_HUD_VISIBLE;
	}
	else
	{
		now.x = point1.x + ( point3.x - point1.x ) * interpolate;
		now.y = point1.y + ( point3.y - point1.y ) * interpolate;
		now.z = point1.z + ( point3.z - point1.z ) * interpolate;

		if ( ts_hud_MenuTimer <= 0.0f )
			ts_hud_MenuState = TS_HUD_HIDE;
	}

	ts_hud_Menu->SetLocation ( now.x, now.y, now.z );
}

//--------------------------------------------------------------------------------------------

GXVoid GXCALL TSHudFPSUpdate ()
{
	GXUShort fps = gx_Core->GetRenderer ()->GetCurrentFPS ();
	if ( ts_hud_LastFPS == fps ) return;

	ts_hud_LastFPS = fps;
	
	ts_hud_FPS->Reset ();

	GXPenInfo info;
	GXVec4 greenFontColor;
	GXColorToVec4 ( greenFontColor, 115, 185, 0, 255 );
	info.fontColor = &greenFontColor;
	info.font = ts_hud_Font;
	info.fontSize = 20;
	info.insertX = 90;
	info.insertY = 10;
	info.kerning = GX_TRUE;

	info.insertX = ts_hud_FPS->AddText ( info, L"Текущий FPS" ) + 10;

	GXVec4 whiteFontColor;
	GXColorToVec4 ( whiteFontColor, 255, 255, 255, 255 );
	info.fontColor = &whiteFontColor;

	ts_hud_FPS->AddText ( info, L"%i", ts_hud_LastFPS );
}

GXVoid GXCALL TSArizonaHudInit ()
{
	ts_hud_Font = GXGetFont ( L"../Fonts/trebuc.ttf" );

	GXFloat aspectRatio = gx_EngineSettings.rendererWidth / (GXFloat)gx_EngineSettings.rendererHeight;
	ts_hud_ViewCamera = new GXCameraPerspective ( GXDegToRad ( 45.0f ), aspectRatio, 0.1f, 5000.0f );

	ts_hud_MenuState = TS_HUD_HIDE;
	ts_hud_MenuTimer = 0.3f;

	ts_hud_Menu = new GXMenu ( 256, 256 );
	GXMenuStyle style;
	style.showBackground = GX_TRUE;
	style.padding = 100;
	ts_hud_Menu->SetStyle ( style );
	ts_hud_Menu->SetLocation ( 0.0f, 0.0f, 1000.0f );

	GXMat4 rot;
	GXSetMat4RotationXYZ ( rot, GXDegToRad ( 10.0f ), GXDegToRad ( 10.0f ), GXDegToRad ( -1.0f ) );
	ts_hud_Menu->SetRotation ( rot );
	
	ts_hud_Menu->AddItem ( L"Продолжить", &TSOnMenu );
	ts_hud_Menu->AddItem ( L"Выход", &TSHudOnExit );
	ts_hud_Menu->SetFocus ( 0 );

	ts_Input->BindKeyFunc ( &TSOnMenu, VK_ESCAPE, INPUT_UP );

	ts_Input->BindKeyFunc ( &TSOnUp, VK_UP, INPUT_UP );
	ts_Input->BindKeyFunc ( &TSOnDown, VK_DOWN, INPUT_UP );
	ts_Input->BindKeyFunc ( &TSOnSelect, VK_RETURN, INPUT_UP );

	ts_hud_GismoTransform = new TSGismoTransform ();

	GXFloat factor = sinf ( GXDegToRad ( 22.5f ) ) * 22.0f;
	ts_hud_GismoTransform->SetLocation ( factor * aspectRatio - 1.5f, -factor + 1.5f, cosf ( GXDegToRad ( 22.5f ) ) * 22.0f );

	GXSetMat4RotationXY ( rot, GXDegToRad ( 90.0f ), 0.0f );
	GXMat4 irot;
	GXSetMat4Inverse ( irot, rot );
	ts_hud_GismoTransform->SetRotation ( irot );

	ts_hud_FPS = new GXHudSurface ( 256, 32 );
	TSHudFPSUpdate ();
	ts_hud_FPS->SetScale ( 256.0f * 0.5f );
	ts_hud_FPS->SetLocation ( gx_EngineSettings.rendererWidth * 0.5f - 128.0f, gx_EngineSettings.rendererHeight * 0.5f - 16.0f, TS_HUD_Z );
	
	ts_hud_Caption = new GXHudSurface ( 512, 64 );

	GXPenInfo info;
	GXVec4 greenFontColor;
	GXColorToVec4 ( greenFontColor, 115, 185, 0, 255 );
	info.fontColor = &greenFontColor;
	info.font = ts_hud_Font;
	info.fontSize = 20;
	info.insertX = 120;
	info.insertY = 40;
	info.kerning = GX_TRUE;
	ts_hud_Caption->AddText ( info, L"Карта" );

	GXVec4 whiteFontColor;
	GXColorToVec4 ( whiteFontColor, 255, 255, 255, 255 );
	info.fontColor = &whiteFontColor;
	info.insertX = 163;
	info.insertY = 20;
	ts_hud_Caption->AddText ( info, L"\"Demo.scn\"" );

	ts_hud_Caption->SetScale ( 512.0f * 0.5f );
	ts_hud_Caption->SetLocation ( 0.0f, gx_EngineSettings.rendererHeight * 0.5f - 32.0f, TS_HUD_Z );

	//-------------------------------------------------------------------

	GXLoadMTR ( L"../Materials/Thesis/Hud_Content.mtr", ts_hud_ContactsGXMaterial );
	GXGetTexture ( ts_hud_ContactsGXMaterial.textures[ 0 ] );
	GXGetTexture ( ts_hud_ContactsGXMaterial.textures[ 1 ] );
	GXGetTexture ( ts_hud_ContactsGXMaterial.textures[ 2 ] );
	
	ts_hud_Contacts = new GXHudSurface ( 512, 512 );
	ts_hud_Contacts->Reset ();
	
	info.fontColor = &greenFontColor;
	info.insertX = 5;
	info.insertY = 100;
	ts_hud_Contacts->AddText ( info, L"Контакты:" );

	GXFloat invAsp = 116.0f / 320.0f;
	GXImageInfo imgInfo;
	imgInfo.insertX = 10;
	imgInfo.insertY = 55;
	imgInfo.insertWidth = 100;
	imgInfo.insertHeight = (GXInt)( 100 * invAsp );
	imgInfo.overlayType = GX_ALPHA_TRANSPARENCY;

	GXGLTextureStruct ts;
	ts.type = GL_UNSIGNED_BYTE;
	ts.format = GL_RGBA;
	ts.internalFormat = GL_RGBA8;
	ts.width = ts_hud_ContactsGXMaterial.textures[ 0 ].usWidth;
	ts.height = ts_hud_ContactsGXMaterial.textures[ 0 ].usHeight;
	ts.wrap = GL_CLAMP_TO_EDGE;
	ts.data = 0;

	imgInfo.texture = &ts;
	imgInfo.textureID = ts_hud_ContactsGXMaterial.textures[ 0 ].uiId;
	
	ts_hud_Contacts->AddImage ( imgInfo );

	info.fontColor = &whiteFontColor;
	info.insertX = 120;
	info.insertY = 65;
	ts_hud_Contacts->AddText ( info, L"/  GoshidoMatazuki@mail.ru" );
	
	invAsp = 200.0f / 444.0f;
	imgInfo.insertX = 10;
	imgInfo.insertY = 0;
	imgInfo.insertWidth = 100;
	imgInfo.insertHeight = (GXInt)( 100 * invAsp );
	imgInfo.textureID = ts_hud_ContactsGXMaterial.textures[ 1 ].uiId;

	ts_hud_Contacts->AddImage ( imgInfo );

	info.insertX = 120;
	info.insertY = 15;
	ts_hud_Contacts->AddText ( info, L"/  GoshidoMatazuki@gmail.com" );
	
	ts_hud_Contacts->SetScale ( 512.0f * 0.5f );
	ts_hud_Contacts->SetLocation ( -gx_EngineSettings.rendererWidth * 0.5f + 256.0f, -gx_EngineSettings.rendererHeight * 0.5f + 256.0f, TS_HUD_Z );

	ts_hud_Engine = new GXHudSurface ( 128, 128 );

	info.fontColor = &greenFontColor;
	info.font = ts_hud_Font;
	info.fontSize = 20;
	info.insertX = 5;
	info.insertY = 105;
	ts_hud_Engine->AddText ( info, L"Powered by" );

	invAsp = 128.0f / 128.0f;
	imgInfo.insertX = 25;
	imgInfo.insertY = 30;
	imgInfo.insertWidth = 64;
	imgInfo.insertHeight = (GXInt)( 64 * invAsp );
	imgInfo.textureID = ts_hud_ContactsGXMaterial.textures[ 2 ].uiId;

	ts_hud_Engine->AddImage ( imgInfo );

	ts_hud_Engine->SetScale ( 128.0f * 0.5f );
	ts_hud_Engine->SetLocation ( -gx_EngineSettings.rendererWidth * 0.5f + 64.0f, gx_EngineSettings.rendererHeight * 0.5f - 64.0f, TS_HUD_Z );
}

GXVoid GXCALL TSArizonaHudUpdate ( GXFloat delta )
{
	GXMat4 rot;
	gx_ActiveCamera->GetRotation ( rot );
	ts_hud_GismoTransform->SetRotation ( rot );

	gx_ActiveCamera = ts_hud_ViewCamera;

	ts_hud_GismoTransform->Draw ();

	if ( ts_hud_MenuState != TS_HUD_HIDE )
	{
		TSHudOnUpdateMenu ( delta );
		ts_hud_Menu->Update ( delta );
		ts_hud_Menu->Draw ();
	}

	gx_ActiveCamera = ts_ScreenCamera;

	TSHudFPSUpdate ();
	ts_hud_FPS->Draw ();

	ts_hud_Contacts->Draw ();
	ts_hud_Caption->Draw ();
	ts_hud_Engine->Draw ();
}

GXVoid GXCALL TSArizonaHudDestroy ()
{
	ts_Input->UnBindKeyFunc ( VK_ESCAPE, INPUT_UP );

	ts_Input->UnBindKeyFunc ( VK_UP, INPUT_UP );
	ts_Input->UnBindKeyFunc ( VK_DOWN, INPUT_UP );
	ts_Input->UnBindKeyFunc ( VK_RETURN, INPUT_UP );

	GXRemoveTexture ( ts_hud_ContactsGXMaterial.textures[ 0 ] );
	GXRemoveTexture ( ts_hud_ContactsGXMaterial.textures[ 1 ] );
	GXRemoveTexture ( ts_hud_ContactsGXMaterial.textures[ 2 ] );

	GXRemoveFont ( ts_hud_Font );

	GXSafeDelete ( ts_hud_GismoTransform );

	GXSafeDelete ( ts_hud_FPS );
	GXSafeDelete ( ts_hud_Engine );
	GXSafeDelete ( ts_hud_Caption );
	GXSafeDelete ( ts_hud_Contacts );

	GXSafeDelete ( ts_hud_ViewCamera );

	ts_hud_Menu->Delete ();
	ts_hud_Menu->Draw ();
}
