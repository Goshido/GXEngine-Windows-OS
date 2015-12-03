#ifndef EM_ACTOR
#define EM_ACTOR


#include <GXCommon/GXMath.h>


class EMActor;
extern EMActor* em_Actors;


class EMActor
{
	private:
		EMActor*		next;
		EMActor*		prev;

	protected:
		GXWChar*		name;
		GXUInt			type;
		GXMat4			origin;
		GXBool			isVisible;

	public:
		EMActor ( const GXWChar* name, GXUInt type, const GXMat4 &origin );
		virtual ~EMActor ();

		virtual GXVoid OnDrawCommonPass ();
		virtual GXVoid OnDrawHudDepthDependentPass ();
		virtual GXVoid OnDrawHudDepthIndependentPass ();
		
		virtual GXVoid OnUpdate ( GXFloat deltaTime );
		virtual GXVoid OnSave ( GXUByte** data, GXUInt &size );
		virtual GXVoid OnLoad ( const GXUByte* data );
		virtual GXUInt OnRequeredSaveSize ();
		virtual GXVoid OnOriginChanged ();

		const GXWChar* GetName ();
		GXUInt GetType ();

		const GXMat4& GetOrigin ();
		GXVoid SetOrigin ( const GXMat4 &origin );

		GXVoid Show ();
		GXVoid Hide ();
};


#endif //EM_ACTOR
