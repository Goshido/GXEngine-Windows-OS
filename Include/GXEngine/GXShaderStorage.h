//version 1.4

#ifndef GX_SHADER_STORAGE
#define GX_SHADER_STORAGE


#include "GXOpenGL.h"
#include <GXCommon/GXMTRStructs.h>


struct GXShaderUnit
{
	GXInt			count_ref;
	GLuint			shaderProgram;
	GXWChar*		shaderFullName;

	GXShaderUnit* next;
	GXShaderUnit* prev;
};


GLuint GXCALL GXGetShaderProgram ( GXShaderInfo &shader_info );
GLuint GXCALL GXGetTransformFeedbackShaderProgram ( GXShaderInfo &shader_info, GXUChar numVaryings, const GXChar** varyings, GLenum bufferMode );
GXVoid GXCALL GXRemoveShaderProgram ( GXShaderInfo &shader_info );
GXUInt GXCALL GXGetTotalShaderStorageObjects ( GXWChar** lastShader );


#endif //GX_SHADERS_STORAGE