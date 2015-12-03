//version 1.1

#ifndef TS_EXPLOSION_PARTICLES
#define TS_EXPLOSION_PARTICLES


#include <GXEngine/GXIdealParticles.h>


class TSExplosionParticles : public GXIdealParticles
{
	private:
		GLuint	deltaTimeLocation;
		GLuint	delayTimeLocation;
		GLuint	maxLifeTimeLocation;
		GLuint	invLifeTimeLocation;
		GLuint	accelerationLocation;
		GLuint	deviationLocation;
		GLuint	maxVelocityLocation;
		GLuint	startPositionLocation;
		GLuint	startDirectionLocation;
		GLuint	mod_matLocation;
		GLuint	local_rot_matLocation;
		GLuint	view_proj_matLocation;
		GLuint	particleSizeLocation;
		GLuint	stopEmitLocation;

	public:
		TSExplosionParticles ( GXIdealParticleSystemParams &params );

		virtual GXVoid Draw ();

	protected:
		virtual ~TSExplosionParticles ();

		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();

		virtual GXVoid InitGraphicalResources ();
};


#endif //TS_EXPLOSION_PARTICLES