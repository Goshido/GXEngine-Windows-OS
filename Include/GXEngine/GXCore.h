//version 1.8

#ifndef GX_CORE
#define GX_CORE


#include "GXInput.h"
#include "GXRenderer.h"
#include "GXSoundMixer.h"
#include "GXNetwork.h"
#include "GXUIExt.h"
#include "GXLocale.h"


typedef GXVoid ( GXCALL* PFNGXONGAMEINITPROC ) ();
typedef GXVoid ( GXCALL* PFNGXONGAMECLOSEPROC ) ();


class GXCore
{
	private:
		static GXRenderer*				renderer;
		static GXInput*					input;
		static GXPhysics*				physics;
		static GXSoundMixer*			soundMixer;
		static GXNetServer*				server;
		static GXNetClient*				client;
		static GXTouchSurface*			touchSurface;
		static GXLocale*				locale;

		static GXBool					loopFlag;
		static PFNGXONGAMEINITPROC		OnGameInit;
		static PFNGXONGAMECLOSEPROC		OnGameClose;

	public:
		GXCore ( PFNGXONGAMEINITPROC onGameInit, PFNGXONGAMECLOSEPROC onGameClose, const GXWChar* gameName );
		~GXCore ();

		GXVoid Start ();
		GXVoid Exit ();

		WNDPROC NotifyGetInputProc ();

		GXRenderer* GetRenderer ();
		GXPhysics* GetPhysics ();
		GXInput* GetInput ();
		GXSoundMixer* GetSoundMixer ();
		GXNetServer* GetNetServer ();
		GXNetClient* GetNetClient ();
		GXTouchSurface* GetTouchSurface ();
		GXLocale* GetLocale ();

	private:
		GXVoid CheckMemoryLeak ();
};


#endif //GX_CORE