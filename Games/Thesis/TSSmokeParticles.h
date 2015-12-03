//verison 1.0

#ifndef TS_SMOKE_PARTICLES
#define TS_SMOKE_PARTICLES


#include <GXEngine/GXIdealParticles.h>


class TSSmokeParticles : public GXIdealParticles
{
	private:
		GLuint			deltaTimeLocation;
		GLuint			delayTimeLocation;
		GLuint			maxLifeTimeLocation;
		GLuint			invLifeTimeLocation;
		GLuint			accelerationLocation;
		GLuint			deviationLocation;
		GLuint			maxVelocityLocation;
		GLuint			startPositionLocation;
		GLuint			startDirectionLocation;
		GLuint			mod_matLocation;
		GLuint			local_rot_matLocation;
		GLuint			view_proj_matLocation;
		GLuint			particleSizeLocation;
		GLuint			stopEmitLocation;

		GXBool			isEmit;
		GXFloat			inertiaTimer;

		GXWChar*		material;

	public:
		TSSmokeParticles ( GXIdealParticleSystemParams &params, const GXWChar* material );

		virtual GXVoid Draw ();
		virtual GXVoid Update ( GXFloat deltaTime );

		GXVoid EnableEmission ( GXBool emission );

	protected:
		virtual ~TSSmokeParticles();

		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();

		virtual GXVoid InitGraphicalResources ();
};


#endif //TS_SMOKE_PARTICLES