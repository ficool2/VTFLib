/*
 * VTFLib
 * Copyright (C) 2005-2010 Neil Jedrzejewski & Ryan Gregg

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later
 * version.
 */

#include "VTFLib.h"
#include "FileReader.h"

using namespace VTFLib;
using namespace VTFLib::IO::Readers;

CFileReader::CFileReader(const vlChar *cFileName)
{
	this->hFile = NULL;

	this->cFileName = new vlChar[strlen(cFileName) + 1];
	strcpy(this->cFileName, cFileName);
}

CFileReader::~CFileReader()
{
	this->Close();

	delete []this->cFileName;
}

vlBool CFileReader::Opened() const
{
	return this->hFile != NULL;
}

vlBool CFileReader::Open()
{
	this->Close();

#ifdef _WIN32
	this->hFile = CreateFile(this->cFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(this->hFile == INVALID_HANDLE_VALUE)
	{
		this->hFile = NULL;

		LastError.Set("Error opening file.", vlTrue);

		return vlFalse;
	}
#else
	this->hFile = fopen(this->cFileName, "rb");

	if(this->hFile == NULL)
	{
		LastError.Set("Error opening file.", vlTrue);

		return vlFalse;
	}
#endif

	return vlTrue;
}

vlVoid CFileReader::Close()
{
	if(this->hFile != NULL)
	{
#ifdef _WIN32
		CloseHandle(this->hFile);
#else
		fclose(this->hFile);
#endif
		this->hFile = NULL;
	}
}

vlUInt CFileReader::GetStreamSize() const
{
	if(this->hFile == NULL)
	{
		return 0;
	}

#ifdef _WIN32
	return GetFileSize(this->hFile, NULL);
#else
	long lPosition = ftell(this->hFile);

	if(lPosition < 0 || fseek(this->hFile, 0, SEEK_END) != 0)
	{
		return 0;
	}

	long lSize = ftell(this->hFile);

	fseek(this->hFile, lPosition, SEEK_SET);

	return lSize < 0 ? 0 : (vlUInt)lSize;
#endif
}

vlUInt CFileReader::GetStreamPointer() const
{
	if(this->hFile == NULL)
	{
		return 0;
	}

#ifdef _WIN32
	return (vlUInt)SetFilePointer(this->hFile, 0, NULL, FILE_CURRENT);
#else
	long lPosition = ftell(this->hFile);

	return lPosition < 0 ? 0 : (vlUInt)lPosition;
#endif
}

vlUInt CFileReader::Seek(vlLong lOffset, vlUInt uiMode)
{
	if(this->hFile == NULL)
	{
		return 0;
	}

#ifdef _WIN32
	return (vlUInt)SetFilePointer(this->hFile, lOffset, NULL, uiMode);
#else
	if(fseek(this->hFile, lOffset, (int)uiMode) != 0)
	{
		LastError.Set("fseek() failed.", vlTrue);
	}

	return this->GetStreamPointer();
#endif
}

vlBool CFileReader::Read(vlChar &cChar)
{
	if(this->hFile == NULL)
	{
		return vlFalse;
	}

	vlULong ulBytesRead = 0;

#ifdef _WIN32
	if(!ReadFile(this->hFile, &cChar, 1, &ulBytesRead, NULL))
	{
		LastError.Set("ReadFile() failed.", vlTrue);
	}
#else
	ulBytesRead = fread(&cChar, 1, 1, this->hFile);

	if(ulBytesRead != 1 && ferror(this->hFile))
	{
		LastError.Set("fread() failed.", vlTrue);
	}
#endif

	return ulBytesRead == 1;
}

vlUInt CFileReader::Read(vlVoid *vData, vlUInt uiBytes)
{
	if(this->hFile == NULL)
	{
		return 0;
	}

	vlULong ulBytesRead = 0;

#ifdef _WIN32
	if(!ReadFile(this->hFile, vData, uiBytes, &ulBytesRead, NULL))
	{
		LastError.Set("ReadFile() failed.", vlTrue);
	}
#else
	ulBytesRead = fread(vData, 1, uiBytes, this->hFile);

	if(ulBytesRead != uiBytes && ferror(this->hFile))
	{
		LastError.Set("fread() failed.", vlTrue);
	}
#endif

	return (vlUInt)ulBytesRead;
}