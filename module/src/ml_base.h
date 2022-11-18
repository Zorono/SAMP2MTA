/*********************************************************
*
*  Multi Theft Auto: San Andreas - Deathmatch
*
*  ml_base, External lua add-on module
*
*  Copyright © 2003-2008 MTA.  All Rights Reserved.
*
*  Grand Theft Auto is © 2002-2003 Rockstar North
*
*  THE FOLLOWING SOURCES ARE PART OF THE MULTI THEFT
*  AUTO SOFTWARE DEVELOPMENT KIT AND ARE RELEASED AS
*  OPEN SOURCE FILES. THESE FILES MAY BE USED AS LONG
*  AS THE DEVELOPER AGREES TO THE LICENSE THAT IS
*  PROVIDED WITH THIS PACKAGE.
*
*********************************************************/

/** MODULE SPECIFIC INFORMATION **/

#ifndef MLBASE_H
#define MLBASE_H

/** MODULE SPECIFIC INFORMATION **/
#define MODULE_NAME			"AMX compatibility layer(SAMP2MTA)"
#define	MODULE_AUTHOR		"arc_"
#define MODULE_VERSION		1.1
#define MODULE_VERSIONSTRING "MTA AMX compatibility layer RC2"

int AMXCallPublicFilterScript(char *fnName);
int AMXCallPublicGameMode(char *fnName);

typedef struct {
	std::string filePath;
	std::string resourceName;
	lua_State *resourceVM;
} AMXPROPS;

#endif
