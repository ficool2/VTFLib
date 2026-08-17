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

#include "VtfOptions.h"

namespace VTFEdit
{
	// ordered by how commonly the format is used

	const ImageFormatEntry NormalImageFormats[NormalImageFormatCount] =
	{
		{ IMAGE_FORMAT_DXT1,				"DXT1",					false },
		{ IMAGE_FORMAT_RGB888,				"RGB888",				false },
		{ IMAGE_FORMAT_BGR888,				"BGR888",				false },
		{ IMAGE_FORMAT_I8,					"I8",					false },
		{ IMAGE_FORMAT_ATI1N,				"ATI1N",				false },
		{ IMAGE_FORMAT_ATI2N,				"ATI2N",				false },
		{ IMAGE_FORMAT_BC4,					"BC4",					true },
		{ IMAGE_FORMAT_BC5,					"BC5",					true },
		{ IMAGE_FORMAT_BC6H,				"BC6H",					true },
		{ IMAGE_FORMAT_BC7,					"BC7",					true },
		{ IMAGE_FORMAT_R8,					"R8",					true },
		{ IMAGE_FORMAT_R32F,				"R32F",					false },
		{ IMAGE_FORMAT_RGB323232F,			"RGB323232F",			false },
		{ IMAGE_FORMAT_RGBA16161616F,		"RGBA16161616F",		false },
		{ IMAGE_FORMAT_RGBA32323232F,		"RGBA32323232F",		false },
		{ IMAGE_FORMAT_BGRX8888,			"BGRX8888",				false },
		{ IMAGE_FORMAT_RGB565,				"RGB565",				false },
		{ IMAGE_FORMAT_BGR565,				"BGR565",				false },
		{ IMAGE_FORMAT_BGRX5551,			"BGRX5551",				false },
		{ IMAGE_FORMAT_UV88,				"UV88",					false },
		{ IMAGE_FORMAT_UVLX8888,			"UVLX8888",				false },
		{ IMAGE_FORMAT_RGB888_BLUESCREEN,	"RGB888 Bluescreen",	false },
		{ IMAGE_FORMAT_BGR888_BLUESCREEN,	"BGR888 Bluescreen",	false },
	};

	const ImageFormatEntry AlphaImageFormats[AlphaImageFormatCount] =
	{
		{ IMAGE_FORMAT_DXT5,				"DXT5",						false },
		{ IMAGE_FORMAT_RGBA8888,			"RGBA8888",					false },
		{ IMAGE_FORMAT_DXT3,				"DXT3",						false },
		{ IMAGE_FORMAT_DXT1_ONEBITALPHA,	"DXT1 With One Bit Alpha",	false },
		{ IMAGE_FORMAT_RGBA16161616,		"RGBA16161616",				false },
		{ IMAGE_FORMAT_RGBA16161616F,		"RGBA16161616F",			false },
		{ IMAGE_FORMAT_RGBA32323232F,		"RGBA32323232F",			false },
		{ IMAGE_FORMAT_A8,					"A8",						false },
		{ IMAGE_FORMAT_IA88,				"IA88",						false },
		{ IMAGE_FORMAT_BC7,					"BC7",						true },
		{ IMAGE_FORMAT_BGRA8888,			"BGRA8888",					false },
		{ IMAGE_FORMAT_ABGR8888,			"ABGR8888",					false },
		{ IMAGE_FORMAT_ARGB8888,			"ARGB8888",					false },
		{ IMAGE_FORMAT_BGRA4444,			"BGRA4444",					false },
		{ IMAGE_FORMAT_BGRA5551,			"BGRA5551",					false },
		{ IMAGE_FORMAT_UVWQ8888,			"UVWQ8888",					false },
	};

	void VtfOptions::reset()
	{
		NormalFormat = IMAGE_FORMAT_DXT1;
		AlphaFormat = IMAGE_FORMAT_DXT5;
		TextureType = VtfTextureType::Animated;

		FlagClampS = vlFalse;
		FlagClampT = vlFalse;
		FlagNoLOD = vlFalse;
		FlagPointSample = vlFalse;

		ResizeImage = vlTrue;
		ResizeMethod = RESIZE_NEAREST_POWER2;
		ResizeFilter = MIPMAP_FILTER_NICE;
		ResizeClamp = vlTrue;
		ResizeClampWidth = 4096;
		ResizeClampHeight = 4096;

		GenerateMipmaps = vlTrue;
		MipmapFilter = MIPMAP_FILTER_NICE;

		Version = QStringLiteral("7.4");
		AuxCompressionLevel = VTF_AUX_COMPRESSION_LEVEL_NONE;
		AuxCompressionMethod = AUX_COMPRESSION_METHOD_DEFLATE;

		ComputeReflectivity = vlTrue;
		GenerateThumbnail = vlTrue;
		GenerateSphereMap = vlTrue;
		StripAlpha = vlFalse;
		sRGB = vlTrue;

		DistanceAlpha = vlFalse;
		DistanceAlphaSpread = 1.0f;
		DistanceAlphaReduce = 1;
		DistanceAlphaThreshold = 10;

		CorrectGamma = vlFalse;
		GammaCorrection = 2.2f;

		LuminanceWeightR = 0.299f;
		LuminanceWeightG = 0.587f;
		LuminanceWeightB = 0.114f;

		CreateLODControlResource = vlFalse;
		LODControlClampU = 31;
		LODControlClampV = 31;

		CreateInformationResource = vlFalse;
		InformationAuthor.clear();
		InformationContact.clear();
		InformationVersion.clear();
		InformationModification.clear();
		InformationDescription.clear();
		InformationComments.clear();
	}
}
