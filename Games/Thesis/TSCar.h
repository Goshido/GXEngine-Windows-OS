//version 1.0

#ifndef TS_CAR
#define TS_CAR


#include <GXEngine/GXAttachment.h>
#include "TSRenderObject.h"


class TSCar : public TSRenderObject
{
	public:
		virtual GXVoid SetBodyOrigin ( const GXVec3 &location, const GXMat4 &rotation ) = 0;
		virtual GXVoid SetWheelOrigin ( GXUShort id, const GXVec3 &location, const GXMat4 &rotation ) = 0;

		virtual GXVoid GetBodyOrigin ( GXVec3 &location, GXMat4 &rotation ) = 0;
		virtual GXVoid GetWheelOrigin ( GXUShort id, GXVec3 &location, GXMat4 &rotation ) = 0;
		
		virtual GXAttachment* GetAttachment ( const GXWChar* name ) = 0;
};


#endif //TS_CAR