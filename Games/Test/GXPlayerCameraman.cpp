//version 1.1

#include "GXPlayerCameraman.h"


GXPlayerCameraman::GXPlayerCameraman ( GXCamera* camera ):
GXCameraman ( camera )
{
	SetLocationOffset ( -4.0f, 6.0f, -15.0f );
}

GXVoid GXPlayerCameraman::SetLocationOffset ( GXFloat x, GXFloat y, GXFloat z )
{
	camLocOffset.x = x;
	camLocOffset.y = y;
	camLocOffset.z = z;
}

GXVoid GXPlayerCameraman::Action ( const GXVec3 &stare_location, const GXQuat &stare_rotation )
{
	//NOTHING
}

GXVoid GXPlayerCameraman::Action ( const GXVec3 &stare_location, const GXMat4 &stare_rotation )
{
	//NOTHING
}

GXVoid GXPlayerCameraman::Action ( GXVec3& stare_location, GXVec3& stare_rotation, GXVoid* arg )
{
	EGXInputMouseFlags* mouse_flags;
	GXVec3* camRotOffset;

	mouse_flags = (EGXInputMouseFlags*)( (GXPointer*)arg )[ 0 ];
	camRotOffset = (GXVec3*)( (GXPointer*)arg )[ 1 ];

	if ( mouse_flags->lmb )
	{
		GXFloat asimuth = stare_rotation.yaw_rad - camRotOffset->pitch_rad;
		if ( camRotOffset->yaw_rad < 0.0f ) camRotOffset->yaw_rad = 0.0f;
		if ( camRotOffset->yaw_rad > GX_MATH_PI ) camRotOffset->yaw_rad = GX_MATH_PI;
		GXFloat zenith = camRotOffset->yaw_rad;
		GXFloat r = 30.0f;
		GXFloat factor = sin ( zenith );
		GXVec3 shiftDir ( factor * sin ( asimuth ), cos ( zenith ), factor * cos ( asimuth ) );
		PxRaycastHit hit;
		GXVec3 origin = GXCreateVec3 ( stare_location.x + 3.0f * shiftDir.x, stare_location.y + 3.0f * shiftDir.y + 10.0f, stare_location.z + 3.0f * shiftDir.z );
		if ( gx_Core->GetPhysics ()->RaycastSingle ( origin, shiftDir, r, hit ) )
			r = hit.distance;
		stare_location = stare_location + ( shiftDir * r );
		stare_location.y += 10.0f;
		stare_rotation.pitch_rad = acos ( shiftDir.y ) - GX_MATH_HALFPI;
		stare_rotation.yaw_rad = asimuth;
		camera->SetLocation ( stare_location.x, stare_location.y, stare_location.z );
		camera->SetRotation ( -stare_rotation.pitch_rad, stare_rotation.yaw_rad + GX_MATH_PI, stare_rotation.roll_rad );		
	}
	else
	{
		GXVec3 back;
		GXVec3 strafe;
		back.z = camLocOffset.z * cos ( stare_rotation.yaw_rad + GX_MATH_PI );
		back.x = camLocOffset.z * sin ( stare_rotation.yaw_rad + GX_MATH_PI );
		strafe.z = camLocOffset.x * cos ( stare_rotation.yaw_rad + GX_MATH_HALFPI );
		strafe.x = camLocOffset.x * sin ( stare_rotation.yaw_rad + GX_MATH_HALFPI );
		GXFloat r = 20.0f;
		GXVec3 shiftDir = GXCreateVec3 ( back.x + strafe.x, camLocOffset.y, back.z + strafe.z );
		GXNormalizeVec3 ( shiftDir );
		PxRaycastHit hit;
		GXVec3 origin = GXCreateVec3 ( stare_location.x + 3.0f * shiftDir.x, stare_location.y + 3.0f * shiftDir.y, stare_location.z + 3.0f * shiftDir.z );
		if ( gx_Core->GetPhysics ()->RaycastSingle ( origin, shiftDir, r, hit ) )
			r = hit.distance;
		GXVec3 myShiftDir ( shiftDir.x, shiftDir.y, shiftDir.z );
		stare_location = stare_location + ( myShiftDir * r );
		camera->SetLocation ( stare_location.x, stare_location.y, stare_location.z );
		camera->SetRotation ( stare_rotation.pitch_rad + camRotOffset->pitch_rad, stare_rotation.yaw_rad + camRotOffset->yaw_rad + GX_MATH_PI, stare_rotation.roll_rad + camRotOffset->roll_rad );
	}
}