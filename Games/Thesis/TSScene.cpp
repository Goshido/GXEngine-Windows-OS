//version 1.3

#include "TSScene.h"
#include "TSGrass.h"
#include <GXEngine/GXGlobals.h>
#include <GXCommon/GXLogger.h>
#include <GXCommon/GXFileSystem.h>
#include <GXCommon/GXSCNStructs.h>



#define		TS_SCENE_INCREASE	32


GXUInt ts_scene_MeshCounter = 0;
GXUInt ts_scene_LightCounter = 0;
GXUInt ts_scene_ActorCounter = 0;


struct TSSceneMeshUnit
{
	GXMesh*		mesh;
	GXWChar*	name;
	GXBool		isCastShadows;
};


struct TSSceneLightUnit
{
	TSLightEmitter*		light;
	GXWChar*			name;
};


struct TSScenePhysicalUnit
{
	PxActor*			actor;
	GXWChar*			name;
};

//----------------------------------------------------------------------------------------------------

TSScene::TSScene ( TSGBuffer* gBuffer )
{
	this->gBuffer = gBuffer;

	numMeshes = 0;
	maxMeshes = TS_SCENE_INCREASE;
	meshes = (TSSceneMeshUnit*)malloc ( maxMeshes * sizeof ( TSSceneMeshUnit ) );
	
	numLights = 0;
	maxLights = TS_SCENE_INCREASE;
	lights = (TSSceneLightUnit*)malloc ( maxMeshes * sizeof ( TSSceneLightUnit ) );

	numActors = 0;
	maxActors = TS_SCENE_INCREASE;
	actors = (TSScenePhysicalUnit*)malloc ( maxActors * sizeof ( TSScenePhysicalUnit ) );
}

TSScene::~TSScene ()
{
	while ( numMeshes )
		DeleteMesh ( (GXUInt)0 );
	free ( meshes );

	while ( numLights )
		DeleteLight ( (GXUInt)0 );
	free ( lights );

	while ( numActors )
		DeleteActor ( (GXUInt)0 );
	free ( actors );
}

GXVoid TSScene::DrawMeshes ()
{
	for ( GXUInt i = 0; i < numMeshes; i++ )
		meshes[ i ].mesh->Draw ();
}

GXVoid TSScene::DrawLights ()
{
	for ( GXUInt i = 0; i < numLights; i++ )
		gBuffer->LightBy ( lights[ i ].light );
}

GXVoid TSScene::DrawLightVolumeGismo ()
{
	for ( GXUInt i = 0; i < numLights; i++ )
		lights[ i ].light->DrawLightVolumeGismo ();
}

GXVoid TSScene::LoadScene ( GXWChar* fileName )
{
	GXChar* buffer;
	GXUInt	fileSize;

	if ( !GXLoadFile ( fileName, (GXVoid**)&buffer, fileSize, GX_TRUE ) )
	{
		return;
	}

	GXUInt totalObjects;
	memcpy ( &totalObjects, buffer, sizeof ( GXUInt ) );

	GXPhysics* physics = gx_Core->GetPhysics ();

	GXUInt offset = sizeof ( GXUInt );
	for ( GXUInt i = 0; i < totalObjects; i++ )
	{
		GXSceneObjectHeader* oh = (GXSceneObjectHeader*)( buffer + offset );
		offset += sizeof ( GXSceneObjectHeader );

		switch ( oh->objectType )
		{
			case GX_SCENE_OBJ_TYPE_MESH:
			{
				TSSceneMeshUnit mu;
				mu.name = (GXWChar*)( buffer + oh->objectName );

				GXSceneMeshHeader* mh = (GXSceneMeshHeader*)( buffer + offset );
				offset += sizeof ( GXSceneMeshHeader );

				switch ( mh->meshClassID )
				{
					case 0:
						mu.mesh = new TSMesh ( (GXWChar*)( buffer + mh->meshFile ), (GXWChar*)( buffer + mh->meshCache ), (GXWChar*)( buffer + mh->materialFile ), mh->twoSided );
					break;

					case 1:
						mu.mesh = new TSGrass ( (GXWChar*)( buffer + mh->meshFile ), (GXWChar*)( buffer + mh->meshCache ), (GXWChar*)( buffer + mh->materialFile ), mh->twoSided );
					break;
				}
				
				mu.mesh->SetRotation ( mh->rotation );
				mu.mesh->SetScale ( mh->scale );
				mu.mesh->SetLocation ( mh->location );
				mu.isCastShadows = mh->castShadows;

				if ( mu.isCastShadows )
				{
					ts_light_Shadowmap->IncludeShadowCaster ( mu.mesh );
					ts_light_CascadeShadowmap->IncludeShadowCaster ( mu.mesh );
				}

				AddMesh ( mu );
			}
			break;

			case GX_SCENE_OBJ_TYPE_BULP:
			{
				TSSceneLightUnit lu;
				lu.name = (GXWChar*)( buffer + oh->objectName );

				TSBulp* bulp = new TSBulp ();
				lu.light = bulp;

				GXSceneBulpHeader* bh = (GXSceneBulpHeader*)( buffer + offset );
				offset += sizeof ( GXSceneBulpHeader );

				bulp->SetLocation ( bh->location );
				bulp->SetColor ( bh->color.r, bh->color.g, bh->color.b );
				bulp->SetIntensity ( bh->intensity );
				bulp->SetInfluenceDistance ( bh->influence );

				AddLight ( lu );
			}
			break;

			case GX_SCENE_OBJ_TYPE_SPOT_LIGHT:
			{
				TSSceneLightUnit lu;
				lu.name = (GXWChar*)( buffer + oh->objectName );

				GXSceneSpotLightHeader* sh = (GXSceneSpotLightHeader*)( buffer + offset );
				offset += sizeof ( GXSceneSpotLightHeader );

				TSSpotlight* spot = new TSSpotlight ( sh->genShadows );
				lu.light = spot;

				spot->SetColor ( sh->color.r, sh->color.g, sh->color.b );
				spot->SetIntensity ( sh->intensity );

				spot->SetConeAngle ( sh->cutoff_rad );
				spot->SetInfluenceDistance ( sh->influence );

				spot->SetRotation ( sh->rotation );
				spot->SetLocation ( sh->location );

				AddLight ( lu );
			}
			break;

			case GX_SCENE_OBJ_TYPE_DIR_LIGHT:
			{
				TSSceneLightUnit lu;
				lu.name = (GXWChar*)( buffer + oh->objectName );

				GXSceneDirectedLightHeader* dh = (GXSceneDirectedLightHeader*)( buffer + offset );
				offset += sizeof ( GXSceneDirectedLightHeader );

				TSDirectedLight* directed = new TSDirectedLight ( dh->genShadows );
				lu.light = directed;

				directed->SetColor ( dh->color.r, dh->color.g, dh->color.b );
				directed->SetIntensity ( dh->intensity );

				directed->SetRotation ( dh->rotation );

				AddLight ( lu );
			}
			break;

			case GX_SCENE_OBJ_TYPE_PHYSICAL_BOX:
			{
				GXScenePhysicalBoxHeader* pbh = (GXScenePhysicalBoxHeader*)( buffer + offset );
				offset += sizeof ( GXScenePhysicalBoxHeader );

				GXQuat q;
				q = GXCreateQuat ( pbh->rotation );
				GXQuat physXQuat;
				GXQuatRehandCoordinateSystem ( physXQuat, q );
				
				PxRigidStatic* box = physics->CreateRigidStatic ( 0, PxTransform ( PxVec3 ( pbh->location.x, pbh->location.y, pbh->location.z ), PxQuat ( physXQuat.x, physXQuat.y, physXQuat.z, physXQuat.w ) ) );
				PxMaterial* mat = physics->CreateMaterial ( 0.5f, 0.3f, 0.5f );
				PxShape* boxShape = box->createShape ( PxBoxGeometry ( pbh->width * 0.5f, pbh->height * 0.5f, pbh->length * 0.5f ), *mat );

				PxFilterData filterData;
				filterData.word0 = GX_PHYSICS_COLLUSION_GROUP_DRIVABLE;
				filterData.word1 = GX_PHYSICS_COLLUSION_GROUP_CHASSIS | GX_PHYSICS_COLLUSION_GROUP_OBSTACLE;
				filterData.word3 = GX_PHYSICS_RAYCAST_DRIVABLE_SURFACE;

				boxShape->setSimulationFilterData ( filterData );
				boxShape->setQueryFilterData ( filterData );

				physics->AddActor ( *box );

				TSScenePhysicalUnit pu;
				pu.actor = box;
				pu.name = (GXWChar*)( buffer + oh->objectName );

				AddActor ( pu );
			}
			break;

			case GX_SCENE_OBJ_TYPE_PHYSICAL_SPHERE:
			{
				GXScenePhysicalSphereHeader* psh = (GXScenePhysicalSphereHeader*)( buffer + offset );
				offset += sizeof ( GXScenePhysicalSphereHeader );

				PxRigidStatic* sphere = physics->CreateRigidStatic ( 0, PxTransform ( PxVec3 ( psh->location.x, psh->location.y, psh->location.z ) ) );
				PxMaterial* mat = physics->CreateMaterial ( 0.5f, 0.3f, 0.5f );
				PxShape* sphereShape = sphere->createShape ( PxSphereGeometry ( psh->radius ), *mat );

				PxFilterData filterData;
				filterData.word0 = GX_PHYSICS_COLLUSION_GROUP_DRIVABLE;
				filterData.word1 = GX_PHYSICS_COLLUSION_GROUP_CHASSIS | GX_PHYSICS_COLLUSION_GROUP_OBSTACLE;
				filterData.word3 = GX_PHYSICS_RAYCAST_DRIVABLE_SURFACE;

				sphereShape->setSimulationFilterData ( filterData );
				sphereShape->setQueryFilterData ( filterData );

				physics->AddActor ( *sphere );

				TSScenePhysicalUnit pu;
				pu.actor = sphere;
				pu.name = (GXWChar*)( buffer + oh->objectName );

				AddActor ( pu );
			}
			break;

			case GX_SCENE_OBJ_TYPE_PHYSICAL_CAPSULE:
			{
				GXScenePhysicalCapsuleHeader* pch = (GXScenePhysicalCapsuleHeader*)( buffer + offset );
				offset += sizeof ( GXScenePhysicalCapsuleHeader );

				GXQuat q;
				q = GXCreateQuat ( pch->rotation );
				GXQuat physXQuat;
				GXQuatRehandCoordinateSystem ( physXQuat, q );

				PxRigidStatic* capsule = physics->CreateRigidStatic ( 0, PxTransform ( PxVec3 ( pch->location.x, pch->location.y, pch->location.z ) ) );
				PxTransform relativePose ( PxQuat ( PxHalfPi, PxVec3 ( 0.0f, 0.0f, 1.0f ) ) );

				PxMaterial* mat = physics->CreateMaterial ( 0.5f, 0.3f, 0.5f );

				PxShape* capsuleShape = capsule->createShape ( PxCapsuleGeometry ( pch->radius, pch->height * 0.5f - pch->radius  ), *mat );
				capsuleShape->setLocalPose ( relativePose );

				PxFilterData filterData;
				filterData.word0 = GX_PHYSICS_COLLUSION_GROUP_DRIVABLE;
				filterData.word1 = GX_PHYSICS_COLLUSION_GROUP_CHASSIS | GX_PHYSICS_COLLUSION_GROUP_OBSTACLE;
				filterData.word3 = GX_PHYSICS_RAYCAST_DRIVABLE_SURFACE;

				capsuleShape->setSimulationFilterData ( filterData );
				capsuleShape->setQueryFilterData ( filterData );

				physics->AddActor ( *capsule );

				TSScenePhysicalUnit pu;
				pu.actor = capsule;
				pu.name = (GXWChar*)( buffer + oh->objectName );

				AddActor ( pu );
			}
			break;

			case GX_SCENE_OBJ_TYPE_PHYSICAL_MESH:
			{
				GXScenePhysicalMeshHeader* pmh = (GXScenePhysicalMeshHeader*)( buffer + offset );
				offset += sizeof ( GXScenePhysicalMeshHeader );

				GXQuat q;
				q = GXCreateQuat ( pmh->rotation );
				GXQuat physXQuat;
				GXQuatRehandCoordinateSystem ( physXQuat, q );

				PxMaterial* mat = physics->CreateMaterial ( 0.5f, 0.3f, 0.5f );

				PxRigidStatic* mesh = physics->CreateRigidStatic ( 0, PxTransform ( PxTransform ( PxVec3 ( pmh->location.x, pmh->location.y, pmh->location.z ), PxQuat ( physXQuat.x, physXQuat.y, physXQuat.z, physXQuat.w ) ) ) );
				PxTriangleMesh* trm = physics->CreateTriangleMesh ( (GXWChar*)( buffer + pmh->bakeFile ) );
				PxShape* meshShape = mesh->createShape ( PxTriangleMeshGeometry ( trm ), *mat );

				PxFilterData filterData;
				filterData.word0 = GX_PHYSICS_COLLUSION_GROUP_DRIVABLE;
				filterData.word1 = GX_PHYSICS_COLLUSION_GROUP_CHASSIS | GX_PHYSICS_COLLUSION_GROUP_OBSTACLE;
				filterData.word3 = GX_PHYSICS_RAYCAST_DRIVABLE_SURFACE;

				meshShape->setSimulationFilterData ( filterData );
				meshShape->setQueryFilterData ( filterData );

				physics->AddActor ( *mesh );

				TSScenePhysicalUnit pu;
				pu.actor = mesh;
				pu.name = (GXWChar*)( buffer + oh->objectName );

				AddActor ( pu );
			}
			break;
		}
	}

	free ( buffer );
}

GXVoid TSScene::AddMesh ( GXMesh* mesh )
{
	GXUInt i = numMeshes;

	numMeshes++;
	if ( numMeshes >= maxMeshes )
	{
		GXUInt size = maxMeshes * sizeof ( TSSceneMeshUnit );
		maxMeshes += TS_SCENE_INCREASE;
		TSSceneMeshUnit* temp = (TSSceneMeshUnit*)malloc ( maxMeshes * sizeof ( TSSceneMeshUnit ) );
		memcpy ( temp, meshes, size );
		free ( meshes );
		meshes = temp;
	}

	GXWChar b[ 50 ];
	_snwprintf_s ( b, 49, L"Mesh%i", ts_scene_MeshCounter++ );
	GXUInt stringSize = sizeof ( GXWChar ) * ( wcslen ( b ) + 1 );

	meshes[ i ].name = (GXWChar*)malloc ( stringSize );
	memcpy ( meshes[ i ].name, b, stringSize );

	meshes[ i ].mesh = mesh;
}

GXVoid TSScene::AddMesh ( const TSSceneMeshUnit &mesh )
{
	GXUInt i = numMeshes;

	numMeshes++;
	if ( numMeshes >= maxMeshes )
	{
		GXUInt size = maxMeshes * sizeof ( TSSceneMeshUnit );
		maxMeshes += TS_SCENE_INCREASE;
		TSSceneMeshUnit* temp = (TSSceneMeshUnit*)malloc ( maxMeshes * sizeof ( TSSceneMeshUnit ) );
		memcpy ( temp, meshes, size );
		free ( meshes );
		meshes = temp;
	}

	GXUInt stringSize = sizeof ( GXWChar ) * ( wcslen ( mesh.name ) + 1 );
	meshes[ i ].name = (GXWChar*)malloc ( stringSize );
	memcpy ( meshes[ i ].name, mesh.name, stringSize );

	meshes[ i ].mesh = mesh.mesh;
}

GXMesh* TSScene::FindMesh ( const GXWChar* name )
{
	for ( GXUInt i = 0; i < numMeshes; i++ )
	{
		if ( wcscmp ( name, meshes[ i ].name ) == 0 )
			return meshes[ i ].mesh;
	}

	return 0;
}

GXVoid TSScene::DeleteMesh ( GXMesh* mesh )
{
	for ( GXUInt i = 0; i < numMeshes; i++ )
	{
		if ( meshes[ i ].mesh == mesh )
		{
			DeleteMesh ( i );
			return;
		}
	}
}

GXVoid TSScene::DeleteMesh ( const GXWChar* name )
{
	for ( GXUInt i = 0; i < numMeshes; i++ )
	{
		if ( wcscmp ( name, meshes[ i ].name ) == 0 )
		{
			DeleteMesh ( i );
			return;
		}
	}
}

GXVoid TSScene::AddLight ( TSLightEmitter* light )
{
	GXUInt i = numLights;

	numLights++;
	if ( numLights >= maxLights )
	{
		GXUInt size = maxLights * sizeof ( TSSceneLightUnit );
		maxLights += TS_SCENE_INCREASE;
		TSSceneLightUnit* temp = (TSSceneLightUnit*)malloc ( maxLights * sizeof ( TSSceneLightUnit ) );
		memcpy ( temp, lights, size );
		free ( lights );
		lights = temp;
	}

	GXWChar b[ 50 ];
	_snwprintf_s ( b, 49, L"Light%i", ts_scene_LightCounter++ );
	GXUInt stringSize = sizeof ( GXWChar ) * ( wcslen ( b ) + 1 );

	lights[ i ].name = (GXWChar*)malloc ( stringSize );
	memcpy ( lights[ i ].name, b, stringSize );

	lights[ i ].light = light;
}

GXVoid TSScene::AddLight ( const TSSceneLightUnit &light )
{
	GXUInt i = numLights;

	numLights++;
	if ( numLights >= maxLights )
	{
		GXUInt size = maxLights * sizeof ( TSSceneLightUnit );
		maxLights += TS_SCENE_INCREASE;
		TSSceneLightUnit* temp = (TSSceneLightUnit*)malloc ( maxLights * sizeof ( TSSceneLightUnit ) );
		memcpy ( temp, lights, size );
		free ( lights );
		lights = temp;
	}

	GXUInt stringSize = sizeof ( GXWChar ) * ( wcslen ( light.name ) + 1 );
	lights[ i ].name = (GXWChar*)malloc ( stringSize );
	memcpy ( lights[ i ].name, light.name, stringSize );

	lights[ i ].light = light.light;
}

TSLightEmitter* TSScene::FindLight ( const GXWChar* name )
{
	for ( GXUInt i = 0; i < numLights; i++ )
	{
		if ( wcscmp ( lights[ i ].name, name ) == 0 )
			return lights[ i ].light;
	}

	return 0;
}

GXVoid TSScene::DeleteLight ( TSLightEmitter* light )
{
	for ( GXUInt i = 0; i < numLights; i++ )
	{
		if ( lights[ i ].light == light )
		{
			DeleteLight ( i );
			return;
		}
	}
}

GXVoid TSScene::DeleteLight ( const GXWChar* name )
{
	for ( GXUInt i = 0; i < numLights; i++ )
	{
		if ( wcscmp ( name, lights[ i ].name ) == 0 )
		{
			DeleteLight ( i );
			return;
		}
	}
}

GXVoid TSScene::AddActor ( PxActor* actor )
{
	GXUInt i = numActors;

	numActors++;
	if ( numActors >= maxActors )
	{
		GXUInt size = maxActors * sizeof ( TSScenePhysicalUnit );
		maxActors += TS_SCENE_INCREASE;
		TSScenePhysicalUnit* temp = (TSScenePhysicalUnit*)malloc ( maxActors * sizeof ( TSScenePhysicalUnit ) );
		memcpy ( temp, actors, size );
		free ( actors );
		actors = temp;
	}

	GXWChar b[ 50 ];
	_snwprintf_s ( b, 49, L"Actor%i", ts_scene_ActorCounter++ );
	GXUInt stringSize = sizeof ( GXWChar ) * ( wcslen ( b ) + 1 );

	actors[ i ].name = (GXWChar*)malloc ( stringSize );
	memcpy ( actors[ i ].name, b, stringSize );

	actors[ i ].actor = actor;
}

GXVoid TSScene::AddActor ( const TSScenePhysicalUnit &actor )
{
	GXUInt i = numActors;

	numActors++;
	if ( numActors >= maxActors )
	{
		GXUInt size = maxActors * sizeof ( TSScenePhysicalUnit );
		maxActors += TS_SCENE_INCREASE;
		TSScenePhysicalUnit* temp = (TSScenePhysicalUnit*)malloc ( maxActors * sizeof ( TSScenePhysicalUnit ) );
		memcpy ( temp, actors, size );
		free ( actors );
		actors = temp;
	}

	GXUInt stringSize = sizeof ( GXWChar ) * ( wcslen ( actor.name ) + 1 );
	actors[ i ].name = (GXWChar*)malloc ( stringSize );
	memcpy ( actors[ i ].name, actor.name, stringSize );

	actors[ i ].actor = actor.actor;
}

PxActor* TSScene::FindActor ( const GXWChar* name )
{
	for ( GXUInt i = 0; i < numActors; i++ )
	{
		if ( wcscmp ( name, actors[ i ].name ) == 0 )
			return actors[ i ].actor;
	}

	return 0;
}

GXVoid TSScene::DeleteActor ( PxActor* actor )
{
	for ( GXUInt i = 0; i < numActors; i++ )
	{
		if ( actors[ i ].actor == actor )
		{
			DeleteActor ( i );
			return;
		}
	}
}

GXVoid TSScene::DeleteActor ( const GXWChar* name )
{
	for ( GXUInt i = 0; i < numActors; i++ )
	{
		if ( wcscmp ( name, actors[ i ].name ) == 0 )
		{
			DeleteActor ( i );
			return;
		}
	}
}

GXVoid TSScene::DeleteMesh ( GXUInt i )
{
	GXUInt	was = numMeshes;
	numMeshes--;

	if ( meshes[ i ].isCastShadows )
		ts_light_Shadowmap->ExcludeShadowCaster ( meshes[ i ].mesh );

	free ( meshes[ i ].name );
	delete meshes[ i ].mesh;
			
	if ( ( maxMeshes - numMeshes ) >= TS_SCENE_INCREASE )
	{
		TSSceneMeshUnit* temp = (TSSceneMeshUnit*)malloc ( numMeshes * sizeof ( TSSceneMeshUnit ) );
		memcpy ( temp, meshes, i * sizeof ( TSSceneMeshUnit ) );
		memcpy ( temp + i, meshes + i + 1, ( was - i - 1 ) * sizeof ( TSSceneMeshUnit ) );
		free ( meshes );
		meshes = temp;

		maxMeshes = numMeshes;
	}
	else
	{
		memcpy ( meshes + i, meshes + i + 1, ( was - i - 1 ) * sizeof ( TSSceneMeshUnit ) );
	}
}

GXVoid TSScene::DeleteLight ( GXUInt i )
{
	GXUInt	was = numLights;
	numLights--;

	free ( lights[ i ].name );
	delete lights[ i ].light;

	if ( ( maxLights - numLights ) >= TS_SCENE_INCREASE )
	{
		TSSceneLightUnit* temp = (TSSceneLightUnit*)malloc ( numLights * sizeof ( TSSceneLightUnit ) );
		memcpy ( temp, lights, i * sizeof ( TSSceneLightUnit ) );
		memcpy ( temp + i, lights + i + 1, ( was - i - 1 ) * sizeof ( TSSceneLightUnit ) );
		free ( lights );
		lights = temp;

		maxLights = numLights;
	}
	else
	{
		memcpy ( lights + i, lights + i + 1, ( was - i - 1 ) * sizeof ( TSSceneLightUnit ) );
	}
}

GXVoid TSScene::DeleteActor ( GXUInt i )
{
	GXUInt	was = numActors;
	numActors--;

	free ( actors[ i ].name );
	actors[ i ].actor->release ();

	if ( ( maxActors - numActors ) >= TS_SCENE_INCREASE )
	{
		TSScenePhysicalUnit* temp = (TSScenePhysicalUnit*)malloc ( numActors * sizeof ( TSScenePhysicalUnit ) );
		memcpy ( temp, actors, i * sizeof ( TSScenePhysicalUnit ) );
		memcpy ( temp + i, actors + i + 1, ( was - i - 1 ) * sizeof ( TSScenePhysicalUnit ) );
		free ( actors );
		actors = temp;

		maxActors = numActors;
	}
	else
	{
		memcpy ( actors + i, actors + i + 1, ( was - i - 1 ) * sizeof ( TSScenePhysicalUnit ) );
	}
}