//version 1.0

#ifndef GX_POINT_LIGHT
#define GX_POINT_LIGHT


#include <GXCommon/GXMath.h>


struct GXPointLight
{
	GXVec3 position;
	GXVec3 attenuation;
	GXVec4 ambient;
	GXVec4 diffuse;
	GXVec4 specular;
};


#endif //GX_POINT_LIGHT