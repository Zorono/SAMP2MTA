int setenv_portable(const char* name, const char* value, int overwrite) {
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(_WIN64)
	if (!overwrite) {
		const char* envvar = getenv_portable(name);
		if (envvar != NULL) {
			return 1; //It's not null, we succeeded, don't set it
		}
	} //Otherwise continue and set it anyway
	return _putenv_s(name, value);
#elif defined(LINUX) || defined(FREEBSD) || defined(__FreeBSD__) || defined(__OpenBSD__)
	return setenv(name, value, overwrite);
#endif
}

//Credit: https://stackoverflow.com/questions/4130180/how-to-use-vs-c-getenvironmentvariable-as-cleanly-as-possible
const char* getenv_portable(const char* name)
{
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(_WIN64)
	const DWORD buffSize = 65535;
	static char buffer[buffSize];
	if (GetEnvironmentVariableA(name, buffer, buffSize))
	{
		return buffer;
	}
	else
	{
		return 0;
	}
#elif defined(LINUX) || defined(FREEBSD) || defined(__FreeBSD__) || defined(__OpenBSD__)
	return getenv(name);
#endif
}

#ifndef WIN32
	void *getProcAddr ( HMODULE hModule, const char *szProcName )
	{
		char *szError = NULL;
		dlerror ();
		void *pFunc = dlsym ( hModule, szProcName );
		if ( ( szError = dlerror () ) != NULL )
			return NULL;
		return pFunc;
	}
#endif

std::string ToUTF8(const char * str)
{
	int strLen = strlen(str);
	int newstrLen = mbstowcs(NULL, str, strLen);
	wchar_t *dest = new wchar_t[newstrLen+1];

	mbstowcs(dest, str, strLen);
	dest[newstrLen] = 0;
	std::string newstr = utf8_wcstombs(dest);
	delete[] dest;
	return newstr;
}

std::string ToOriginalCP(const char * str)
{
    /*iconv_t conv = iconv_open("CP1251","UTF-8");
    iconv(conv, (const char**)&str, (size_t*)&strLen, &pOut, (size_t*)&newstrLen);
    iconv_close(conv);*/

	std::wstring newstr = utf8_mbstowcs(str);

	int strLen = strlen(str);
	int newstrLen = wcstombs(NULL, newstr.c_str(), newstr.length());
	char *dest = new char[newstr.length()+1];

	wcstombs(dest, newstr.c_str(), newstr.length());
	dest[newstr.length()] = 0;

	std::string retstr = dest;
    delete[] dest;
	return retstr;
}

void lua_pushamxstring(lua_State* luaVM, AMX* amx, cell *physaddr) {
	if(!physaddr) {
		lua_pushnil(luaVM);
		return;
	}

	int strLen;
	amx_StrLen(physaddr, &strLen);

	char *str = new char[strLen+1];
	amx_GetString(str, physaddr, 0, strLen+1);

	std::string newstr = ToUTF8(str);

	lua_pushlstring(luaVM, newstr.c_str(), newstr.length());
	delete[] str;
}



void lua_pushamxstring(lua_State *luaVM, AMX *amx, cell addr) {
	cell *physaddr;

	amx_GetAddr(amx, addr, &physaddr);
	lua_pushamxstring(luaVM, amx, physaddr);
}

void lua_pushremotevalue(lua_State *localVM, lua_State *remoteVM, int index, bool toplevel) {
	bool seenTableList = false;

	switch(lua_type(remoteVM, index)) {
		case LUA_TNIL: {
			lua_pushnil(localVM);
			break;
		}
		case LUA_TBOOLEAN: {
			lua_pushboolean(localVM, lua_toboolean(remoteVM, index));
			break;
		}
		case LUA_TNUMBER: {
			lua_pushnumber(localVM, lua_tonumber(remoteVM, index));
			break;
		}
		case LUA_TSTRING: {
			size_t len;
			const char *str = lua_tolstring(remoteVM, index, &len);
			std::string newstr = ToUTF8(str);
			lua_pushlstring(localVM, newstr.c_str(), newstr.length());
			break;
		}
		case LUA_TTABLE: {
			if(toplevel && !seenTableList) {
				lua_newtable(localVM);
				lua_setfield(localVM, LUA_REGISTRYINDEX, "_dstSeenTables");
				lua_newtable(remoteVM);
				lua_setfield(remoteVM, LUA_REGISTRYINDEX, "_srcSeenTables");
				seenTableList = true;
			}

			if(index < 0)
				index = lua_gettop(remoteVM) + index + 1;

			lua_getfield(remoteVM, LUA_REGISTRYINDEX, "_srcSeenTables");

			lua_pushvalue(remoteVM, index);
			lua_gettable(remoteVM, -2);
			if(!lua_isnil(remoteVM, -1)) {
				lua_Number tblNum = lua_tonumber(remoteVM, -1);
				lua_pop(remoteVM, 2);
				lua_getfield(localVM, LUA_REGISTRYINDEX, "_dstSeenTables");
				lua_pushnumber(localVM, tblNum);
				lua_gettable(localVM, -2);
				lua_remove(localVM, -2);
				break;
			}
			lua_pop(remoteVM, 1);

			lua_newtable(localVM);
			lua_getfield(localVM, LUA_REGISTRYINDEX, "_dstSeenTables");
			lua_Number tblNum = lua_objlen(localVM, -1) + 1;
			lua_pushnumber(localVM, tblNum);
			lua_pushvalue(localVM, -3);
			lua_settable(localVM, -3);
			lua_pop(localVM, 1);

			lua_pushvalue(remoteVM, index);
			lua_pushnumber(remoteVM, tblNum);
			lua_settable(remoteVM, -3);
			lua_pop(remoteVM, 1);

			lua_pushnil(remoteVM);
			while(lua_next(remoteVM, index)) {
				lua_pushremotevalue(localVM, remoteVM, -2, false);
				lua_pushremotevalue(localVM, remoteVM, -1, false);
				lua_settable(localVM, -3);
				lua_pop(remoteVM, 1);
			}
			break;
		}
		case LUA_TUSERDATA:
		case LUA_TLIGHTUSERDATA: {
			lua_pushlightuserdata(localVM, lua_touserdata(remoteVM, index));
			break;
		}
		default: {
			lua_pushboolean(localVM, 0);
			break;
		}
	}
	if(toplevel && seenTableList) {
		lua_pushnil(localVM);
		lua_setfield(localVM, LUA_REGISTRYINDEX, "_dstSeenTables");
		lua_pushnil(remoteVM);
		lua_setfield(remoteVM, LUA_REGISTRYINDEX, "_srcSeenTables");
	}
}

void lua_pushremotevalues(lua_State *localVM, lua_State *remoteVM, int num) {
	for(int i = -num; i < 0; i++) {
		lua_pushremotevalue(localVM, remoteVM, i);
	}
}

vector<AMX *> getResourceAMXs(lua_State *luaVM) {
	vector<AMX *> amxs;
	for (const auto& it : loadedAMXs) {
		if (it.second.resourceVM == luaVM)
			amxs.push_back(it.first);
	}
	return amxs;
}

bool isSafePath(const char *path) {
	return path && !strstr(path, "..") && !strchr(path, ':') && !strchr(path, '|') && path[0] != '\\' && path[0] != '/';
}

int set_amxstring(AMX *amx,cell amx_addr,const char *source,int max)
{
  cell* physaddr;
  amx_GetAddr(amx, amx_addr, &physaddr);
  if(!physaddr)
    return 0;
  cell* start = physaddr;
  while (max--&&*source)
    *physaddr++=(cell)*source++;
  *physaddr = 0;
  return physaddr-start;
}

char* amx_FormatString(AMX* amx, cell* params, int32_t parm)
{
    static char outbuf[4096];
	atcprintf(outbuf, sizeof(outbuf) - 1, get_amxaddr(amx, params[parm++]), amx, params, &parm);
    return outbuf;
}

char *GetCurrentNativeFunctionName(AMX *amx) // http://pro-pawn.ru/showthread.php?14522
{
    #if (6 <= CUR_FILE_VERSION) && (CUR_FILE_VERSION <= 8)
		const cell OP_SYSREQ_C = 123, OP_SYSREQ_D = 135;
    #elif CUR_FILE_VERSION == 9
		const cell OP_SYSREQ_C = 123, OP_SYSREQ_D = 158, OP_SYSREQ_ND = 159;
		const cell OP_SYSREQ_N = 135;
    #elif CUR_FILE_VERSION == 10
		const cell OP_SYSREQ_C = 123, OP_SYSREQ_D = 213, OP_SYSREQ_ND = 214;
		const cell OP_SYSREQ_N = 135;
    #elif CUR_FILE_VERSION == 11
		const cell OP_SYSREQ_C = 69, OP_SYSREQ_D = 75, OP_SYSREQ_ND = 76;
		const cell OP_SYSREQ_N = 112;
    #else
		#error Unsupported version of AMX instruction set.
    #endif

		AMX_HEADER *hdr = (AMX_HEADER *)amx->base;
		AMX_FUNCSTUB *natives =
			(AMX_FUNCSTUB *)((size_t)hdr + (size_t)hdr->natives);
		const size_t defsize = (size_t)hdr->defsize;
		const cell num_natives =
			(cell)(hdr->libraries - hdr->natives) / (cell)defsize;
		AMX_FUNCSTUB *func = NULL;
    #ifndef AMX_FLAG_OVERLAY
		unsigned char *code = amx->base + (size_t)(hdr->cod);
    #else
		unsigned char *code = amx->code;
    #endif
		cell op_addr, opcode;

		static cell *jump_table = NULL;
		static bool jump_table_checked = false;
		if (!jump_table_checked)
		{
			// On Pawn 4.0 there's no clear way to get the jump table,
			// so only the ANSI C version of the interpreter core is supported.
    #if CUR_FILE_VERSION < 11
			// Set the AMX_FLAG_BROWSE flag and call amx_Exec.
			// If there's no jump table (ANSI C version) amx_Exec would just
			// stumble upon the HALT instruction at address 0 and return.
			const int flags_bck = amx->flags;
			const cell cip_bck = amx->cip;
			const cell pri_bck = amx->pri;
	#if defined AMX_FLAG_BROWSE
			amx->flags |= AMX_FLAG_BROWSE;
	#else
			amx->flags |= AMX_FLAG_VERIFY;
	#endif
			amx->pri = 0;
			amx->cip = 0;
			amx_Exec(amx, (cell *)(size_t)&jump_table, AMX_EXEC_CONT);
			amx->cip = cip_bck;
			amx->pri = pri_bck;
			amx->flags = flags_bck;
    #endif // CUR_FILE_VERSION < 11
			jump_table_checked = true;
		}

    #ifdef AMX_FLAG_SYSREQN
		if (amx->flags & AMX_FLAG_SYSREQN)
		{
			op_addr = amx->cip - 3 * sizeof(cell);
			if (op_addr < 0)
				goto ret;
			opcode = *(cell *)(code + (size_t)op_addr);
			if (jump_table != NULL)
			{
				if (opcode == jump_table[OP_SYSREQ_N])
					goto sysreq_c;
				if (opcode == jump_table[OP_SYSREQ_ND])
					goto sysreq_d;
				goto ret;
			}
			if (opcode == OP_SYSREQ_N)
				goto sysreq_c;
			if (opcode == OP_SYSREQ_ND)
				goto sysreq_d;
			goto ret;
		}
    #endif

		op_addr = amx->cip - 2 * (cell)sizeof(cell);
		if (op_addr < 0)
			goto ret;
		opcode = *(cell *)(void *)(code + (size_t)op_addr);

		if ((jump_table != NULL)
			? (opcode == jump_table[OP_SYSREQ_C]) : (opcode == OP_SYSREQ_C))
		{
    #ifdef AMX_FLAG_SYSREQN
	sysreq_c:
    #endif
			const cell func_index =
				*(cell *)(void *)(code + (size_t)op_addr + sizeof(cell));
			if (func_index < num_natives)
				func = (AMX_FUNCSTUB *)((unsigned char *)(void *)natives +
					(size_t)func_index * (size_t)hdr->defsize);
			goto ret;
		}
		if ((jump_table != NULL)
			? (opcode == jump_table[OP_SYSREQ_D]) : (opcode == OP_SYSREQ_D))
		{
    #ifdef AMX_FLAG_SYSREQN
    sysreq_d:
    #endif
			const ucell func_addr =
				*(ucell *)(void *)(code + (size_t)op_addr + sizeof(cell));
			func = natives;
			size_t libraries = (size_t)amx->base + (size_t)hdr->libraries;
			for (; (size_t)natives < libraries; *((size_t *)&func) += defsize)
				if (func->address == func_addr)
					goto ret;
			func = NULL;
			goto ret;
		}

	ret:
		static char str_unknown[] = "(unknown)";
		if (NULL == func)
			return str_unknown;
    #if CUR_FILE_VERSION < 11
		if (hdr->defsize == (int16_t)sizeof(AMX_FUNCSTUB))
			return (char *)func->name;
		return (char *)
			((size_t)hdr + (size_t)((AMX_FUNCSTUBNT *)func)->nameofs);
    #else
		return (char *)((size_t)hdr + (size_t)func->nameofs);
    #endif
}

bool CheckNumberOfArguments(AMX *amx, const cell *params, int num_expected, bool funcF = false)
{
    if ((int)params[0] < (num_expected * (int)sizeof(cell))) {
        amx_RaiseError(amx, AMX_ERR_PARAMS);
        logprintf("%s: Incorrect number of arguments (expected %d%s, got %d).", GetCurrentNativeFunctionName(amx), num_expected, (funcF ? "+" : ""), ((int)params[0] / (int)sizeof(cell)));
        return false;
    }
    return true;
}
