#include "EMTool.h"


EMTool* em_Tool = 0;


EMTool::EMTool ()
{
	actor = 0;
}

EMTool::~EMTool ()
{
	if ( em_Tool == this )
		UnBind ();
}

GXVoid EMTool::Bind ()
{
	em_Tool = this;
	actor = 0;
}

GXVoid EMTool::SetActor ( EMActor* actor )
{
	this->actor = actor;
}

GXVoid EMTool::UnBind ()
{
	em_Tool = 0;
}

GXVoid EMTool::OnDrawCommonPass ()
{
	//NOTHING
}

GXVoid EMTool::OnDrawHudColorPass ()
{
	//NOTHING
}

GXVoid EMTool::OnDrawHudMaskPass ()
{
	//NOTHING
}
