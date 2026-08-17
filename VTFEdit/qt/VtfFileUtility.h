/*
 * VTFEdit
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

#pragma once

#include "VtfOptions.h"

#include <vector>

namespace VTFEdit
{
	namespace VtfFileUtility
	{
		// Returns true if the currently bound image stores floating point channels
		bool IsFloatImage();
		bool IsFloatImageFileName(const QString &sFileName);

		// Returns true if any pixel of an image is not fully opaque
		bool HasAlphaData(const vlByte *lpImageData, vlUInt uiWidth, vlUInt uiHeight);
		bool HasAlphaDataRGBA32F(const vlSingle *lpImageData, vlUInt uiWidth, vlUInt uiHeight);

		// Replaces the alpha channel of every RGBA8888 frame with a signed distance field
		bool ApplyDistanceAlpha(std::vector<vlByte *> &vImageData, vlUInt &uiWidth, vlUInt &uiHeight, const VtfOptions &Options);

		SVTFCreateOptions GetCreateOptions(const VtfOptions &Options);

		void ApplyFlags(const VtfOptions &Options, VTFLib::CVTFFile *pVTFFile);

		bool CreateResources(const VtfOptions &Options, VTFLib::CVTFFile *pVTFFile);
	}
}
