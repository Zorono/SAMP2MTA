#include "misc.h"

// native SHA256_PassHash(password[], salt[], ret_hash[], ret_hash_len);
static cell AMX_NATIVE_CALL ___SHA256_PassHash(AMX *amx, cell *params)
{
    if (CheckNumberOfArguments(amx, params, 3) == false) return 0;
	char* szPassword;
	char* szSalt;
	amx_StrParam(amx, params[1], szPassword);
	amx_StrParam(amx, params[2], szSalt);

	char szResult[65];
	sha256(szPassword, szSalt, szResult, 65);
	return set_amxstring(amx, params[3], szResult, params[4]);
}

// native print(const string[])
static cell AMX_NATIVE_CALL ___print(AMX* amx, cell* params)
{
	if (CheckNumberOfArguments(amx, params, 1) == false) return 0;

	char* msg;
	amx_StrParam(amx, params[1], msg);
	logprintf(msg);
	return 0;
}

// native printf(const format[], {Float,_}:...)
static cell AMX_NATIVE_CALL ___printf(AMX *amx, cell *params)
{
	if (CheckNumberOfArguments(amx, params, 1) == false) return 0;

	logprintf(amx_FormatString(amx, params, 1));
	return 0;
}

extern "C" const AMX_NATIVE_INFO sampMisc_Natives[] = {
  { "SHA256_PassHash",  ___SHA256_PassHash },
  { "print",  ___print },
  { "printf",  ___printf },
  { NULL, NULL }
};

int AMXEXPORT amx_sampMiscInit(AMX *amx)
{
	return amx_Register(amx, sampMisc_Natives, -1);
}

int AMXEXPORT amx_sampMiscCleanup(AMX *amx)
{
	return AMX_ERR_NONE;
}
