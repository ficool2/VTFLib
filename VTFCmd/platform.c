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

#include "stdafx.h"
#include "platform.h"

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

vlBool PathInfo(const vlChar *lpPath, vlBool *pbDirectory)
{
	struct stat Stat;

	if(stat(lpPath, &Stat) != 0)
	{
		return vlFalse;
	}

	if(pbDirectory != 0)
	{
		*pbDirectory = (Stat.st_mode & S_IFDIR) != 0;
	}

	return vlTrue;
}

#ifdef _WIN32

struct tagSDirectorySearch
{
	HANDLE Handle;
	WIN32_FIND_DATA FindData;
	vlBool bPending;
};

SDirectorySearch *DirectoryOpen(const vlChar *lpDirectory, const vlChar *lpWildcard)
{
	vlChar cSearchString[512];
	SDirectorySearch *pSearch;

	sprintf(cSearchString, "%s" PATH_SEPARATOR_STRING "%s", lpDirectory, lpWildcard);

	pSearch = (SDirectorySearch *)malloc(sizeof(SDirectorySearch));

	if(pSearch == 0)
	{
		return 0;
	}

	pSearch->Handle = FindFirstFile(cSearchString, &pSearch->FindData);

	if(pSearch->Handle == INVALID_HANDLE_VALUE)
	{
		free(pSearch);
		return 0;
	}

	pSearch->bPending = vlTrue;

	return pSearch;
}

vlBool DirectoryNext(SDirectorySearch *pSearch, SDirectoryEntry *pEntry)
{
	if(!pSearch->bPending && !FindNextFile(pSearch->Handle, &pSearch->FindData))
	{
		return vlFalse;
	}

	pSearch->bPending = vlFalse;

	strncpy(pEntry->cFileName, pSearch->FindData.cFileName, sizeof(pEntry->cFileName) - 1);
	pEntry->cFileName[sizeof(pEntry->cFileName) - 1] = '\0';
	pEntry->bDirectory = (pSearch->FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

	return vlTrue;
}

vlVoid DirectoryClose(SDirectorySearch *pSearch)
{
	FindClose(pSearch->Handle);
	free(pSearch);
}

#else

#include <dirent.h>
#include <fnmatch.h>

struct tagSDirectorySearch
{
	DIR *pDirectory;
	vlChar cPath[512];
	vlChar cWildcard[260];
};

SDirectorySearch *DirectoryOpen(const vlChar *lpDirectory, const vlChar *lpWildcard)
{
	SDirectorySearch *pSearch = (SDirectorySearch *)malloc(sizeof(SDirectorySearch));

	if(pSearch == 0)
	{
		return 0;
	}

	pSearch->pDirectory = opendir(lpDirectory);

	if(pSearch->pDirectory == 0)
	{
		free(pSearch);
		return 0;
	}

	snprintf(pSearch->cPath, sizeof(pSearch->cPath), "%s", lpDirectory);
	snprintf(pSearch->cWildcard, sizeof(pSearch->cWildcard), "%s", lpWildcard);

	return pSearch;
}

vlBool DirectoryNext(SDirectorySearch *pSearch, SDirectoryEntry *pEntry)
{
	struct dirent *pDirent;

	while((pDirent = readdir(pSearch->pDirectory)) != 0)
	{
		vlChar cFullPath[772];
		vlBool bDirectory = vlFalse;

		if(fnmatch(pSearch->cWildcard, pDirent->d_name, 0) != 0)
		{
			continue;
		}

		snprintf(cFullPath, sizeof(cFullPath), "%s" PATH_SEPARATOR_STRING "%s",
			pSearch->cPath, pDirent->d_name);

		if(pDirent->d_type == DT_DIR)
		{
			bDirectory = vlTrue;
		}
		else if(pDirent->d_type == DT_UNKNOWN)
		{
			PathInfo(cFullPath, &bDirectory);
		}

		snprintf(pEntry->cFileName, sizeof(pEntry->cFileName), "%s", pDirent->d_name);
		pEntry->bDirectory = bDirectory;

		return vlTrue;
	}

	return vlFalse;
}

vlVoid DirectoryClose(SDirectorySearch *pSearch)
{
	closedir(pSearch->pDirectory);
	free(pSearch);
}

#endif
