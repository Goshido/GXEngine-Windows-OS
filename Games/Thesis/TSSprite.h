//version 1.1

#ifndef TS_SPRITE
#define TS_SPRITE


#include <GXEngine/GXMesh.h>


class TSSprite : public GXMesh
{
	protected:
		GLuint		mod_view_proj_matLocation;
		GXWChar*	material;

		GXBool		isTwoFaced;
		GXBool		isDelete;

	public:
		TSSprite ( GXWChar* material );
		GXVoid Delete ();

		virtual GXVoid Draw ();

		GXVoid SetTwoFaced ( GXBool twoFaced );

	protected:
		virtual ~TSSprite ();

		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();

		virtual GXVoid InitGraphicalResources ();
};


#endif //TS_SPRITE