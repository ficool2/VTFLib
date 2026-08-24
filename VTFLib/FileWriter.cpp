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
#include "FileWriter.h"

using namespace VTFLib;
using namespace VTFLib::IO::Writers;

CFileWriter::CFileWriter(const vlChar *cFileName)
{
	this->hFile = NULL;

	this->cFileName = new vlChar[strlen(cFileName) + 1];
	strcpy(this->cFileName, cFileName);
}

CFileWriter::~CFileWriter()
{
	this->Close();

	delete []this->cFileName;
}

vlBool CFileWriter::Opened() const
{
	return this->hFile != NULL;
}

vlBool CFileWriter::Open()
{
	this->Close();

#ifdef _WIN32
	this->hFile = CreateFile(this->cFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(this->hFile == INVALID_HANDLE_VALUE)
	{
		this->hFile = NULL;

		LastError.Set("Error opening file.", vlTrue);

		return vlFalse;
	}
#else
	this->hFile = fopen(this->cFileName, "wb");

	if(this->hFile == NULL)
	{
		LastError.Set("Error opening file.", vlTrue);

		return vlFalse;
	}
#endif

	return vlTrue;
}

vlVoid CFileWriter::Close()
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

vlUInt CFileWriter::GetStreamSize() const
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

vlUInt CFileWriter::GetStreamPointer() const
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

vlUInt CFileWriter::Seek(vlLong lOffset, vlUInt uiMode)
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

vlBool CFileWriter::Write(vlChar cChar)
{
	if(this->hFile == NULL)
	{
		return vlFalse;
	}

	vlULong ulBytesWritten = 0;

#ifdef _WIN32
	if(!WriteFile(this->hFile, &cChar, 1, &ulBytesWritten, NULL))
	{
		LastError.Set("WriteFile() failed.", vlTrue);
	}
#else
	ulBytesWritten = fwrite(&cChar, 1, 1, this->hFile);

	if(ulBytesWritten != 1)
	{
		LastError.Set("fwrite() failed.", vlTrue);
	}
#endif

	return ulBytesWritten == 1;
}

vlUInt CFileWriter::Write(vlVoid *vData, vlUInt uiBytes)
{
	if(this->hFile == NULL)
	{
		return 0;
	}

	vlULong ulBytesWritten = 0;

#ifdef _WIN32
	if(!WriteFile(this->hFile, vData, uiBytes, &ulBytesWritten, NULL))
	{
		LastError.Set("WriteFile() failed.", vlTrue);
	}
#else
	ulBytesWritten = fwrite(vData, 1, uiBytes, this->hFile);

	if(ulBytesWritten != uiBytes)
	{
		LastError.Set("fwrite() failed.", vlTrue);
	}
#endif

	return (vlUInt)ulBytesWritten;
}