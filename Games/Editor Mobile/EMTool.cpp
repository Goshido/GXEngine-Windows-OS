#include "EMTool.h"


EMTool::EMTool ()
{
	actor = 0;
}

EMTool::~EMTool ()
{
	//NOTHING
}

GXVoid EMTool::Bind ()
{
	//NOTHING
}

GXVoid EMTool::SetActor ( EMActor* actor )
{
	this->actor = actor;
}

GXVoid EMTool::UnBind ()
{
	//NOTHING
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
