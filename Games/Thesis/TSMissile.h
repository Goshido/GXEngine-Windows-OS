//version 1.1

#ifndef TS_MISSILE
#define TS_MISSILE


#include "TSRenderObject.h"
#include "TSMesh.h"


class TSMissile : public TSRenderObject
{
	private:
		TSMesh*				missile;
		GXBool				isDelete;

	public:
		TSMissile ();
		virtual GXVoid Delete ();

		virtual GXVoid OnDraw ( eTSGBufferPass pass );
		
		GXVoid SetOrigin ( const GXVec3 &location, const GXMat4 &rotation );
		GXVoid GetOrigin ( GXVec3 &location, GXQuat &rotation );
		GXVoid GetFireOrigin ( GXVec3 &location, GXMat4 &rotation );

	protected:
		virtual ~TSMissile ();
};


#endif //TS_MISSILE