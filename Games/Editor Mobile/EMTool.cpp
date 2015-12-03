#include "EMTool.h"


EMTool::EMTool ()
{
	actor = 0;
}

EMTool::~EMTool ()
{
	//NOTHING
}

GXVoid EMTool::Bind ( EMActor* actor )
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

GXVoid EMTool::OnDrawHudDepthDependentPass ()
{
	//NOTHING
}

GXVoid EMTool::OnDrawHudDepthIndependentPass ()
{
	//NOTHING
}
