//version 1.7

#ifndef GX_SHADER_STORAGE_EXT
#define GX_SHADER_STORAGE_EXT


#include "GXOpenGL.h"
#include <GXCommon/GXMTRStructs.h>


GXVoid GXCALL GXGetShaderProgramExt ( GXShaderInfo &shader_info, const GXWChar* vs, const GXWChar* gs, const GXWChar* fs );
GXVoid GXCALL GXGetShaderProgramExt ( GXShaderInfo &shader_info );

GLuint GXCALL GXGetTransformFeedbackShaderProgram ( GXShaderInfo &shader_info, GXUChar numVaryings, const GXChar** varyings, GLenum bufferMode );

GXVoid GXCALL GXRemoveShaderProgramExt ( const GXShaderInfo &shader_info );

GXVoid GXCALL GXTuneShaderSamplers ( GXShaderInfo &shader_info, const GLuint* samplerIndexes, const GLchar** samplerNames, GXUByte numSamplers );
GXUInt GXCALL GXGetTotalShaderStorageObjectsExt ( const GXWChar** lastVS, const GXWChar** lastGS, const GXWChar** lastFS );


#endif //GX_SHADER_STORAGE_EXT
