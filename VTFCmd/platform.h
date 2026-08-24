/*
 * VTFCmd
 * Copyright (C) 2005-2026 ficool2, Neil Jedrzejewski & Ryan Gregg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include "../lib/VTFLib.h"

#ifdef _WIN32
#	define PATH_SEPARATOR			'\\'
#	define PATH_SEPARATOR_STRING	"\\"
#	define DEFAULT_WILDCARD			"*.*"
#else
#	define PATH_SEPARATOR			'/'
#	define PATH_SEPARATOR_STRING	"/"
#	define DEFAULT_WILDCARD			"*"
#endif

typedef struct tagSDirectoryEntry
{
	vlChar cFileName[260];
	vlBool bDirectory;
} SDirectoryEntry;

typedef struct tagSDirectorySearch SDirectorySearch;

//! Test whether a path exists, and if so whether it is a directory.
vlBool PathInfo(const vlChar *lpPath, vlBool *pbDirectory);

//! Begin enumerating the entries of lpDirectory matching lpWildcard.
SDirectorySearch *DirectoryOpen(const vlChar *lpDirectory, const vlChar *lpWildcard);

//! Fetch the next matching entry; returns vlFalse when the search is exhausted.
vlBool DirectoryNext(SDirectorySearch *pSearch, SDirectoryEntry *pEntry);

//! Release a search returned by DirectoryOpen().
vlVoid DirectoryClose(SDirectorySearch *pSearch);

#endif
