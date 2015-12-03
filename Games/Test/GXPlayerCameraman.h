//version 1.1

#ifndef GX_PLAYER_CAMERAMAN
#define GX_PLAYER_CAMERAMAN


#include <GXEngine/GXCameraman.h>
#include <GXEngine/GXGlobals.h>


class GXPlayerCameraman : public GXCameraman
{
	private:
		GXVec3 camLocOffset;

	public:
		GXPlayerCameraman ( GXCamera* camera );

		GXVoid SetLocationOffset ( GXFloat x, GXFloat y, GXFloat z );

		GXVoid Action ( GXVec3& stare_location, GXVec3& stare_rotation, GXVoid* arg );

		virtual GXVoid Action ( const GXVec3 &stare_location, const GXQuat &stare_rotation );
		virtual GXVoid Action ( const GXVec3 &stare_location, const GXMat4 &stare_rotation );
};


#endif	//GX_PLAYER_CAMERAMAN