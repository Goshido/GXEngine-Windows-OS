//version 1.3

#ifndef TS_SCENE
#define TS_SCENE


#include <GXEngine/GXMesh.h>
#include <GXEngineDLL/GXPhysX.h>
#include "TSLight.h"
#include "TSGBuffer.h"


struct TSSceneMeshUnit;
struct TSSceneLightUnit;
struct TSScenePhysicalUnit;
class TSScene
{
	private:
		TSGBuffer*				gBuffer;

		TSSceneMeshUnit*		meshes;
		GXUInt					numMeshes;
		GXUInt					maxMeshes;

		TSSceneLightUnit*		lights;
		GXUInt					numLights;
		GXUInt					maxLights;

		TSScenePhysicalUnit*	actors;
		GXUInt					numActors;
		GXUInt					maxActors;
		
	public:
		TSScene ( TSGBuffer* gBuffer );
		virtual ~TSScene ();

		GXVoid DrawMeshes ();
		GXVoid DrawLights ();
		GXVoid DrawLightVolumeGismo ();

		GXVoid LoadScene ( GXWChar* fileName );

		GXVoid AddMesh ( GXMesh* mesh );
		GXVoid AddMesh ( const TSSceneMeshUnit &mesh );
		GXMesh* FindMesh ( const GXWChar* name );
		GXVoid DeleteMesh ( GXMesh* mesh );
		GXVoid DeleteMesh ( const GXWChar* name );

		GXVoid AddLight ( TSLightEmitter* light );
		GXVoid AddLight ( const TSSceneLightUnit &light );
		TSLightEmitter* FindLight ( const GXWChar* name );
		GXVoid DeleteLight ( TSLightEmitter* light );
		GXVoid DeleteLight ( const GXWChar* name );

		GXVoid AddActor ( PxActor* actor );
		GXVoid AddActor ( const TSScenePhysicalUnit &actor );
		PxActor* FindActor ( const GXWChar* name );
		GXVoid DeleteActor ( PxActor* actor );
		GXVoid DeleteActor ( const GXWChar* name );

	private:
		GXVoid DeleteMesh ( GXUInt i );
		GXVoid DeleteLight ( GXUInt i );
		GXVoid DeleteActor ( GXUInt i );
};



#endif //TS_SCENE