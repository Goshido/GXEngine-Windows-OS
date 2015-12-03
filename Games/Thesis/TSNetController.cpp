//version 1.1

#include "TSNetController.h"


TSNetController::TSNetController ( GXUInt clientID, GXUInt objectID )
{
	this->clientID = clientID;
	this->objectID = objectID;
}

TSNetController::~TSNetController ()
{ 
	/*PURE VIRTUAL*/
}

GXUInt TSNetController::GetClientID ()
{
	return clientID;
}

GXUInt TSNetController::GetObjectID ()
{
	return objectID;
}