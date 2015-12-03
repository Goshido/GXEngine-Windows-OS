//version 1.0

#ifndef TS_SKELETAL_MESH
#define TS_SKELETAL_MESH


#include <GXEngine/GXSkeletalMeshQuatExt.h>


class TSSkeletalMesh : public GXSkeletalMeshQuatExt
{
	private:
		GXWChar*	fileName;
		GXWChar*	materialName;

		GLint		mod_view_proj_matLocation;
		GLint		mod_view_matLocation;
		GLint		bonesLocation;

		GXBool		isDelete;

	public:
		TSSkeletalMesh ( const GXWChar* fileName, const GXWChar* materialName );
		virtual GXVoid Draw ();

		GXVoid Delete ();

	protected:
		~TSSkeletalMesh ();

		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif //TS_SKELETAL_MESH
