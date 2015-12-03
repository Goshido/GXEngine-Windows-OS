//verison 1.3

#ifndef TS_EXHAUST_PARTICLES
#define TS_EXHAUST_PARTICLES


#include <GXEngine/GXIdealParticles.h>


class TSExhaustParticles : public GXIdealParticles
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

		GXBool	isEmit;
		GXFloat	inertiaTimer;

	public:
		TSExhaustParticles ( GXIdealParticleSystemParams &params );
		
		virtual GXVoid Draw ();
		virtual GXVoid Update ( GXFloat deltaTime );

		GXVoid EnableEmission ( GXBool emission );

	protected:
		virtual ~TSExhaustParticles ();

		virtual GXVoid Load3DModel ();
		virtual GXVoid InitUniforms ();

		virtual GXVoid InitGraphicalResources ();
};


#endif //TS_EXHAUST_PARTICLES