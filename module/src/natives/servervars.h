#include <stdio.h>
#include <map>
#include <malloc.h>
#include <string.h>

#include "../amx/amx.h"

typedef void (*varFuncType)();

#define SERVER_VARTYPE_FLOAT	0
#define SERVER_VARTYPE_INT		1
#define SERVER_VARTYPE_BOOL	2
#define SERVER_VARTYPE_STRING	3

struct serverVarStruct
{
	uint32_t varType;
	void* varValue;
};

std::map<std::string, serverVarStruct*> varMap;
serverVarStruct* FindVar(char* pVarName);
bool RemoveVar( char* pVarName );
char* GetStringVar(char* pVarName);
void SetStringVar(char* pVarName, char* pString);
void AddVar(char* pVarName, uint32_t varType, uint32_t VarFlags, void* VarPtr, varFuncType VarChangeFunc = NULL);
char* AddStringVar(char* pVarName, uint32_t VarFlags, char* pInitStr, varFuncType VarChangeFunc = NULL);
int GetIntVar(char* pVarName);
void SetIntVar(char* pVarName, int intValue);
bool GetBoolVar(char* pVarName);
void SetBoolVar(char* pVarName, bool boolValue);
float GetFloatVar(char* pVarName);
void SetFloatVar(char* pVarName, float floatValue);

int AMXEXPORT amx_sampVarsInit(AMX *amx);
int AMXEXPORT amx_sampVarsCleanup(AMX *amx);
