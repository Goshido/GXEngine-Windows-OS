#include "EMUI.h"
#include "EMGlobals.h"


EMUI* em_UIElements = 0;


EMUI::EMUI ()
{
	prev = 0;
	next = em_UIElements;
	if ( em_UIElements )
		em_UIElements->prev = this;
	em_UIElements = this;
}

EMUI::~EMUI ()
{
	if ( next ) next->prev = prev;
	if ( prev ) 
		prev->next = next;
	else
		em_UIElements = next;
}

GXVoid EMUI::OnDraw ()
{
	//NOTHING
}

GXVoid EMUI::OnDrawMask ()
{
	//NOTHING
}

//--------------------------------------------------------------

GXVoid GXCALL EMDrawUI ()
{
	for ( EMUI* ui = em_UIElements; ui; ui = ui->next )
		ui->OnDraw ();
}

GXVoid GXCALL EMDrawUIMasks ()
{
	em_Renderer->SetObjectMask ( 0x00000000 );

	for ( EMUI* ui = em_UIElements; ui; ui = ui->next )
		ui->OnDrawMask ();
}
