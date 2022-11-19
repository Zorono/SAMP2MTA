#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "../amx/amx.h"
#include "../sqlite/sqlite3.h"

//--------------------------------------------------------------------------------------

typedef struct _SQLiteResult
{
	int nRows;
	int nColumns;
	char **pResults;
	char *szErrMsg;
	int nCurrentRow;

} SQLiteResult;

int AMXEXPORT amx_sampDbInit(AMX *amx);
int AMXEXPORT amx_sampDbCleanup(AMX *amx);
