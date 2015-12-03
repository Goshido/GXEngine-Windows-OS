//version 1.1

#ifndef TS_LAND_CRUISER
#define TS_LAND_CRUISER


#include "TSCar.h"
#include "TSMesh.h"


class TSLandCruiser : public TSCar
{
	private:
		TSMesh*			body;
		TSMesh*			wheels[ 4 ];

		GXAttachment*	attachments[ 6 ];

		GXBool			isDelete;

	public:
		TSLandCruiser ();
		virtual GXVoid Delete ();

		virtual GXVoid OnDraw ( eTSGBufferPass pass );

		virtual GXVoid SetBodyOrigin ( const GXVec3 &location, const GXMat4 &rotation );
		virtual GXVoid SetWheelOrigin ( GXUShort id, const GXVec3 &location, const GXMat4 &rotation );

		virtual GXVoid GetBodyOrigin ( GXVec3 &location, GXMat4 &rotation );
		virtual GXVoid GetWheelOrigin ( GXUShort id, GXVec3 &location, GXMat4 &rotation );
		
		virtual GXAttachment* GetAttachment ( const GXWChar* name );
		
	protected:
		virtual ~TSLandCruiser ();
};


#endif //TS_LAND_CRUISER