#include "EMActor.h"
#include <GXCommon/GXStrings.h>


EMActor* em_Actors = 0;


EMActor::EMActor ( const GXWChar* name, GXUInt type, const GXMat4 &origin )
{
	GXWcsclone ( &this->name, name );
	this->type = type;
	memcpy ( &this->origin, &origin, sizeof ( GXMat4 ) );

	isVisible = GX_TRUE;

	prev = 0;
	next = em_Actors;
	if ( em_Actors )
		em_Actors->prev = this;
	em_Actors = this;
}

EMActor::~EMActor ()
{
	if ( next ) next->prev = prev;
	if ( prev ) 
		prev->next = next;
	else
		em_Actors = next;

	free ( name );
}

GXVoid EMActor::OnDrawCommonPass ()
{
	//NOTHING
}

GXVoid EMActor::OnDrawHudDepthDependentPass ()
{
	//NOTHING
}

GXVoid EMActor::OnDrawHudDepthIndependentPass ()
{
	//NOTHING
}

GXVoid EMActor::OnUpdate ( GXFloat deltaTime )
{
	//NOTHING
}

GXVoid EMActor::OnSave ( GXUByte** data, GXUInt &size )
{
	*data = 0;
	size = 0;
}

GXVoid EMActor::OnLoad ( const GXUByte* data )
{
	//NOTHING
}

GXUInt EMActor::OnRequeredSaveSize ()
{
	return 0;
}

GXVoid EMActor::OnOriginChanged ()
{
	//NOTHING
}

const GXWChar* EMActor::GetName ()
{
	return name;
}

GXUInt EMActor::GetType ()
{
	return type;
}

const GXMat4& EMActor::GetOrigin ()
{
	return origin;
}

GXVoid EMActor::SetOrigin ( const GXMat4 &origin )
{
	memcpy ( &this->origin, &origin, sizeof ( GXMat4 ) );
	OnOriginChanged ();
}

GXVoid EMActor::Show ()
{
	isVisible = GX_TRUE;
}

GXVoid EMActor::Hide ()
{
	isVisible = GX_FALSE;
}
