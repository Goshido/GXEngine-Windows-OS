//version 1.3

#include "TSCameraman.h"
#include <GXEngine/GXGlobals.h>


TSCameraman::TSCameraman ( GXCamera* camera ):
GXCameraman ( camera )
{
	physics = gx_Core->GetPhysics ();
	soundMixer = gx_Core->GetSoundMixer ();
}

TSCameraman::~TSCameraman ()
{
	//NOTHING
}

GXVoid TSCameraman::Action ( const GXVec3 &stare_location, const GXQuat &stare_rotation )
{
	GXMat4 rotation;
	rotation.SetRotation ( stare_rotation );
	Action ( stare_location, rotation );
}

GXVoid TSCameraman::Action ( const GXVec3 &stare_location, const GXMat4 &stare_rotation )
{
	GXVec3 vector ( 1.5f, 1.5f, -7.0f ); //Длина вектора 7.31f

	GXMat4 rotation = stare_rotation;
	rotation.yv = GXCreateVec3 ( 0.0f, 1.0f, 0.0f );
	rotation.zv.y = 0.0f;
	GXNormalizeVec3 ( rotation.zv );
	GXCrossVec3Vec3 ( rotation.xv, rotation.yv, rotation.zv );
	
	GXVec3 transform;
	GXMulVec3Mat4AsNormal ( transform, vector, rotation );
	GXVec3 cameraLocation;
	GXSumVec3Vec3 ( cameraLocation, transform, stare_location );

	GXVec3 start = GXCreateVec3 ( cameraLocation.x + rotation.zv.x * 7.31f, cameraLocation.y + rotation.zv.y * 7.31f, cameraLocation.z + rotation.zv.z * 7.31f );
	GXVec3 dir = GXCreateVec3 ( -rotation.zv.x, -rotation.zv.y, -rotation.zv.z );

	PxRaycastHit hit;
	if ( physics->RaycastSingle ( start, dir, 7.51f, hit ) )
		cameraLocation = GXCreateVec3 ( hit.impact.x + rotation.zv.x * 1.2f, hit.impact.y + rotation.zv.y * 1.2f, hit.impact.z + rotation.zv.z * 1.2f ); //Поправка на 1.2f

	camera->SetLocation ( cameraLocation );
	camera->SetRotation ( rotation );

	soundMixer->SetListenerLocation ( cameraLocation );
	soundMixer->SetListenerRotation ( rotation );
}