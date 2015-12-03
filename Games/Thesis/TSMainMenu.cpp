//version 1.0

#include "TSMainMenu.h"
#include "TSGlobals.h"
#include <GXEngine/GXHudSurface.h>
#include <GXEngine/GXMenu.h>
#include <GXEngine/GXEditBox.h>
#include "TSHistory.h"
#include "TSConfig.h"
#include "TSServer.h"
#include "TSClient.h"
#include <GXCommon/GXCFGSaver.h>
#include <GXCommon/GXCFGLoader.h>
#include <GXCommon/GXStrings.h>
#include <GXEngine/GXOGGSoundProvider.h>
#include <GXEngine/GXSoundStorage.h>


enum eTSMainMenuInput
{
	PC,
	XBOX
};

enum eTSMainMenuState
{
	TITLE,
	JOIN,
	EDIT_SERVER,
	SETTINGS,
	SOUND,
	GRAPHICS,
	UNKNOWN
};


GXMenu*					ts_main_menuOne = 0;
GXMenu*					ts_main_menuTwo = 0;
GXMenu*					ts_main_menuHistory = 0;
GXHudSurface*			ts_main_menuBackground = 0;
GXHudSurface*			ts_main_menuTip = 0;
GXHudSurface*			ts_main_menuParameters = 0;
GXHudSurface*			ts_main_menuTitle = 0;
GXEditBox*				ts_main_menuEditBox = 0;

TSHistory*				ts_main_menuHistoryData = 0;

GXMaterialInfo			ts_main_menuMaterial;

eTSMainMenuInput		ts_main_menuDevice = PC;
eTSMainMenuState		ts_main_menuState = TITLE;
eTSMainMenuState		ts_main_menuOldState = TITLE;
GXUInt					ts_main_menuOldSelected = 0;

GXBool					ts_main_menuInvalidTip = GX_FALSE;
GXBool					ts_main_menuInvalidParameters = GX_FALSE;
GXBool					ts_main_menuInvalidVisuals = GX_FALSE;

GXOGGSoundTrack*		ts_menuFocusSoundTrack = 0;
GXOGGSoundTrack*		ts_menuSelectSoundTrack = 0;
GXSoundEmitter*			ts_menuSoundEmitter = 0;

GXRendererResolutions	ts_main_menuResolutions;
GXUShort				ts_main_menuSelectedResolution;

GXWChar*				ts_main_menuServerAddress = 0;

TSConfig				ts_main_menuConfig;
GXEngineConfiguration	ts_main_menuSettings;

FT_Face					ts_main_menuFont = 0;

//--------------------------------------------------------------------------------------------

GXVoid GXCALL TSMainMenuCreateGame ()
{
	ts_menuSoundEmitter->ChangeSoundTrack ( ts_menuSelectSoundTrack, GX_FALSE, GX_FALSE, GX_TRUE );
	ts_menuSoundEmitter->Play ();
	TSServerInit ();
	TSClientInit ( GX_TRUE );
	ts_SceneKeeper->SwitchScene ( ARIZONA );
}

GXVoid GXCALL TSMainMenuJoinGame ()
{
	ts_menuSoundEmitter->ChangeSoundTrack ( ts_menuSelectSoundTrack, GX_FALSE, GX_FALSE, GX_TRUE );
	ts_menuSoundEmitter->Play ();
	TSClientInit ( GX_FALSE );
	ts_SceneKeeper->SwitchScene ( ARIZONA );
}

GXVoid GXCALL TSMainMenuStartJoinGame ()
{
	ts_main_menuEditBox->ReleaseInput ();
	ts_menuSoundEmitter->ChangeSoundTrack ( ts_menuSelectSoundTrack, GX_FALSE, GX_FALSE, GX_TRUE );
	ts_menuSoundEmitter->Play ();
	ts_main_menuEditBox->MarkAll ();
	ts_main_menuState = JOIN;
}

GXVoid GXCALL TSMainMenuHistoryOnUp ()
{
	ts_main_menuHistory->OnFocusPrevious ();
}

GXVoid GXCALL TSMainMenuHistoryOnDown ()
{
	ts_main_menuHistory->OnFocusNext ();
}

GXVoid GXCALL TSMainMenuSaveServer ();
GXVoid GXCALL TSMainMenuCancelServer ();
GXVoid GXCALL TSMainMenuStartEditServer ()
{
	ts_main_menuEditBox->CaptureInput ();

	ts_Input->BindKeyFunc ( &TSMainMenuSaveServer, VK_RETURN, INPUT_DOWN );
	ts_Input->BindKeyFunc ( &TSMainMenuCancelServer, VK_ESCAPE, INPUT_DOWN );

	ts_Input->BindKeyFunc ( &TSMainMenuHistoryOnUp, VK_UP, INPUT_DOWN );
	ts_Input->BindKeyFunc ( &TSMainMenuHistoryOnDown, VK_DOWN, INPUT_DOWN );
	
	ts_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_B, INPUT_DOWN );
	ts_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_A, INPUT_DOWN );

	ts_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_CROSS_UP, INPUT_DOWN );
	ts_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_CROSS_DOWN, INPUT_DOWN );

	ts_menuSoundEmitter->ChangeSoundTrack ( ts_menuSelectSoundTrack, GX_FALSE, GX_FALSE, GX_TRUE );
	ts_menuSoundEmitter->Play ();
	ts_main_menuState = ts_main_menuOldState = EDIT_SERVER;

	ts_main_menuInvalidTip = GX_TRUE;
}

GXVoid GXCALL TSMainMenuOnTextChanged ()
{
	const GXWChar* key = ts_main_menuEditBox->GetText ();

	if ( key[ 0 ] == 0 )
	{
		if ( ts_main_menuHistory->GetTotalItems () > 0 )
			ts_main_menuHistory->Reset ();
		return;
	}

	TSHistoryResult res;
	ts_main_menuHistoryData->Find ( key, res );
	
	ts_main_menuHistory->Reset ();
	for ( GXUInt i = 0; i < res.numEntries; i++ )
		ts_main_menuHistory->AddItem ( res.entries[ i ], 0 );
}

GXVoid GXCALL TSMainMenuSaveServer ()
{
	const GXWChar* url;

	if ( ts_main_menuHistory->IsFocused () )
	{
		GXUInt item = ts_main_menuHistory->GetFocus ();
		url = ts_main_menuHistory->GetItemText ( item );
	}
	else
	{
		url = ts_main_menuEditBox->GetText ();
	}

	GXUInt size = sizeof ( GXWChar ) * ( GXWcslen ( url ) + 1 );
	GXWriteToFile ( L"../Config/Thesis/TSNetwork.cfg", url, size );

	GXSafeFree ( ts_main_menuServerAddress );
	ts_main_menuServerAddress = (GXWChar*)malloc ( size );
	memcpy ( ts_main_menuServerAddress, url, size );

	ts_main_menuHistoryData->Add ( url );

	if ( ts_main_menuHistory->IsFocused () )
		ts_main_menuEditBox->ReplaceText ( url );

	TSMainMenuStartJoinGame ();
}

GXVoid GXCALL TSMainMenuCancelServer ()
{
	ts_main_menuEditBox->ReplaceText ( ts_main_menuServerAddress );

	TSMainMenuStartJoinGame ();
}

GXVoid GXCALL TSMainMenuStartSettings ()
{
	ts_menuSoundEmitter->ChangeSoundTrack ( ts_menuSelectSoundTrack, GX_FALSE, GX_FALSE, GX_TRUE );
	ts_menuSoundEmitter->Play ();

	ts_Input->UnBindKeyFunc ( VK_RIGHT, INPUT_DOWN );
	ts_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_CROSS_RIGHT, INPUT_DOWN );
	ts_Input->UnBindKeyFunc ( VK_LEFT, INPUT_DOWN );
	ts_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_CROSS_LEFT, INPUT_DOWN );

	ts_main_menuState = SETTINGS;
}

GXVoid GXCALL TSMainMenuStartTitle ()
{
	ts_menuSoundEmitter->ChangeSoundTrack ( ts_menuSelectSoundTrack, GX_FALSE, GX_FALSE, GX_TRUE );
	ts_menuSoundEmitter->Play ();

	ts_main_menuState = TITLE;
}

GXVoid GXCALL TSMainMenuStartGraphics ()
{
	ts_menuSoundEmitter->ChangeSoundTrack ( ts_menuSelectSoundTrack, GX_FALSE, GX_FALSE, GX_TRUE );
	ts_menuSoundEmitter->Play ();

	ts_main_menuState = GRAPHICS;
}

GXVoid GXCALL TSMainMenuSaveVisuals ()
{
	ts_menuSoundEmitter->ChangeSoundTrack ( ts_menuSelectSoundTrack, GX_FALSE, GX_FALSE, GX_TRUE );
	ts_menuSoundEmitter->Play ();

	GXEngineConfiguration cfg;

	cfg.usRendererWidthResoluton = ts_main_menuSettings.usRendererWidthResoluton;
	cfg.usRendererHeightResoluton = ts_main_menuSettings.usRendererHeightResoluton;
	cfg.bVSync = ts_main_menuSettings.bVSync;
	cfg.bIsWindowedMode = ts_main_menuSettings.bIsWindowedMode;
	cfg.chAnisotropy = ts_main_menuSettings.chAnisotropy;
	cfg.chResampling = ts_main_menuSettings.chResampling;

	GXExportCFG ( cfg );

	ts_main_menuInvalidVisuals = GX_TRUE;
}

GXVoid GXCALL TSMainMenuStartSound ()
{
	ts_menuSoundEmitter->ChangeSoundTrack ( ts_menuSelectSoundTrack, GX_FALSE, GX_FALSE, GX_TRUE );
	ts_menuSoundEmitter->Play ();

	ts_main_menuState = SOUND;
}

//--------------------------------------------------------------------------------------------

GXVoid GXCALL TSMainMenuOnFocus ( GXUInt item )
{
	switch ( ts_main_menuState )
	{
		case SOUND:
		{
			if ( ( item == 0 || item == 1 ) && ts_main_menuOldSelected == 3 )
				ts_main_menuInvalidTip = GX_TRUE;
			else if ( ( ts_main_menuOldSelected == 0 || ts_main_menuOldSelected == 1 ) && item == 3 )
				ts_main_menuInvalidTip = GX_TRUE;

			ts_main_menuInvalidParameters = GX_TRUE;
			ts_main_menuOldSelected = item;
		}
		break;

		case GRAPHICS:
		{
			if ( ( item == 0 || item == 4 ) && ( ts_main_menuOldSelected == 7 || ts_main_menuOldSelected == 6 ) )
				ts_main_menuInvalidTip = GX_TRUE;
			else if ( ( ts_main_menuOldSelected == 0 || ts_main_menuOldSelected == 4 ) && ( item == 7 || item == 6 ) )
				ts_main_menuInvalidTip = GX_TRUE;

			ts_main_menuInvalidParameters = GX_TRUE;
			ts_main_menuOldSelected = item;
		}
		break;

		default:
		break;
	}

	ts_menuSoundEmitter->ChangeSoundTrack ( ts_menuFocusSoundTrack, GX_FALSE, GX_FALSE, GX_TRUE );
	ts_menuSoundEmitter->Play ();
}

GXVoid GXCALL TSMainMenuOnUpPC ()
{
	if ( ts_main_menuDevice == XBOX )
	{
		ts_main_menuDevice = PC;
		ts_main_menuInvalidTip = GX_TRUE;
	}

	if ( ts_main_menuState == GRAPHICS )
	{
		ts_main_menuTwo->OnFocusPrevious ();
		ts_main_menuInvalidParameters = GX_TRUE;
	}
	else
		ts_main_menuOne->OnFocusPrevious ();
}

GXVoid GXCALL TSMainMenuOnDownPC ()
{
	if ( ts_main_menuDevice == XBOX )
	{
		ts_main_menuDevice = PC;
		ts_main_menuInvalidTip = GX_TRUE;
	}

	if ( ts_main_menuState == GRAPHICS )
	{
		ts_main_menuTwo->OnFocusNext ();
		ts_main_menuInvalidParameters = GX_TRUE;
	}
	else
		ts_main_menuOne->OnFocusNext ();
}

GXVoid GXCALL TSMainMenuOnRightPC ()
{
	if ( ts_main_menuDevice == XBOX )
	{
		ts_main_menuDevice = PC;
		ts_main_menuInvalidTip = GX_TRUE;
	}

	switch ( ts_main_menuState )
	{
		case SOUND:
		{
			GXUInt focus = ts_main_menuOne->GetFocus ();

			if ( focus == 2 || focus == 3 ) return;

			if ( focus == 0 )
			{
				if ( ts_main_menuConfig.musicVolume < 100.0f )
				{
					ts_main_menuConfig.musicVolume += 10.0f;
					ts_main_menuInvalidParameters = GX_TRUE;
				}
			}
			else
			{
				if ( ts_main_menuConfig.effectVolume < 100.0f )
				{
					ts_main_menuConfig.effectVolume += 10.0f;
					ts_main_menuInvalidParameters = GX_TRUE;
				}
			}
		}
		break;

		case GRAPHICS:
		{
			GXUInt focus = ts_main_menuTwo->GetFocus ();

			switch ( focus )
			{
				case 0:
				{
					//Разрешение
					if ( ts_main_menuSelectedResolution < ( ts_main_menuResolutions.GetTotalResolutions () - 1 ) )
						ts_main_menuSelectedResolution++;
				}
				break;

				case 1:
				{
					//Ресемплинг
					if ( ts_main_menuSettings.chResampling < 2 )
						ts_main_menuSettings.chResampling += 1;
				}
				break;

				case 2:
				{
					//Анизотропия
					if ( ts_main_menuSettings.chAnisotropy < 16 )
						ts_main_menuSettings.chAnisotropy <<= 1;
				}
				break;

				case 3:
				{
					//Оконный режим
					ts_main_menuSettings.bIsWindowedMode = !ts_main_menuSettings.bIsWindowedMode;
				}
				break;

				case 4:
				{
					//Вертикальная синхронизация
					ts_main_menuSettings.bVSync = !ts_main_menuSettings.bVSync;
				}
				break;

				default:
				break;
			}

			ts_main_menuInvalidParameters = GX_TRUE;
		}
		break;

		default:
		break;
	}
}

GXVoid GXCALL TSMainMenuOnLeftPC ()
{
	if ( ts_main_menuDevice == XBOX )
	{
		ts_main_menuDevice = PC;
		ts_main_menuInvalidTip = GX_TRUE;
	}

	switch ( ts_main_menuState )
	{
		case SOUND:
		{
			GXUInt focus = ts_main_menuOne->GetFocus ();

			if ( focus == 2 || focus == 3 ) return;

			if ( focus == 0 )
			{
				if ( ts_main_menuConfig.musicVolume > 0.0f )
				{
					ts_main_menuConfig.musicVolume -= 10.0f;
					ts_main_menuInvalidParameters = GX_TRUE;
				}
			}
			else
			{
				if ( ts_main_menuConfig.effectVolume > 0.0f )
				{
					ts_main_menuConfig.effectVolume -= 10.0f;
					ts_main_menuInvalidParameters = GX_TRUE;
				}
			}
		}
		break;

		case GRAPHICS:
		{
			GXUInt focus = ts_main_menuTwo->GetFocus ();

			switch ( focus )
			{
				case 0:
				{
					//Разрешение
					if ( ts_main_menuSelectedResolution > 0 )
						ts_main_menuSelectedResolution--;
				}
				break;

				case 1:
				{
					//Ресемплинг
					if ( ts_main_menuSettings.chResampling > 0 )
						ts_main_menuSettings.chResampling -= 1;
				}
				break;

				case 2:
				{
					//Анизотропия
					if ( ts_main_menuSettings.chAnisotropy > 1 )
						ts_main_menuSettings.chAnisotropy >>= 1;
				}
				break;

				case 3:
				{
					//Оконный режим
					ts_main_menuSettings.bIsWindowedMode = !ts_main_menuSettings.bIsWindowedMode;
				}
				break;

				case 4:
				{
					//Вертикальная синхронизация
					ts_main_menuSettings.bVSync = !ts_main_menuSettings.bVSync;
				}
				break;

				default:
				break;
			}

			ts_main_menuInvalidParameters = GX_TRUE;
		}
		break;

		default:
		break;
	}
}

GXVoid GXCALL TSMainMenuOnSelectPC ()
{
	if ( ts_main_menuDevice == XBOX )
	{
		ts_main_menuDevice = PC;
		ts_main_menuInvalidTip = GX_TRUE;
	}

	if ( ts_main_menuState == GRAPHICS )
		ts_main_menuTwo->OnSelect ();
	else
		ts_main_menuOne->OnSelect ();
}

GXBool	ts_main_menuFlag = GX_TRUE;
GXVoid GXCALL TSMainMenuOnUpXBOX ()
{
	if ( ts_main_menuFlag )
	{
		ts_main_menuFlag = GX_FALSE;
		return;
	}

	if ( ts_main_menuDevice == PC )
	{
		ts_main_menuDevice = XBOX;
		ts_main_menuInvalidTip = GX_TRUE;
	}

	if ( ts_main_menuState == GRAPHICS )
	{
		ts_main_menuTwo->OnFocusPrevious ();
		ts_main_menuInvalidParameters = GX_TRUE;
	}
	else
		ts_main_menuOne->OnFocusPrevious ();
}

GXVoid GXCALL TSMainMenuOnDownXBOX ()
{
	if ( ts_main_menuDevice == PC )
	{
		ts_main_menuDevice = XBOX;
		ts_main_menuInvalidTip = GX_TRUE;
	}

	if ( ts_main_menuState == GRAPHICS )
	{
		ts_main_menuTwo->OnFocusNext ();
		ts_main_menuInvalidParameters = GX_TRUE;
	}
	else
		ts_main_menuOne->OnFocusNext ();
}

GXVoid GXCALL TSMainMenuOnRightXBOX ()
{
	if ( ts_main_menuDevice == PC )
	{
		ts_main_menuDevice = XBOX;
		ts_main_menuInvalidTip = GX_TRUE;
	}

	switch ( ts_main_menuState )
	{
		case SOUND:
		{
			GXUInt focus = ts_main_menuOne->GetFocus ();

			if ( focus == 2 || focus == 3 ) return;

			if ( focus == 0 )
			{
				if ( ts_main_menuConfig.musicVolume < 100.0f )
				{
					ts_main_menuConfig.musicVolume += 10.0f;
					ts_main_menuInvalidParameters = GX_TRUE;
				}
			}
			else
			{
				if ( ts_main_menuConfig.effectVolume < 100.0f )
				{
					ts_main_menuConfig.effectVolume += 10.0f;
					ts_main_menuInvalidParameters = GX_TRUE;
				}
			}
		}
		break;

		case GRAPHICS:
		{
			GXUInt focus = ts_main_menuTwo->GetFocus ();

			switch ( focus )
			{
				case 0:
				{
					//Разрешение
					if ( ts_main_menuSelectedResolution < ( ts_main_menuResolutions.GetTotalResolutions () - 1 ) )
						ts_main_menuSelectedResolution++;
				}
				break;

				case 1:
				{
					//Ресемплинг
					if ( ts_main_menuSettings.chResampling < 2 )
						ts_main_menuSettings.chResampling += 1;
				}
				break;

				case 2:
				{
					//Анизотропия
					if ( ts_main_menuSettings.chAnisotropy < 16 )
						ts_main_menuSettings.chAnisotropy <<= 1;
				}
				break;

				case 3:
				{
					//Оконный режим
					ts_main_menuSettings.bIsWindowedMode = !ts_main_menuSettings.bIsWindowedMode;
				}
				break;

				case 4:
				{
					//Вертикальная синхронизация
					ts_main_menuSettings.bVSync = !ts_main_menuSettings.bVSync;
				}
				break;
			}

			ts_main_menuInvalidParameters = GX_TRUE;
		}
		break;

		default:
		break;
	}
}

GXVoid GXCALL TSMainMenuOnLeftXBOX ()
{
	if ( ts_main_menuDevice == PC )
	{
		ts_main_menuDevice = XBOX;
		ts_main_menuInvalidTip = GX_TRUE;
	}

	switch ( ts_main_menuState )
	{
		case SOUND:
		{
			GXUInt focus = ts_main_menuOne->GetFocus ();

			if ( focus == 2 || focus == 3 ) return;

			if ( focus == 0 )
			{
				if ( ts_main_menuConfig.musicVolume > 0.0f )
				{
					ts_main_menuConfig.musicVolume -= 10.0f;
					ts_main_menuInvalidParameters = GX_TRUE;
				}
			}
			else
			{
				if ( ts_main_menuConfig.effectVolume > 0.0f )
				{
					ts_main_menuConfig.effectVolume -= 10.0f;
					ts_main_menuInvalidParameters = GX_TRUE;
				}
			}
		}
		break;

		case GRAPHICS:
		{
			GXUInt focus = ts_main_menuTwo->GetFocus ();

			switch ( focus )
			{
				case 0:
				{
					//Разрешение
					if ( ts_main_menuSelectedResolution > 0 )
						ts_main_menuSelectedResolution--;
				}
				break;

				case 1:
				{
					//Ресемплинг
					if ( ts_main_menuSettings.chResampling > 0 )
						ts_main_menuSettings.chResampling -= 1;
				}
				break;

				case 2:
				{
					//Анизотропия
					if ( ts_main_menuSettings.chAnisotropy > 1 )
						ts_main_menuSettings.chAnisotropy >>= 1;
				}
				break;

				case 3:
				{
					//Оконный режим
					ts_main_menuSettings.bIsWindowedMode = !ts_main_menuSettings.bIsWindowedMode;
				}
				break;

				case 4:
				{
					//Вертикальная синхронизация
					ts_main_menuSettings.bVSync = !ts_main_menuSettings.bVSync;
				}
				break;

				default:
				break;
			}

			ts_main_menuInvalidParameters = GX_TRUE;
		}
		break;

		default:
		break;
	}
}

GXVoid GXCALL TSMainMenuOnSelectXBOX ()
{
	if ( ts_main_menuDevice == PC )
	{
		ts_main_menuDevice = XBOX;
		ts_main_menuInvalidTip = GX_TRUE;
	}

	if ( ts_main_menuState == GRAPHICS )
		ts_main_menuTwo->OnSelect ();
	else
		ts_main_menuOne->OnSelect ();
}

//--------------------------------------------------------------------------------------------

GXVoid GXCALL TSMainMenuUpdateTip ()
{
	switch ( ts_main_menuState )
	{
		case TITLE:
		{
			GXUChar id = ( ts_main_menuDevice == PC ) ? 4 : 10;

			GXGLTextureStruct ts;
			ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
			ts.height = ts_main_menuMaterial.textures[ id ].usHeight;
			ts.internalFormat = GL_RGBA8;

			GXImageInfo ii;
			ii.insertWidth = 40;
			ii.insertHeight = 40;
			ii.insertX = 1024 - 68 - 55;
			ii.insertY = 0;
			ii.overlayType = GX_SIMPLE_REPLACE;
			ii.texture = &ts;
			ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

			ts_main_menuTip->Reset ();
			ts_main_menuTip->AddImage ( ii );

			GXVec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );
			GXPenInfo pi;
			pi.fontColor = &white;
			pi.font = ts_main_menuFont;
			pi.insertX = 1024 - 68;
			pi.insertY = 12;
			pi.fontSize = 21;
			pi.kerning = GX_TRUE;

			ts_main_menuTip->AddText ( pi, L"Выбор" );
		}
		break;

		case JOIN:
		case SETTINGS:
		{
			GXUChar id = ( ts_main_menuDevice == PC ) ? 4 : 10;

			GXGLTextureStruct ts;
			ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
			ts.height = ts_main_menuMaterial.textures[ id ].usHeight;
			ts.internalFormat = GL_RGBA8;

			GXImageInfo ii;
			ii.insertWidth = 40;
			ii.insertHeight = 40;
			ii.insertX = 1024 - 68 - 55;
			ii.insertY = 0;
			ii.overlayType = GX_SIMPLE_REPLACE;
			ii.texture = &ts;
			ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

			ts_main_menuTip->Reset ();
			ts_main_menuTip->AddImage ( ii );

			id = ( ts_main_menuDevice == PC ) ? 5 : 11;

			ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
			ts.height = ts_main_menuMaterial.textures[ id ].usHeight;

			ii.insertX = 1024 - 68 - 55 - 80 - 55;
			ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

			ts_main_menuTip->AddImage ( ii );

			GXVec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );
			GXPenInfo pi;
			pi.fontColor = &white;
			pi.font = ts_main_menuFont;
			pi.insertX = 1024 - 68;
			pi.insertY = 12;
			pi.fontSize = 21;
			pi.kerning = GX_TRUE;

			ts_main_menuTip->AddText ( pi, L"Выбор" );

			pi.insertX = 1024 - 68 - 55 - 80;
			ts_main_menuTip->AddText ( pi, L"Назад" );
		}
		break;

		case EDIT_SERVER:
		{
			GXGLTextureStruct ts;
			ts.width = ts_main_menuMaterial.textures[ 16 ].usWidth;
			ts.height = ts_main_menuMaterial.textures[ 16 ].usHeight;
			ts.internalFormat = GL_RGBA8;

			GXImageInfo ii;
			ii.insertWidth = 40;
			ii.insertHeight = 40;
			ii.insertX = 1024 - 68 - 55;
			ii.insertY = 0;
			ii.overlayType = GX_SIMPLE_REPLACE;
			ii.texture = &ts;
			ii.textureID = ts_main_menuMaterial.textures[ 16 ].uiId;

			ts_main_menuTip->Reset ();
			ts_main_menuTip->AddImage ( ii );

			ts.width = ts_main_menuMaterial.textures[ 0 ].usWidth;
			ts.height = ts_main_menuMaterial.textures[ 0 ].usHeight;

			ii.insertX = 1024 - 68 - 55 - 100 - 55;
			ii.textureID = ts_main_menuMaterial.textures[ 0 ].uiId;

			ts_main_menuTip->AddImage ( ii );

			ts.width = ts_main_menuMaterial.textures[ 1 ].usWidth;
			ts.height = ts_main_menuMaterial.textures[ 1 ].usHeight;

			ii.insertX = 1024 - 68 - 55 - 100 - 55 - 42;
			ii.textureID = ts_main_menuMaterial.textures[ 1 ].uiId;

			ts_main_menuTip->AddImage ( ii );

			ts.width = ts_main_menuMaterial.textures[ 4 ].usWidth;
			ts.height = ts_main_menuMaterial.textures[ 4 ].usHeight;

			ii.insertX = 1024 - 68 - 55 - 100 - 55 - 42 - 85 - 55;
			ii.textureID = ts_main_menuMaterial.textures[ 4 ].uiId;

			ts_main_menuTip->AddImage ( ii );

			ts.width = ts_main_menuMaterial.textures[ 5 ].usWidth;
			ts.height = ts_main_menuMaterial.textures[ 5 ].usHeight;

			ii.insertX = 1024 - 68 - 55 - 100 - 55 - 42 - 85 - 55 - 85 - 55;
			ii.textureID = ts_main_menuMaterial.textures[ 5 ].uiId;

			ts_main_menuTip->AddImage ( ii );

			GXVec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );
			GXPenInfo pi;
			pi.fontColor = &white;
			pi.font = ts_main_menuFont;
			pi.insertX = 1024 - 68;
			pi.insertY = 12;
			pi.fontSize = 21;
			pi.kerning = GX_TRUE;

			ts_main_menuTip->AddText ( pi, L"Ввод" );

			pi.insertX = 1024 - 68 - 55 - 100;
			ts_main_menuTip->AddText ( pi, L"История" );

			pi.insertX = 1024 - 68 - 55 - 100 - 55 - 42 - 85;
			ts_main_menuTip->AddText ( pi, L"Готово" );

			pi.insertX = 1024 - 68 - 55 - 100 - 55 - 42 - 85 - 55 - 85;
			ts_main_menuTip->AddText ( pi, L"Отмена" );
		}
		break;

		case GRAPHICS:
		{
			GXUShort focus = ts_main_menuTwo->GetFocus ();

			if ( focus >= 5 )
			{
				GXUChar id = ( ts_main_menuDevice == PC ) ? 4 : 10;

				GXGLTextureStruct ts;
				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;
				ts.internalFormat = GL_RGBA8;

				GXImageInfo ii;
				ii.insertWidth = 40;
				ii.insertHeight = 40;
				ii.insertX = 1024 - 68 - 55;
				ii.insertY = 0;
				ii.overlayType = GX_SIMPLE_REPLACE;
				ii.texture = &ts;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->Reset ();
				ts_main_menuTip->AddImage ( ii );

				id = ( ts_main_menuDevice == PC ) ? 5 : 11;

				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;

				ii.insertX = 1024 - 68 - 55 - 80 - 55;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->AddImage ( ii );

				GXVec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );
				GXPenInfo pi;
				pi.fontColor = &white;
				pi.font = ts_main_menuFont;
				pi.insertX = 1024 - 68;
				pi.insertY = 12;
				pi.fontSize = 21;
				pi.kerning = GX_TRUE;

				ts_main_menuTip->AddText ( pi, L"Выбор" );

				pi.insertX = 1024 - 68 - 55 - 80;
				ts_main_menuTip->AddText ( pi, L"Назад" );
			}
			else
			{
				GXUChar id = ( ts_main_menuDevice == PC ) ? 2 : 8;

				GXGLTextureStruct ts;
				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;
				ts.internalFormat = GL_RGBA8;

				GXImageInfo ii;
				ii.insertWidth = 40;
				ii.insertHeight = 40;
				ii.insertX = 1024 - 110 - 55;
				ii.insertY = 0;
				ii.overlayType = GX_SIMPLE_REPLACE;
				ii.texture = &ts;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->Reset ();
				ts_main_menuTip->AddImage ( ii );

				id = ( ts_main_menuDevice == PC ) ? 3 : 9;

				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;

				ii.insertX = 1024 - 110 - 55 - 42;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->AddImage ( ii );

				id = ( ts_main_menuDevice == PC ) ? 0 : 6;

				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;

				ii.insertX = 1024 - 110 - 55 - 42 - 120 - 55;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->AddImage ( ii );

				id = ( ts_main_menuDevice == PC ) ? 1 : 7;

				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;

				ii.insertX = 1024 - 110 - 55 - 42 - 120 - 55 - 42;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->AddImage ( ii );

				id = ( ts_main_menuDevice == PC ) ? 5 : 11;

				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;

				ii.insertX = 1024 - 110 - 55 - 42 - 120 - 55 - 42 - 75 - 55;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->AddImage ( ii );

				GXVec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );
				GXPenInfo pi;
				pi.fontColor = &white;
				pi.font = ts_main_menuFont;
				pi.insertX = 1024 - 110;
				pi.insertY = 12;
				pi.fontSize = 21;
				pi.kerning = GX_TRUE;

				ts_main_menuTip->AddText ( pi, L"Изменить" );

				pi.insertX = 1024 - 110 - 55 - 45 - 120;
				ts_main_menuTip->AddText ( pi, L"Параметр" );

				pi.insertX = 1024 - 110 - 55 - 45 - 120 - 55 - 45 - 75;
				ts_main_menuTip->AddText ( pi, L"Назад" );	
			}
		}
		break;

		case SOUND:
		{
			if ( ts_main_menuOldSelected == 3 )
			{
				GXUChar id = ( ts_main_menuDevice == PC ) ? 4 : 10;

				GXGLTextureStruct ts;
				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;
				ts.internalFormat = GL_RGBA8;

				GXImageInfo ii;
				ii.insertWidth = 40;
				ii.insertHeight = 40;
				ii.insertX = 1024 - 68 - 55;
				ii.insertY = 0;
				ii.overlayType = GX_SIMPLE_REPLACE;
				ii.texture = &ts;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->Reset ();
				ts_main_menuTip->AddImage ( ii );

				id = ( ts_main_menuDevice == PC ) ? 5 : 11;

				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;

				ii.insertX = 1024 - 68 - 55 - 80 - 55;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->AddImage ( ii );

				GXVec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );
				GXPenInfo pi;
				pi.fontColor = &white;
				pi.font = ts_main_menuFont;
				pi.insertX = 1024 - 68;
				pi.insertY = 12;
				pi.fontSize = 21;
				pi.kerning = GX_TRUE;

				ts_main_menuTip->AddText ( pi, L"Выбор" );

				pi.insertX = 1024 - 68 - 55 - 80;
				ts_main_menuTip->AddText ( pi, L"Назад" );
			}
			else
			{
				GXUChar id = ( ts_main_menuDevice == PC ) ? 2 : 8;

				GXGLTextureStruct ts;
				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;
				ts.internalFormat = GL_RGBA8;

				GXImageInfo ii;
				ii.insertWidth = 40;
				ii.insertHeight = 40;
				ii.insertX = 1024 - 110 - 55;
				ii.insertY = 0;
				ii.overlayType = GX_SIMPLE_REPLACE;
				ii.texture = &ts;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->Reset ();
				ts_main_menuTip->AddImage ( ii );

				id = ( ts_main_menuDevice == PC ) ? 3 : 9;

				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;

				ii.insertX = 1024 - 110 - 55 - 42;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->AddImage ( ii );

				id = ( ts_main_menuDevice == PC ) ? 0 : 6;

				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;

				ii.insertX = 1024 - 110 - 55 - 42 - 120 - 55;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->AddImage ( ii );

				id = ( ts_main_menuDevice == PC ) ? 1 : 7;

				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;

				ii.insertX = 1024 - 110 - 55 - 42 - 120 - 55 - 42;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->AddImage ( ii );

				id = ( ts_main_menuDevice == PC ) ? 5 : 11;

				ts.width = ts_main_menuMaterial.textures[ id ].usWidth;
				ts.height = ts_main_menuMaterial.textures[ id ].usHeight;

				ii.insertX = 1024 - 110 - 55 - 42 - 120 - 55 - 42 - 75 - 55;
				ii.textureID = ts_main_menuMaterial.textures[ id ].uiId;

				ts_main_menuTip->AddImage ( ii );

				GXVec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );
				GXPenInfo pi;
				pi.fontColor = &white;
				pi.font = ts_main_menuFont;
				pi.insertX = 1024 - 110;
				pi.insertY = 12;
				pi.fontSize = 21;
				pi.kerning = GX_TRUE;

				ts_main_menuTip->AddText ( pi, L"Изменить" );

				pi.insertX = 1024 - 110 - 55 - 45 - 120;
				ts_main_menuTip->AddText ( pi, L"Параметр" );

				pi.insertX = 1024 - 110 - 55 - 45 - 120 - 55 - 45 - 75;
				ts_main_menuTip->AddText ( pi, L"Назад" );
			}
		}
		break;

		default:
		break;
	}
}

GXVoid GXCALL TSMainMenuUpdateParameters ()
{
	switch ( ts_main_menuState )
	{
		case SOUND:
		{
			ts_main_menuParameters->SetLocation ( (GXFloat)( -gx_EngineSettings.rendererWidth >> 1 ) + 128.0f + 256.0f, (GXFloat)( -gx_EngineSettings.rendererHeight >> 1 ) + 128.0f + 50.0f + 82.0f, 19.0f );
			ts_main_menuParameters->Reset ();

			GXUInt focus = ts_main_menuOne->GetFocus ();

			GXUChar offset = 2;

			GXVec4 green ( 0.451f, 0.725f, 0.0f, 1.0f );
			GXVec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );
			GXPenInfo pi;
			pi.fontColor = ( focus == 1 ) ? &white : &green;
			pi.font = ts_main_menuFont;
			pi.fontSize = 21;
			pi.insertX = 0;
			pi.insertY = offset;
			pi.kerning = GX_TRUE;

			ts_main_menuParameters->AddText ( pi, L"%i%%", (GXUInt)ts_main_menuConfig.effectVolume );

			pi.fontColor = ( focus == 0 ) ? &white : &green;
			pi.insertY = offset + 27;

			ts_main_menuParameters->AddText ( pi, L"%i%%", (GXUInt)ts_main_menuConfig.musicVolume );
		}
		break;

		case GRAPHICS:
		{
			ts_main_menuParameters->SetLocation ( (GXFloat)( -gx_EngineSettings.rendererWidth >> 1 ) + 256.0f + 30.0f + 256.0f, (GXFloat)( -gx_EngineSettings.rendererHeight >> 1 ) + 128.0f + 131.0f, 19.0f );
			ts_main_menuParameters->Reset ();

			GXUInt focus = ts_main_menuTwo->GetFocus ();

			GXUChar offset = 30;

			GXVec4 green ( 0.451f, 0.725f, 0.0f, 1.0f );
			GXVec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );
			GXPenInfo pi;
			pi.fontColor = ( focus == 4 ) ? &white : &green;
			pi.font = ts_main_menuFont;
			pi.fontSize = 21;
			pi.insertX = 0;
			pi.insertY = offset;
			pi.kerning = GX_TRUE;

			ts_main_menuParameters->AddText ( pi, L"%s", ts_main_menuSettings.bVSync ? L"Да" : L"Нет" );

			offset += 27;
			pi.insertY = offset;
			pi.fontColor = ( focus == 3 ) ? &white : &green;

			ts_main_menuParameters->AddText ( pi, L"%s", ts_main_menuSettings.bIsWindowedMode ? L"Да" : L"Нет" );

			offset += 27;
			pi.insertY = offset;
			pi.fontColor = ( focus == 2 ) ? &white : &green;

			if ( ts_main_menuSettings.chAnisotropy > 1 )
				ts_main_menuParameters->AddText ( pi, L"x%i", (GXInt)ts_main_menuSettings.chAnisotropy );
			else
				ts_main_menuParameters->AddText ( pi, L"Нет" );

			offset += 27;
			pi.insertY = offset;
			pi.fontColor = ( focus == 1 ) ? &white : &green;
			static const GXWChar* resampling[ 3 ] =
			{
				L"Нет",
				L"Билинейный",
				L"Трилинейный"
			};

			ts_main_menuParameters->AddText ( pi, L"%s", resampling[ ts_main_menuSettings.chResampling ] );

			offset += 27;
			pi.insertY = offset;
			pi.fontColor = ( focus == 0 ) ? &white : &green;
			ts_main_menuResolutions.GetResolution ( ts_main_menuSelectedResolution, ts_main_menuSettings.usRendererWidthResoluton, ts_main_menuSettings.usRendererHeightResoluton );
			ts_main_menuParameters->AddText ( pi, L"%ix%i", ts_main_menuSettings.usRendererWidthResoluton, ts_main_menuSettings.usRendererHeightResoluton );
		}
		break;

		default:
		break;
	}
}

GXVoid GXCALL TSMainMenuInitTitle ()
{
	ts_Input->UnBindKeyFunc ( VK_ESCAPE, INPUT_DOWN );
	ts_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_B, INPUT_DOWN );

	ts_main_menuOne->Reset ();

	ts_main_menuOne->AddItem ( L"Создать игру", &TSMainMenuCreateGame );
	ts_main_menuOne->AddItem ( L"Присоединиться", &TSMainMenuStartJoinGame );
	ts_main_menuOne->AddItem ( L"Настройки", &TSMainMenuStartSettings );
	ts_main_menuOne->AddGap ();
	ts_main_menuOne->AddItem ( L"Выход", &TSOnExit );

	ts_main_menuOne->SetFocus ( 0 );

	TSMainMenuUpdateTip ();
}

GXVoid GXCALL TSMainMenuInitJoinGame ()
{
	ts_Input->BindKeyFunc ( &TSMainMenuOnSelectPC, VK_RETURN, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSMainMenuOnSelectXBOX, GX_INPUT_XBOX_A, INPUT_DOWN );

	ts_Input->BindKeyFunc ( &TSMainMenuStartTitle, VK_ESCAPE, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSMainMenuStartTitle, GX_INPUT_XBOX_B, INPUT_DOWN );

	ts_Input->BindKeyFunc ( &TSMainMenuOnUpPC, VK_UP, INPUT_DOWN );
	ts_Input->BindKeyFunc ( &TSMainMenuOnDownPC, VK_DOWN, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSMainMenuOnUpXBOX, GX_INPUT_XBOX_CROSS_UP, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSMainMenuOnDownXBOX, GX_INPUT_XBOX_CROSS_DOWN, INPUT_DOWN );

	ts_main_menuOne->Reset ();

	ts_main_menuOne->AddItem ( L"Сервер:", &TSMainMenuStartEditServer );
	ts_main_menuOne->AddItem ( L"Присоединиться", &TSMainMenuJoinGame );
	ts_main_menuOne->AddGap ();
	ts_main_menuOne->AddItem ( L"Назад", &TSMainMenuStartTitle );

	ts_main_menuOne->SetFocus ( 1 );

	ts_main_menuTitle->Reset ();

	GXVec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );
	GXPenInfo pi;
	pi.fontColor = &white;
	pi.font = ts_main_menuFont;
	pi.fontSize = 52;
	pi.insertX = 0;
	pi.insertY = 12;
	pi.kerning = GX_TRUE;

	ts_main_menuTitle->AddText ( pi, L"Присоединиться" );

	TSMainMenuUpdateTip ();
}

GXVoid GXCALL TSMainMenuInitSettings ()
{
	ts_main_menuOne->Reset ();
	
	ts_main_menuOne->AddItem ( L"Графика", &TSMainMenuStartGraphics );
	ts_main_menuOne->AddItem ( L"Звук", &TSMainMenuStartSound );
	ts_main_menuOne->AddItem ( L"Управление", 0 );
	ts_main_menuOne->AddGap ();
	ts_main_menuOne->AddItem ( L"Назад", &TSMainMenuStartTitle );

	ts_main_menuOne->DisableItem ( 2 );

	ts_main_menuOne->SetFocus ( 0 );

	ts_Input->BindKeyFunc ( &TSMainMenuStartTitle, VK_ESCAPE, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSMainMenuStartTitle, GX_INPUT_XBOX_B, INPUT_DOWN );

	ts_main_menuTitle->Reset ();

	GXVec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );
	GXPenInfo pi;
	pi.fontColor = &white;
	pi.font = ts_main_menuFont;
	pi.fontSize = 52;
	pi.insertX = 0;
	pi.insertY = 12;
	pi.kerning = GX_TRUE;

	ts_main_menuTitle->AddText ( pi, L"Настройки" );

	TSMainMenuUpdateTip ();
}

GXVoid GXCALL TSMainMenuInitGraphics ()
{
	ts_main_menuTwo->SetFocus ( 0 );

	ts_main_menuTitle->Reset ();

	GXVec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );
	GXPenInfo pi;
	pi.fontColor = &white;
	pi.font = ts_main_menuFont;
	pi.fontSize = 52;
	pi.insertX = 0;
	pi.insertY = 12;
	pi.kerning = GX_TRUE;

	ts_main_menuTitle->AddText ( pi, L"Графика" );

	ts_Input->BindKeyFunc ( &TSMainMenuStartSettings, VK_ESCAPE, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSMainMenuStartSettings, GX_INPUT_XBOX_B, INPUT_DOWN );

	ts_Input->BindKeyFunc ( &TSMainMenuOnRightPC, VK_RIGHT, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSMainMenuOnRightXBOX, GX_INPUT_XBOX_CROSS_RIGHT, INPUT_DOWN );
	ts_Input->BindKeyFunc ( &TSMainMenuOnLeftPC, VK_LEFT, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSMainMenuOnLeftXBOX, GX_INPUT_XBOX_CROSS_LEFT, INPUT_DOWN );

	TSMainMenuUpdateTip ();
	TSMainMenuUpdateParameters ();
}

GXVoid GXCALL TSMainMenuInitSound ()
{
	ts_main_menuOne->Reset ();
	
	ts_main_menuOne->AddItem ( L"Музыка", 0 );
	ts_main_menuOne->AddItem ( L"Эффекты", 0 );
	ts_main_menuOne->AddGap ();
	ts_main_menuOne->AddItem ( L"Назад", &TSMainMenuStartSettings );
	
	ts_main_menuOldSelected = 0;
	ts_main_menuOne->SetFocus ( ts_main_menuOldSelected );

	ts_Input->BindKeyFunc ( &TSMainMenuStartSettings, VK_ESCAPE, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSMainMenuStartSettings, GX_INPUT_XBOX_B, INPUT_DOWN );

	ts_Input->BindKeyFunc ( &TSMainMenuOnRightPC, VK_RIGHT, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSMainMenuOnRightXBOX, GX_INPUT_XBOX_CROSS_RIGHT, INPUT_DOWN );
	ts_Input->BindKeyFunc ( &TSMainMenuOnLeftPC, VK_LEFT, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSMainMenuOnLeftXBOX, GX_INPUT_XBOX_CROSS_LEFT, INPUT_DOWN );

	ts_main_menuTitle->Reset ();

	GXVec4 white ( 1.0f, 1.0f, 1.0f, 1.0f );
	GXPenInfo pi;
	pi.fontColor = &white;
	pi.font = ts_main_menuFont;
	pi.fontSize = 52;
	pi.insertX = 0;
	pi.insertY = 12;
	pi.kerning = GX_TRUE;

	ts_main_menuTitle->AddText ( pi, L"Звук" );

	TSMainMenuUpdateTip ();
	TSMainMenuUpdateParameters ();
}

//--------------------------------------------------------------------------------------------

GXVoid GXCALL TSMainMenuOnInit ()
{
	ts_main_menuFont = GXGetFont ( L"../Fonts/trebuc.ttf" );

	GXLoadMTR ( L"../Materials/Thesis/Main_Menu.mtr", ts_main_menuMaterial );
	for ( GXUChar i = 0; i < ts_main_menuMaterial.numTextures; i++ )
		GXGetTexture ( ts_main_menuMaterial.textures[ i ] );

	GXMenuStyle mStyle;
	mStyle.fontSize = 21;
	mStyle.stripe = 27;
	mStyle.selectorOffset = 24.0f;
	mStyle.fontOffset = 20;

	ts_main_menuOne = new GXMenu ( 256, 256, &TSMainMenuOnFocus );
	ts_main_menuOne->SetStyle ( mStyle );

	ts_main_menuOne->SetLocation ( (GXFloat)( -gx_EngineSettings.rendererWidth >> 1 ) + 128.0f + 30.0f, (GXFloat)( -gx_EngineSettings.rendererHeight >> 1 ) + 128.0f + 50.0f, 19.0f );

	ts_main_menuTwo = new GXMenu ( 512, 256, &TSMainMenuOnFocus );
	mStyle.selectorOffset = 23.0f;
	ts_main_menuTwo->SetStyle ( mStyle );
	ts_main_menuTwo->SetLocation ( (GXFloat)( -gx_EngineSettings.rendererWidth >> 1 ) + 256.0f + 30.0f, (GXFloat)( -gx_EngineSettings.rendererHeight >> 1 ) + 128.0f + 50.0f, 19.0f );
	ts_main_menuTwo->AddItem ( L"Разрешение", 0 );
	ts_main_menuTwo->AddItem ( L"Ресемплинг", 0 );
	ts_main_menuTwo->AddItem ( L"Анизотропия", 0 );
	ts_main_menuTwo->AddItem ( L"Оконный режим", 0 );
	ts_main_menuTwo->AddItem ( L"Вертикальная синхронизация", 0 );
	ts_main_menuTwo->AddGap ();
	ts_main_menuTwo->AddItem ( L"Применить", &TSMainMenuSaveVisuals );
	ts_main_menuTwo->AddItem ( L"Назад", &TSMainMenuStartSettings );
	ts_main_menuTwo->SetFocus ( 0 );

	GXUInt size;
	GXLoadFile ( L"../Config/Thesis/TSNetwork.cfg", (GXVoid**)&ts_main_menuServerAddress, size, GX_TRUE );

	GXEditBoxStyle eStyle;
	eStyle.OnTextChanged = &TSMainMenuOnTextChanged;

	ts_main_menuHistoryData = new TSHistory ( L"../Config/Thesis/TSHistory.cfg" );

	ts_main_menuHistory = new GXMenu ( 512, 256 );
	mStyle.selectorOffset = 24.0f;
	mStyle.fontOffset = 0;
	mStyle.padding = 5;
	mStyle.showBackground = GX_FALSE;
	mStyle.inactiveColor = GXVec4 ( 0.5f, 0.5f, 0.5f, 1.0f );
	mStyle.showSelector = GX_FALSE;
	ts_main_menuHistory->SetStyle ( mStyle );
	ts_main_menuHistory->SetLocation ( (GXFloat)( -gx_EngineSettings.rendererWidth >> 1 ) + 256.0f + 30.0f + 256.0f, (GXFloat)( -gx_EngineSettings.rendererHeight >> 1 ) + 128.0f + 50.0f + 111.0f + 25.0f, 19.0f );

	ts_main_menuEditBox = new GXEditBox ( 512, 32 );
	ts_main_menuEditBox->SetTextStyle ( eStyle );
	ts_main_menuEditBox->SetLocation ( (GXFloat)( -gx_EngineSettings.rendererWidth >> 1 ) + 256.0f + 30.0f + 256.0f, (GXFloat)( -gx_EngineSettings.rendererHeight >> 1 ) + 16.0f + 50.0f + 111.0f, 19.0f );
	ts_main_menuEditBox->AddText ( ts_main_menuServerAddress );

	ts_main_menuTip = new GXHudSurface ( 1024, 128 );
	ts_main_menuTip->SetScale ( 512.0f, 64.0f, 1.0f );
	ts_main_menuTip->SetLocation ( ( gx_EngineSettings.rendererWidth >> 1 ) - 512.0f - 10.0f, ( -gx_EngineSettings.rendererHeight >> 1 ) + 32.0f + 45.0f, 20.0f );

	ts_main_menuTitle = new GXHudSurface ( 512, 64 );
	ts_main_menuTitle->SetScale ( 256.0f, 32.0f, 1.0f );
	ts_main_menuTitle->SetLocation ( ( -gx_EngineSettings.rendererWidth >> 1 ) + 256.0f + 140.0f, ( gx_EngineSettings.rendererHeight >> 1 ) - 32.0f - 143.0f, 20.0f );

	ts_main_menuParameters = new GXHudSurface ( 256, 256 );
	ts_main_menuParameters->SetScale ( 128.0f, 128.0f, 1.0f );
	
	ts_main_menuBackground = new GXHudSurface ( 1024, 1024 );
	
	GXGLTextureStruct ts;
	ts.width = ts_main_menuMaterial.textures[ 14 ].usWidth;
	ts.height = ts_main_menuMaterial.textures[ 14 ].usHeight;
	ts.internalFormat = GL_RGBA8;

	GXImageInfo ii;
	ii.insertWidth = 1024;
	ii.insertHeight = 1024;
	ii.insertX = ii.insertY = 0;
	ii.overlayType = GX_SIMPLE_REPLACE;
	ii.texture = &ts;
	ii.textureID = ts_main_menuMaterial.textures[ 14 ].uiId;

	ts_main_menuBackground->AddImage ( ii );
	ts_main_menuBackground->SetScale ( (GXFloat)( gx_EngineSettings.rendererWidth >> 1 ), (GXFloat)( gx_EngineSettings.rendererHeight >> 1 ), 1.0f );
	ts_main_menuBackground->SetLocation ( 0.0f, 0.0f, 20.0f );

	TSMainMenuInitTitle ();

	ts_Input->BindKeyFunc ( &TSMainMenuOnUpPC, VK_UP, INPUT_DOWN );
	ts_Input->BindKeyFunc ( &TSMainMenuOnDownPC, VK_DOWN, INPUT_DOWN );
	ts_Input->BindKeyFunc ( &TSMainMenuOnSelectPC, VK_RETURN, INPUT_DOWN );

	ts_Input->BindGamepadKeyFunc ( &TSMainMenuOnUpXBOX, GX_INPUT_XBOX_CROSS_UP, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSMainMenuOnDownXBOX, GX_INPUT_XBOX_CROSS_DOWN, INPUT_DOWN );
	ts_Input->BindGamepadKeyFunc ( &TSMainMenuOnSelectXBOX, GX_INPUT_XBOX_A, INPUT_DOWN );

	TSLoadConfig ( ts_main_menuConfig );
	GXLoadCFG ( ts_main_menuSettings );

	ts_Renderer->GetSupportedResolutions ( ts_main_menuResolutions );
	GXUShort total = ts_main_menuResolutions.GetTotalResolutions ();
	for ( ts_main_menuSelectedResolution = 0; ts_main_menuSelectedResolution < total; ts_main_menuSelectedResolution++ )
	{
		GXUShort w;
		GXUShort h;

		ts_main_menuResolutions.GetResolution ( ts_main_menuSelectedResolution, w, h );
		if ( w == ts_main_menuSettings.usRendererWidthResoluton && h == ts_main_menuSettings.usRendererHeightResoluton )
			break;
	}

	ts_menuFocusSoundTrack = (GXOGGSoundTrack*)GXGetSoundTrack ( L"../Sounds/Thesis/SFX/Focus.ogg" );
	ts_menuSelectSoundTrack = (GXOGGSoundTrack*)GXGetSoundTrack ( L"../Sounds/Thesis/SFX/Select.ogg" );

	ts_menuSoundEmitter = new GXSoundEmitter ( ts_menuFocusSoundTrack, GX_FALSE, GX_FALSE, GX_TRUE );
	ts_EffectChannel->AddEmitter ( ts_menuSoundEmitter );
}

GXBool GXCALL TSMainMenuOnFrame ( GXFloat delta )
{
	if ( ts_main_menuInvalidVisuals )
	{
		delete ts_GBuffer;

		gx_EngineSettings.rendererWidth = ts_main_menuSettings.usRendererWidthResoluton;
		gx_EngineSettings.rendererHeight = ts_main_menuSettings.usRendererHeightResoluton;
		gx_EngineSettings.resampling = ts_main_menuSettings.chResampling;
		gx_EngineSettings.anisotropy = ts_main_menuSettings.chAnisotropy;
		gx_EngineSettings.vSync = ts_main_menuSettings.bVSync;

		ts_Renderer->SetFullscreen ( !ts_main_menuSettings.bIsWindowedMode );
		ts_Renderer->SetResolution ( ts_main_menuSettings.usRendererWidthResoluton, ts_main_menuSettings.usRendererHeightResoluton ); 
		ts_Renderer->SetVSync ( ts_main_menuSettings.bVSync );

		ts_ScreenCamera->SetProjection ( ts_main_menuSettings.usRendererWidthResoluton, ts_main_menuSettings.usRendererHeightResoluton, 0.1f, 5000.0f );
		ts_GBuffer = new TSGBuffer ( ts_ScreenCamera );

		ts_main_menuInvalidVisuals = GX_FALSE;

		ts_main_menuOldState = UNKNOWN;

		ts_main_menuOne->SetLocation ( (GXFloat)( -gx_EngineSettings.rendererWidth >> 1 ) + 128.0f + 30.0f, (GXFloat)( -gx_EngineSettings.rendererHeight >> 1 ) + 128.0f + 50.0f, 19.0f );
		ts_main_menuTwo->SetLocation ( (GXFloat)( -gx_EngineSettings.rendererWidth >> 1 ) + 256.0f + 30.0f, (GXFloat)( -gx_EngineSettings.rendererHeight >> 1 ) + 128.0f + 50.0f, 19.0f );
		ts_main_menuEditBox->SetLocation ( (GXFloat)( -gx_EngineSettings.rendererWidth >> 1 ) + 256.0f + 30.0f + 256.0f, (GXFloat)( -gx_EngineSettings.rendererHeight >> 1 ) + 16.0f + 50.0f + 111.0f, 19.0f );
		ts_main_menuHistory->SetLocation ( (GXFloat)( -gx_EngineSettings.rendererWidth >> 1 ) + 256.0f + 30.0f + 256.0f, (GXFloat)( -gx_EngineSettings.rendererHeight >> 1 ) + 128.0f + 50.0f + 111.0f + 25.0f, 19.0f );
		ts_main_menuTip->SetLocation ( ( gx_EngineSettings.rendererWidth >> 1 ) - 512.0f - 10.0f, ( -gx_EngineSettings.rendererHeight >> 1 ) + 32.0f + 45.0f, 20.0f );
		ts_main_menuTitle->SetLocation ( ( -gx_EngineSettings.rendererWidth >> 1 ) + 256.0f + 140.0f, ( gx_EngineSettings.rendererHeight >> 1 ) - 32.0f - 143.0f, 20.0f );
		ts_main_menuBackground->SetScale ( (GXFloat)( gx_EngineSettings.rendererWidth >> 1 ), (GXFloat)( gx_EngineSettings.rendererHeight >> 1 ), 1.0f );
	}

	gx_ActiveCamera = ts_ScreenCamera;

	if ( ts_main_menuState != ts_main_menuOldState )
	{
		switch ( ts_main_menuState )
		{
			case TITLE:
				TSMainMenuInitTitle ();
			break;

			case JOIN:
				TSMainMenuInitJoinGame ();
			break;

			case SETTINGS:
				TSMainMenuInitSettings ();
			break;

			case GRAPHICS:
				TSMainMenuInitGraphics ();
			break;

			case SOUND:
				TSMainMenuInitSound ();
			break;

			default:
			break;
		}

		ts_main_menuOldState = ts_main_menuState;
	};

	ts_GBuffer->UpdateExposure ( delta );
	ts_GBuffer->ClearGBuffer ();
	ts_GBuffer->StartLightUpPass ();
	ts_GBuffer->StartCommonPass ();
	ts_GBuffer->StartLightUpPass ();
	ts_GBuffer->CombineSlots ();
	ts_GBuffer->AddBloom ();
	ts_GBuffer->StartHudDepthDependentPass ();
	ts_GBuffer->StartHudDepthIndependentPass ();

	ts_main_menuBackground->Draw ();

	if ( ts_main_menuState != TITLE )
		ts_main_menuTitle->Draw ();

	if ( ts_main_menuInvalidTip )
	{
		TSMainMenuUpdateTip ();
		ts_main_menuInvalidTip = GX_FALSE;
	}
	ts_main_menuTip->Draw ();

	switch ( ts_main_menuState )
	{
		case SOUND:
		{
			if ( ts_main_menuInvalidParameters )
			{
				TSMainMenuUpdateParameters ();
				ts_main_menuInvalidParameters = GX_FALSE;
			}

			ts_main_menuParameters->Draw ();

			ts_main_menuOne->Update ( delta );
			ts_main_menuOne->Draw ();
		}
		break;

		case GRAPHICS:
		{
			if ( ts_main_menuInvalidParameters )
			{
				TSMainMenuUpdateParameters ();
				ts_main_menuInvalidParameters = GX_FALSE;
			}

			ts_main_menuParameters->Draw ();

			ts_main_menuTwo->Update ( delta );
			ts_main_menuTwo->Draw ();
		}
		break;

		case JOIN:
		{
			ts_main_menuEditBox->Update ( delta );
			ts_main_menuEditBox->Draw ();
			ts_main_menuOne->Update ( delta );
			ts_main_menuOne->Draw ();
		}
		break;

		case EDIT_SERVER:
		{
			ts_main_menuOne->Update ( delta );
			ts_main_menuOne->Draw ();
			ts_main_menuEditBox->Update ( delta );
			ts_main_menuEditBox->Draw ();
			ts_main_menuHistory->Update ( delta );
			ts_main_menuHistory->Draw ();
		}
		break;
		
		default:
		{
			ts_main_menuOne->Update ( delta );
			ts_main_menuOne->Draw ();
		}
		break;
	}

	ts_GBuffer->ShowTexture ( TS_OUT );

	return GX_TRUE;
}

GXVoid GXCALL TSMainMenuOnDelete ()
{
	ts_Input->UnBindKeyFunc ( VK_UP, INPUT_DOWN );
	ts_Input->UnBindKeyFunc ( VK_DOWN, INPUT_DOWN );
	ts_Input->UnBindKeyFunc ( VK_RETURN, INPUT_DOWN );

	ts_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_CROSS_UP, INPUT_DOWN );
	ts_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_CROSS_DOWN, INPUT_DOWN );
	ts_Input->UnBindGamepadKeyFunc ( GX_INPUT_XBOX_A, INPUT_DOWN );

	for ( GXUChar i = 0; i < ts_main_menuMaterial.numTextures; i++ )
		GXRemoveTexture ( ts_main_menuMaterial.textures[ i ] );

	GXRemoveFont ( ts_main_menuFont );

	ts_main_menuMaterial.Cleanup ();

	delete ts_main_menuBackground;
	delete ts_main_menuTip;
	delete ts_main_menuTitle;
	delete ts_main_menuParameters;

	ts_main_menuEditBox->ReleaseInput ();
	ts_main_menuEditBox->Delete ();
	ts_main_menuEditBox->Draw ();

	ts_main_menuHistory->Delete ();
	ts_main_menuHistory->Draw ();

	delete ts_main_menuHistoryData;

	GXSafeFree ( ts_main_menuServerAddress );

	ts_main_menuOne->Delete ();
	ts_main_menuOne->Draw ();

	ts_main_menuTwo->Delete ();
	ts_main_menuTwo->Draw ();

	TSConfig config;
	config.musicVolume = (GXFloat)ts_main_menuConfig.musicVolume;
	config.effectVolume = (GXFloat)ts_main_menuConfig.effectVolume;

	TSSaveConfig ( config );

	ts_main_menuResolutions.Cleanup ();

	Sleep ( 500 );

	delete ts_menuSoundEmitter;

	ts_menuFocusSoundTrack->Release ();
	ts_menuSelectSoundTrack->Release ();
}

GXVoid GXCALL TSMainMenuOnPhysics ( GXFloat delta )
{
	//NOTHING
}