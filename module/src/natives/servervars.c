#include "servervars.h"

static serverVarStruct* FindVar(char* pVarName)
{
	std::map<std::string, serverVarStruct*>::iterator itor;
	itor = varMap.find(pVarName);
	if (itor != varMap.end())
	{
		return itor->second;
	}
	return NULL;
}

static bool RemoveVar(char* pVarName)
{
	std::map<std::string, serverVarStruct*>::iterator itor;
	itor = varMap.find(pVarName);

	if (itor != varMap.end())
	{
		varMap.erase( itor );
		return true;
	}
	return false;
}

static char* GetStringVar(char* pVarName)
{
	serverVarStruct* ConVar = FindVar(pVarName);
	if (ConVar != NULL)
	{
		if (ConVar->varType == SERVER_VARTYPE_STRING) // string
			return (char*)ConVar->varValue;
	}
	return NULL;
}

static void SetStringVar(char* pVarName, char* pString)
{
	char varName[100];
	strcpy(varName, pVarName);
	serverVarStruct* ConVar = FindVar(varName);
	if (ConVar != NULL)
	{
		if (ConVar->varType == SERVER_VARTYPE_STRING)
		{
			if (ConVar->varValue != NULL)
				delete [] ConVar->varValue;

			char* str = new char[strlen(pString)+1];
			strcpy(str, pString);
			ConVar->varValue = str;
		}
	}
}

static bool GetBoolVar(char* pVarName)
{
	serverVarStruct* ConVar = FindVar(pVarName);
	if (ConVar != NULL)
	{
		if (ConVar->varType == SERVER_VARTYPE_BOOL)
			return *(bool*)ConVar->varValue;
	}
	return false;
}

static int GetIntVar(char* pVarName)
{
	serverVarStruct* ConVar = FindVar(pVarName);
	if (ConVar != NULL)
	{
		if (ConVar->varType == SERVER_VARTYPE_INT)
			return *(int*)ConVar->varValue;
	}
	return 0;
}

static void SetIntVar(char* pVarName, int intValue)
{
	serverVarStruct* ConVar = FindVar(pVarName);
	if (ConVar != NULL)
	{
		if (ConVar->varType == SERVER_VARTYPE_INT)
			*(int*)ConVar->varValue = intValue;
	}
}

static void SetBoolVar(char* pVarName, bool boolValue)
{
	serverVarStruct* ConVar = FindVar(pVarName);
	if (ConVar != NULL)
	{
		if (ConVar->varType == SERVER_VARTYPE_BOOL)
			*(bool*)ConVar->varValue = boolValue;
	}
}

static float GetFloatVar(char* pVarName)
{
	serverVarStruct* ConVar = FindVar(pVarName);
	if (ConVar != NULL)
	{
		if (ConVar->varType == SERVER_VARTYPE_FLOAT)
			return *(float*)ConVar->varValue;
	}
	return 0.0f;
}

static void SetFloatVar(char* pVarName, float floatValue)
{
	serverVarStruct* ConVar = FindVar(pVarName);
	if (ConVar != NULL)
	{
		if (ConVar->varType == SERVER_VARTYPE_FLOAT)
			*(float*)ConVar->varValue = floatValue;
	}
}

// native GetServerVarAsString(const varname[], buffer[], len)
static cell AMX_NATIVE_CALL ___GetServerVarAsString(AMX *amx, cell *params)
{
	if (CheckNumberOfArguments(amx, params, 3) == false) return 0;
	char *szParam;
	amx_StrParam(amx,params[1],szParam);
	return set_amxstring(amx,params[2],GetStringVariable(szParam),params[3]);
}

// native GetServerVarAsInt(const varname[])
static cell AMX_NATIVE_CALL ___GetServerVarAsInt(AMX *amx, cell *params)
{
	if (CheckNumberOfArguments(amx, params, 1) == false) return 0;
	char *szParam;
	amx_StrParam(amx,params[1],szParam);
	return GetIntVariable(szParam);
}

// native GetServerVarAsBool(const varname[])
static cell AMX_NATIVE_CALL ___GetServerVarAsBool(AMX *amx, cell *params)
{
	if (CheckNumberOfArguments(amx, params, 1) == false) return 0;
	char *szParam;
	amx_StrParam(amx,params[1],szParam);
	return (int)GetBoolVariable(szParam);
}

extern "C" const AMX_NATIVE_INFO sampVars_Natives[] = {
  { "GetServerVarAsString",	___GetServerVarAsString },
  { "GetServerVarAsInt",		___GetServerVarAsInt },
  { "GetServerVarAsBool",		___GetServerVarAsBool },
  { NULL, NULL }
};

int AMXEXPORT amx_sampVarsInit(AMX *amx)
{
	return amx_Register(amx, sampVars_Natives, -1);
}

int AMXEXPORT amx_sampVarsCleanup(AMX *amx)
{
	return AMX_ERR_NONE;
}
