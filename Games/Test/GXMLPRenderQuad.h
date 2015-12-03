//version 1.9

#ifndef GX_MLP_RENDER_QUAD
#define GX_MLP_RENDER_QUAD


#include <GXEngine/GXMesh.h>


class GXMLPRenderQuad : public GXMesh
{
	private:
		GLuint	fbo;

		GLuint	depthTexture;
		GLuint	normalTexture;
		GLuint	figurationTexture;
		GLuint	diffuseTexture;
		GLuint	preoutTexture;

		GLuint	dofParamsLocation;
		GLuint	projParamsLocation;

		GLuint	projParamsCombineLayersLocation;

		GXVec4	clearColor;

		GXVec3	dofParams;

	public:
		GXMLPRenderQuad ();
		virtual ~GXMLPRenderQuad ();

		virtual GXVoid Draw ();
		GXVoid CombineLayers ();

		GLuint GetColorTexture ();
		GLuint GetDepthTexture ();
		GLuint GetDiffuseTexture ();
		GLuint GetFigurationTexture ();

		GLuint GetFBO ();

		GXVoid StartFigurationPass ();
		GXVoid StartCommonPass ();
		GXVoid StartTransparencyPass ();

		GXVoid SetClearColor ( const GXVec4 &clearColor );
		GXVoid ClearAllBuffers ();

		GXVoid SetDofParams ( const GXVec3 &dofParams );

	protected:
		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();
};


#endif	//GX_MLP_RENDER_QUAD