#include "pch.h"
#include "../framework.h"

#include "Environment.h"

Environment::Environment(void)
{
}

Environment::~Environment(void)
{
}

Environment* Environment::GetInstance(void)
{
	static Environment S_Environnement;
	return &S_Environnement;
}

