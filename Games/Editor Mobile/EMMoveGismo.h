#ifndef EM_MOVE_GISMO
#define EM_MOVE_GISMO


#include <GXEngine/GXMesh.h>
#include <GXEngine/GXTextureStorageExt.h>


class EMMoveGismo : public GXMesh
{
	private:
		GXBool		isDeleted;
		GXBool		isVisible;

		GLint		mod_view_proj_matLocation;

		GXTexture	texture;

	public:
		EMMoveGismo ();
		GXVoid Delete ();

		GXVoid Hide ();
		GXVoid Show ();

		virtual GXVoid Draw ();

	protected:
		~EMMoveGismo ();

		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif //EM_MOVE_GISMO
