//version 1.3

#ifndef TS_CAMERAMAN
#define TS_CAMERAMAN


#include <GXEngine/GXCameraman.h>
#include <GXEngine/GXCore.h>


class TSCameraman : public GXCameraman
{
	private:
		GXPhysics*		physics;
		GXSoundMixer*	soundMixer;

	public:
		TSCameraman ( GXCamera* camera );
		virtual ~TSCameraman ();

		virtual GXVoid Action ( const GXVec3 &stare_location, const GXQuat &stare_rotation );
		virtual GXVoid Action ( const GXVec3 &stare_location, const GXMat4 &stare_rotation );
};


#endif //TS_CAMERAMAN