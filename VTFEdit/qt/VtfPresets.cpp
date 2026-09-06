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

#include "VtfPresets.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

#include <cmath>

namespace VTFEdit
{
	namespace
	{
		bool sameFloat(vlSingle a, vlSingle b)
		{
			return std::fabs(a - b) < 0.0005f;
		}

		QString boolText(bool bValue)
		{
			return bValue ? QStringLiteral("True") : QStringLiteral("False");
		}

		bool toBool(const QString &sValue)
		{
			return sValue.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0
				|| sValue == QLatin1String("1");
		}

		QString escape(const QString &sValue)
		{
			QString sResult = sValue;
			sResult.replace(QLatin1Char('\\'), QLatin1String("\\\\"));
			sResult.replace(QLatin1Char('\n'), QLatin1String("\\n"));
			sResult.replace(QLatin1Char('\r'), QLatin1String("\\r"));
			return sResult;
		}

		QString unescape(const QString &sValue)
		{
			QString sResult;
			sResult.reserve(sValue.size());

			for(int i = 0; i < sValue.size(); i++)
			{
				if(sValue[i] == QLatin1Char('\\') && i + 1 < sValue.size())
				{
					const QChar Next = sValue[++i];
					if(Next == QLatin1Char('n'))
						sResult += QLatin1Char('\n');
					else if(Next == QLatin1Char('r'))
						sResult += QLatin1Char('\r');
					else
						sResult += Next;
				}
				else
				{
					sResult += sValue[i];
				}
			}

			return sResult;
		}

		VtfOptions diffuseBase()
		{
			VtfOptions Options;
			Options.NormalFormat = IMAGE_FORMAT_DXT1;
			Options.AlphaFormat = IMAGE_FORMAT_DXT5;
			Options.GenerateMipmaps = vlTrue;
			Options.sRGB = vlTrue;
			Options.NormalMap = vlFalse;
			return Options;
		}

		VtfOptions normalMapBase()
		{
			VtfOptions Options = diffuseBase();
			Options.sRGB = vlFalse;
			Options.NormalMap = vlTrue;
			return Options;
		}
	}

	VtfPresets::VtfPresets()
	{
		m_Presets = stockPresets();
	}

	const QList<VtfPreset> &VtfPresets::stockPresets()
	{
		static const QList<VtfPreset> Stock = []()
		{
			QList<VtfPreset> Presets;

			const auto add = [&Presets](const QString &sName, const QString &sDescription, const VtfOptions &Options)
			{
				VtfPreset Preset;
				Preset.sName = sName;
				Preset.sDescription = sDescription;
				Preset.bBuiltin = true;
				Preset.Options = Options;
				Presets.append(Preset);
			};

			{
				add(QStringLiteral("Compressed Diffuse Texture"),
					QStringLiteral("DXT1, or DXT5 when the image has an alpha channel. Mipmapped and sRGB."),
					diffuseBase());
			}

			{
				VtfOptions Options = diffuseBase();
				Options.NormalFormat = IMAGE_FORMAT_RGB888;
				Options.AlphaFormat = IMAGE_FORMAT_RGBA8888;
				add(QStringLiteral("Uncompressed Diffuse Texture"),
					QStringLiteral("RGB888, or RGBA8888 when the image has an alpha channel. Mipmapped and sRGB."),
					Options);
			}

			{
				add(QStringLiteral("Compressed Normal Map"),
					QStringLiteral("DXT1/DXT5 tangent space normal map. sRGB off, normals re-normalized."),
					normalMapBase());
			}

			{
				VtfOptions Options = diffuseBase();
				Options.Version = QStringLiteral("7.6");
				Options.AlphaFormat = IMAGE_FORMAT_BC7;
				add(QStringLiteral("Compressed Diffuse Texture (DX11)"),
					QStringLiteral("Version 7.6. DXT1, or BC7 when the image has an alpha channel."),
					Options);
			}

			{
				VtfOptions Options = normalMapBase();
				Options.Version = QStringLiteral("7.6");
				Options.NormalFormat = IMAGE_FORMAT_BC7;
				Options.AlphaFormat = IMAGE_FORMAT_BC7;
				add(QStringLiteral("Compressed Normal Map (DX11)"),
					QStringLiteral("Version 7.6. BC7 tangent space normal map, sRGB off."),
					Options);
			}

			{
				VtfOptions Options;
				Options.NormalFormat = IMAGE_FORMAT_BGR888;
				Options.AlphaFormat = IMAGE_FORMAT_BGRA8888;
				Options.GenerateMipmaps = vlFalse;
				Options.sRGB = vlFalse;
				Options.FlagClampS = vlTrue;
				Options.FlagClampT = vlTrue;
				Options.FlagNoLOD = vlTrue;
				add(QStringLiteral("UI Icon"),
					QStringLiteral("Uncompressed and unfiltered, no mipmaps, no LOD and clamped."),
					Options);
			}

			{
				VtfOptions Options = diffuseBase();
				Options.AlphaFormat = IMAGE_FORMAT_DXT5;
				Options.FlagClampS = vlTrue;
				Options.FlagClampT = vlTrue;
				add(QStringLiteral("Decal"),
					QStringLiteral("DXT5 with an alpha channel and clamped."),
					Options);
			}

			{
				VtfOptions Options;
				Options.NormalFormat = IMAGE_FORMAT_BGR888;
				Options.AlphaFormat = IMAGE_FORMAT_BGRA8888;
				Options.GenerateMipmaps = vlFalse;
				Options.sRGB = vlTrue;
				Options.FlagClampS = vlTrue;
				Options.FlagClampT = vlTrue;
				Options.FlagNoLOD = vlTrue;
				add(QStringLiteral("Skybox Face"),
					QStringLiteral("Uncompressed and clamped with no mipmaps, to avoid seams and banding in the sky."),
					Options);
			}

			{
				VtfOptions Options = diffuseBase();
				Options.TextureType = VtfTextureType::EnvironmentMap;
				Options.GenerateSphereMap = vlTrue;
				add(QStringLiteral("Environment Map (Cubemap)"),
					QStringLiteral("Cubemap built from six faces (plus spheremap on Version 7.4 or lower)"),
					Options);
			}

			{
				VtfOptions Options;
				Options.NormalFormat = IMAGE_FORMAT_RGBA16161616F;
				Options.AlphaFormat = IMAGE_FORMAT_RGBA16161616F;
				Options.TextureType = VtfTextureType::EnvironmentMap;
				Options.GenerateSphereMap = vlTrue;
				Options.sRGB = vlFalse;
				add(QStringLiteral("HDR Environment Map (Float)"),
					QStringLiteral("Floating point cubemap for HDR space"),
					Options);
			}

			{
				VtfOptions Options;
				Options.NormalFormat = IMAGE_FORMAT_I8;
				Options.AlphaFormat = IMAGE_FORMAT_RGBA8888;
				Options.sRGB = vlFalse;
				add(QStringLiteral("Mask / Data Texture"),
					QStringLiteral("Uncompressed non-color data, e.g. for exponent maps, masks and blend modulation."),
					Options);
			}

			{
				VtfOptions Options;
				Options.NormalFormat = IMAGE_FORMAT_I8;
				Options.AlphaFormat = IMAGE_FORMAT_RGBA8888;
				Options.GenerateMipmaps = vlFalse;
				Options.sRGB = vlFalse;
				Options.FlagClampS = vlTrue;
				Options.FlagClampT = vlTrue;
				Options.FlagNoLOD = vlTrue;
				Options.DistanceAlpha = vlTrue;
				add(QStringLiteral("Distance alpha Icon"),
					QStringLiteral("Alpha replaced with a SDF for $distancealpha, uncompressed with no mipmaps."),
					Options);
			}

			return Presets;
		}();

		return Stock;
	}

	const VtfPreset *VtfPresets::at(int iIndex) const
	{
		if(iIndex < 0 || iIndex >= m_Presets.size())
		{
			return nullptr;
		}

		return &m_Presets[iIndex];
	}

	int VtfPresets::indexOf(const QString &sName) const
	{
		for(int i = 0; i < m_Presets.size(); i++)
		{
			if(m_Presets[i].sName.compare(sName, Qt::CaseInsensitive) == 0)
			{
				return i;
			}
		}

		return -1;
	}

	int VtfPresets::match(const VtfOptions &Options) const
	{
		for(int i = 0; i < m_Presets.size(); i++)
		{
			if(equals(m_Presets[i].Options, Options))
			{
				return i;
			}
		}

		return -1;
	}

	int VtfPresets::save(const QString &sName, const VtfOptions &Options)
	{
		if(sName.trimmed().isEmpty())
		{
			return -1;
		}

		const int iExisting = indexOf(sName);

		if(iExisting != -1)
		{
			m_Presets[iExisting].Options = Options;
			return iExisting;
		}

		VtfPreset Preset;
		Preset.sName = sName.trimmed();
		Preset.bBuiltin = false;
		Preset.Options = Options;
		m_Presets.append(Preset);

		return m_Presets.size() - 1;
	}

	bool VtfPresets::remove(int iIndex)
	{
		if(iIndex < 0 || iIndex >= m_Presets.size())
		{
			return false;
		}

		if(m_Presets[iIndex].bBuiltin && !m_RemovedBuiltins.contains(m_Presets[iIndex].sName, Qt::CaseInsensitive))
		{
			m_RemovedBuiltins.append(m_Presets[iIndex].sName);
		}

		m_Presets.removeAt(iIndex);

		return true;
	}

	bool VtfPresets::isModified(int iIndex) const
	{
		const VtfPreset *pPreset = at(iIndex);

		if(pPreset == nullptr || !pPreset->bBuiltin)
		{
			return false;
		}

		for(const VtfPreset &Stock : stockPresets())
		{
			if(Stock.sName.compare(pPreset->sName, Qt::CaseInsensitive) == 0)
			{
				return !equals(Stock.Options, pPreset->Options);
			}
		}

		return false;
	}

	bool VtfPresets::hasModifiedBuiltins() const
	{
		if(!m_RemovedBuiltins.isEmpty())
		{
			return true;
		}

		for(int i = 0; i < m_Presets.size(); i++)
		{
			if(isModified(i))
			{
				return true;
			}
		}

		return false;
	}

	void VtfPresets::restoreDefaults()
	{
		m_RemovedBuiltins.clear();

		// keep the user's own presets, in the order they were made
		QList<VtfPreset> Restored = stockPresets();

		for(const VtfPreset &Preset : m_Presets)
		{
			if(!Preset.bBuiltin)
			{
				Restored.append(Preset);
			}
		}

		m_Presets = Restored;
	}

	void VtfPresets::apply(const VtfPreset &Preset, VtfOptions &Options)
	{
		const VtfOptions &From = Preset.Options;

		Options.NormalFormat = From.NormalFormat;
		Options.AlphaFormat = From.AlphaFormat;
		Options.TextureType = From.TextureType;

		Options.FlagClampS = From.FlagClampS;
		Options.FlagClampT = From.FlagClampT;
		Options.FlagNoLOD = From.FlagNoLOD;
		Options.FlagPointSample = From.FlagPointSample;

		Options.ResizeImage = From.ResizeImage;
		Options.ResizeMethod = From.ResizeMethod;
		Options.ResizeFilter = From.ResizeFilter;
		Options.ResizeClamp = From.ResizeClamp;
		Options.ResizeClampWidth = From.ResizeClampWidth;
		Options.ResizeClampHeight = From.ResizeClampHeight;

		Options.GenerateMipmaps = From.GenerateMipmaps;
		Options.MipmapFilter = From.MipmapFilter;

		Options.Version = From.Version;
		Options.AuxCompressionLevel = From.AuxCompressionLevel;
		Options.AuxCompressionMethod = From.AuxCompressionMethod;

		Options.ComputeReflectivity = From.ComputeReflectivity;
		Options.GenerateThumbnail = From.GenerateThumbnail;
		Options.GenerateSphereMap = From.GenerateSphereMap;
		Options.StripAlpha = From.StripAlpha;
		Options.sRGB = From.sRGB;
		Options.NormalMap = From.NormalMap;

		Options.DistanceAlpha = From.DistanceAlpha;
		Options.DistanceAlphaSpread = From.DistanceAlphaSpread;
		Options.DistanceAlphaReduce = From.DistanceAlphaReduce;
		Options.DistanceAlphaThreshold = From.DistanceAlphaThreshold;

		Options.CorrectGamma = From.CorrectGamma;
		Options.GammaCorrection = From.GammaCorrection;

		Options.LuminanceWeightR = From.LuminanceWeightR;
		Options.LuminanceWeightG = From.LuminanceWeightG;
		Options.LuminanceWeightB = From.LuminanceWeightB;

		Options.CreateLODControlResource = From.CreateLODControlResource;
		Options.LODControlClampU = From.LODControlClampU;
		Options.LODControlClampV = From.LODControlClampV;
	}

	bool VtfPresets::equals(const VtfOptions &A, const VtfOptions &B)
	{
		return A.NormalFormat == B.NormalFormat
			&& A.AlphaFormat == B.AlphaFormat
			&& A.TextureType == B.TextureType

			&& (A.FlagClampS != vlFalse) == (B.FlagClampS != vlFalse)
			&& (A.FlagClampT != vlFalse) == (B.FlagClampT != vlFalse)
			&& (A.FlagNoLOD != vlFalse) == (B.FlagNoLOD != vlFalse)
			&& (A.FlagPointSample != vlFalse) == (B.FlagPointSample != vlFalse)

			&& (A.ResizeImage != vlFalse) == (B.ResizeImage != vlFalse)
			&& A.ResizeMethod == B.ResizeMethod
			&& A.ResizeFilter == B.ResizeFilter
			&& (A.ResizeClamp != vlFalse) == (B.ResizeClamp != vlFalse)
			&& A.ResizeClampWidth == B.ResizeClampWidth
			&& A.ResizeClampHeight == B.ResizeClampHeight

			&& (A.GenerateMipmaps != vlFalse) == (B.GenerateMipmaps != vlFalse)
			&& A.MipmapFilter == B.MipmapFilter

			&& A.Version == B.Version
			&& A.AuxCompressionLevel == B.AuxCompressionLevel
			&& A.AuxCompressionMethod == B.AuxCompressionMethod

			&& (A.ComputeReflectivity != vlFalse) == (B.ComputeReflectivity != vlFalse)
			&& (A.GenerateThumbnail != vlFalse) == (B.GenerateThumbnail != vlFalse)
			&& (A.GenerateSphereMap != vlFalse) == (B.GenerateSphereMap != vlFalse)
			&& (A.StripAlpha != vlFalse) == (B.StripAlpha != vlFalse)
			&& (A.sRGB != vlFalse) == (B.sRGB != vlFalse)
			&& (A.NormalMap != vlFalse) == (B.NormalMap != vlFalse)

			&& (A.DistanceAlpha != vlFalse) == (B.DistanceAlpha != vlFalse)
			&& sameFloat(A.DistanceAlphaSpread, B.DistanceAlphaSpread)
			&& A.DistanceAlphaReduce == B.DistanceAlphaReduce
			&& A.DistanceAlphaThreshold == B.DistanceAlphaThreshold

			&& (A.CorrectGamma != vlFalse) == (B.CorrectGamma != vlFalse)
			&& sameFloat(A.GammaCorrection, B.GammaCorrection)

			&& sameFloat(A.LuminanceWeightR, B.LuminanceWeightR)
			&& sameFloat(A.LuminanceWeightG, B.LuminanceWeightG)
			&& sameFloat(A.LuminanceWeightB, B.LuminanceWeightB)

			&& (A.CreateLODControlResource != vlFalse) == (B.CreateLODControlResource != vlFalse)
			&& A.LODControlClampU == B.LODControlClampU
			&& A.LODControlClampV == B.LODControlClampV;
	}

	QString VtfPresets::filePath()
	{
		const QString sDirectory = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

		QDir().mkpath(sDirectory);

		return QDir(sDirectory).filePath(QApplication::applicationName() + QStringLiteral("-Presets.ini"));
	}

	bool VtfPresets::read(const QString &sFileName)
	{
		QFile File(sFileName);
		if(!File.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			return false;
		}

		QTextStream Stream(&File);

		QList<VtfPreset> Parsed;
		QStringList Removed;

		VtfPreset Preset;
		bool bHavePreset = false;

		const auto flush = [&]()
		{
			if(bHavePreset && !Preset.sName.isEmpty())
			{
				Parsed.append(Preset);
			}

			Preset = VtfPreset();
			bHavePreset = false;
		};

		while(!Stream.atEnd())
		{
			const QString sLine = Stream.readLine();
			const int iIndex = sLine.indexOf(QLatin1Char('='));

			if(iIndex == -1)
			{
				continue;
			}

			const QString sArg = sLine.left(iIndex).trimmed();
			const QString sVal = sLine.mid(iIndex + 1).trimmed();

			if(sArg.compare(QLatin1String("Presets.RemovedBuiltin"), Qt::CaseInsensitive) == 0)
			{
				const QString sRemoved = unescape(sVal);
				if(!sRemoved.isEmpty() && !Removed.contains(sRemoved, Qt::CaseInsensitive))
				{
					Removed.append(sRemoved);
				}
				continue;
			}

			if(sArg.compare(QLatin1String("Preset.Name"), Qt::CaseInsensitive) == 0)
			{
				flush();
				Preset.sName = unescape(sVal);
				Preset.bBuiltin = false;
				bHavePreset = true;
				continue;
			}

			if(!bHavePreset)
			{
				continue;
			}

			if(sArg.compare(QLatin1String("Preset.Builtin"), Qt::CaseInsensitive) == 0)
			{
				Preset.bBuiltin = toBool(sVal);
				continue;
			}

			VtfOptions &Options = Preset.Options;

			if(sArg.compare(QLatin1String("Preset.NormalFormat"), Qt::CaseInsensitive) == 0)
				Options.NormalFormat = static_cast<VTFImageFormat>(sVal.toInt());
			else if(sArg.compare(QLatin1String("Preset.AlphaFormat"), Qt::CaseInsensitive) == 0)
				Options.AlphaFormat = static_cast<VTFImageFormat>(sVal.toInt());
			else if(sArg.compare(QLatin1String("Preset.TextureType"), Qt::CaseInsensitive) == 0)
				Options.TextureType = static_cast<VtfTextureType>(sVal.toInt());

			else if(sArg.compare(QLatin1String("Preset.FlagClampS"), Qt::CaseInsensitive) == 0)
				Options.FlagClampS = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.FlagClampT"), Qt::CaseInsensitive) == 0)
				Options.FlagClampT = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.FlagNoLOD"), Qt::CaseInsensitive) == 0)
				Options.FlagNoLOD = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.FlagPointSample"), Qt::CaseInsensitive) == 0)
				Options.FlagPointSample = toBool(sVal);

			else if(sArg.compare(QLatin1String("Preset.Resize"), Qt::CaseInsensitive) == 0)
				Options.ResizeImage = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.ResizeMethod"), Qt::CaseInsensitive) == 0)
				Options.ResizeMethod = static_cast<VTFResizeMethod>(sVal.toInt());
			else if(sArg.compare(QLatin1String("Preset.ResizeFilter"), Qt::CaseInsensitive) == 0)
				Options.ResizeFilter = static_cast<VTFMipmapFilter>(sVal.toInt());
			else if(sArg.compare(QLatin1String("Preset.ResizeClamp"), Qt::CaseInsensitive) == 0)
				Options.ResizeClamp = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.ResizeClampWidth"), Qt::CaseInsensitive) == 0)
				Options.ResizeClampWidth = sVal.toUInt();
			else if(sArg.compare(QLatin1String("Preset.ResizeClampHeight"), Qt::CaseInsensitive) == 0)
				Options.ResizeClampHeight = sVal.toUInt();

			else if(sArg.compare(QLatin1String("Preset.GenerateMipmaps"), Qt::CaseInsensitive) == 0)
				Options.GenerateMipmaps = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.MipmapFilter"), Qt::CaseInsensitive) == 0)
				Options.MipmapFilter = static_cast<VTFMipmapFilter>(sVal.toInt());

			else if(sArg.compare(QLatin1String("Preset.Version"), Qt::CaseInsensitive) == 0)
				Options.Version = sVal;
			else if(sArg.compare(QLatin1String("Preset.AuxCompressionLevel"), Qt::CaseInsensitive) == 0)
				Options.AuxCompressionLevel = static_cast<vlShort>(sVal.toInt());
			else if(sArg.compare(QLatin1String("Preset.AuxCompressionMethod"), Qt::CaseInsensitive) == 0)
				Options.AuxCompressionMethod = static_cast<vlShort>(sVal.toInt());

			else if(sArg.compare(QLatin1String("Preset.ComputeReflectivity"), Qt::CaseInsensitive) == 0)
				Options.ComputeReflectivity = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.GenerateThumbnail"), Qt::CaseInsensitive) == 0)
				Options.GenerateThumbnail = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.GenerateSphereMap"), Qt::CaseInsensitive) == 0)
				Options.GenerateSphereMap = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.StripAlpha"), Qt::CaseInsensitive) == 0)
				Options.StripAlpha = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.sRGB"), Qt::CaseInsensitive) == 0)
				Options.sRGB = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.NormalMap"), Qt::CaseInsensitive) == 0)
				Options.NormalMap = toBool(sVal);

			else if(sArg.compare(QLatin1String("Preset.DistanceAlpha"), Qt::CaseInsensitive) == 0)
				Options.DistanceAlpha = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.DistanceAlphaSpread"), Qt::CaseInsensitive) == 0)
				Options.DistanceAlphaSpread = sVal.toFloat();
			else if(sArg.compare(QLatin1String("Preset.DistanceAlphaReduce"), Qt::CaseInsensitive) == 0)
				Options.DistanceAlphaReduce = sVal.toUInt();
			else if(sArg.compare(QLatin1String("Preset.DistanceAlphaThreshold"), Qt::CaseInsensitive) == 0)
				Options.DistanceAlphaThreshold = sVal.toUInt();

			else if(sArg.compare(QLatin1String("Preset.CorrectGamma"), Qt::CaseInsensitive) == 0)
				Options.CorrectGamma = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.GammaCorrection"), Qt::CaseInsensitive) == 0)
				Options.GammaCorrection = sVal.toFloat();

			else if(sArg.compare(QLatin1String("Preset.LuminanceWeightR"), Qt::CaseInsensitive) == 0)
				Options.LuminanceWeightR = sVal.toFloat();
			else if(sArg.compare(QLatin1String("Preset.LuminanceWeightG"), Qt::CaseInsensitive) == 0)
				Options.LuminanceWeightG = sVal.toFloat();
			else if(sArg.compare(QLatin1String("Preset.LuminanceWeightB"), Qt::CaseInsensitive) == 0)
				Options.LuminanceWeightB = sVal.toFloat();

			else if(sArg.compare(QLatin1String("Preset.CreateLODControlResource"), Qt::CaseInsensitive) == 0)
				Options.CreateLODControlResource = toBool(sVal);
			else if(sArg.compare(QLatin1String("Preset.LODControlClampU"), Qt::CaseInsensitive) == 0)
				Options.LODControlClampU = sVal.toUInt();
			else if(sArg.compare(QLatin1String("Preset.LODControlClampV"), Qt::CaseInsensitive) == 0)
				Options.LODControlClampV = sVal.toUInt();
		}

		flush();

		File.close();

		// start from how the presets ship.. then lay the saved changes over the top
		m_Presets = stockPresets();
		m_RemovedBuiltins.clear();

		for(const QString &sRemoved : Removed)
		{
			const int iIndex = indexOf(sRemoved);
			if(iIndex != -1 && m_Presets[iIndex].bBuiltin)
			{
				m_Presets.removeAt(iIndex);
			}

			m_RemovedBuiltins.append(sRemoved);
		}

		for(const VtfPreset &Saved : Parsed)
		{
			const int iIndex = indexOf(Saved.sName);

			if(Saved.bBuiltin)
			{
				// an edited built-in preset
				if(iIndex != -1 && m_Presets[iIndex].bBuiltin)
				{
					m_Presets[iIndex].Options = Saved.Options;
				}
			}
			else if(iIndex == -1)
			{
				m_Presets.append(Saved);
			}
		}

		return true;
	}

	bool VtfPresets::write(const QString &sFileName) const
	{
		QFile File(sFileName);
		if(!File.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
		{
			return false;
		}

		QTextStream Stream(&File);

		Stream << "[VTFEdit Presets]\n";

		for(const QString &sRemoved : m_RemovedBuiltins)
		{
			Stream << "Presets.RemovedBuiltin = " << escape(sRemoved) << "\n";
		}

		for(int iPreset = 0; iPreset < m_Presets.size(); iPreset++)
		{
			const VtfPreset &Preset = m_Presets[iPreset];

			// built in presets are only written out once they've been edited
			if(Preset.bBuiltin && !isModified(iPreset))
			{
				continue;
			}

			const VtfOptions &Options = Preset.Options;

			Stream << "\nPreset.Name = " << escape(Preset.sName) << "\n";
			Stream << "Preset.Builtin = " << boolText(Preset.bBuiltin) << "\n";

			Stream << "Preset.NormalFormat = " << static_cast<int>(Options.NormalFormat) << "\n";
			Stream << "Preset.AlphaFormat = " << static_cast<int>(Options.AlphaFormat) << "\n";
			Stream << "Preset.TextureType = " << static_cast<int>(Options.TextureType) << "\n";

			Stream << "Preset.FlagClampS = " << boolText(Options.FlagClampS != vlFalse) << "\n";
			Stream << "Preset.FlagClampT = " << boolText(Options.FlagClampT != vlFalse) << "\n";
			Stream << "Preset.FlagNoLOD = " << boolText(Options.FlagNoLOD != vlFalse) << "\n";
			Stream << "Preset.FlagPointSample = " << boolText(Options.FlagPointSample != vlFalse) << "\n";

			Stream << "Preset.Resize = " << boolText(Options.ResizeImage != vlFalse) << "\n";
			Stream << "Preset.ResizeMethod = " << static_cast<int>(Options.ResizeMethod) << "\n";
			Stream << "Preset.ResizeFilter = " << static_cast<int>(Options.ResizeFilter) << "\n";
			Stream << "Preset.ResizeClamp = " << boolText(Options.ResizeClamp != vlFalse) << "\n";
			Stream << "Preset.ResizeClampWidth = " << Options.ResizeClampWidth << "\n";
			Stream << "Preset.ResizeClampHeight = " << Options.ResizeClampHeight << "\n";

			Stream << "Preset.GenerateMipmaps = " << boolText(Options.GenerateMipmaps != vlFalse) << "\n";
			Stream << "Preset.MipmapFilter = " << static_cast<int>(Options.MipmapFilter) << "\n";

			Stream << "Preset.Version = " << Options.Version << "\n";
			Stream << "Preset.AuxCompressionLevel = " << Options.AuxCompressionLevel << "\n";
			Stream << "Preset.AuxCompressionMethod = " << Options.AuxCompressionMethod << "\n";

			Stream << "Preset.ComputeReflectivity = " << boolText(Options.ComputeReflectivity != vlFalse) << "\n";
			Stream << "Preset.GenerateThumbnail = " << boolText(Options.GenerateThumbnail != vlFalse) << "\n";
			Stream << "Preset.GenerateSphereMap = " << boolText(Options.GenerateSphereMap != vlFalse) << "\n";
			Stream << "Preset.StripAlpha = " << boolText(Options.StripAlpha != vlFalse) << "\n";
			Stream << "Preset.sRGB = " << boolText(Options.sRGB != vlFalse) << "\n";
			Stream << "Preset.NormalMap = " << boolText(Options.NormalMap != vlFalse) << "\n";

			Stream << "Preset.DistanceAlpha = " << boolText(Options.DistanceAlpha != vlFalse) << "\n";
			Stream << "Preset.DistanceAlphaSpread = " << Options.DistanceAlphaSpread << "\n";
			Stream << "Preset.DistanceAlphaReduce = " << Options.DistanceAlphaReduce << "\n";
			Stream << "Preset.DistanceAlphaThreshold = " << Options.DistanceAlphaThreshold << "\n";

			Stream << "Preset.CorrectGamma = " << boolText(Options.CorrectGamma != vlFalse) << "\n";
			Stream << "Preset.GammaCorrection = " << Options.GammaCorrection << "\n";

			Stream << "Preset.LuminanceWeightR = " << Options.LuminanceWeightR << "\n";
			Stream << "Preset.LuminanceWeightG = " << Options.LuminanceWeightG << "\n";
			Stream << "Preset.LuminanceWeightB = " << Options.LuminanceWeightB << "\n";

			Stream << "Preset.CreateLODControlResource = " << boolText(Options.CreateLODControlResource != vlFalse) << "\n";
			Stream << "Preset.LODControlClampU = " << Options.LODControlClampU << "\n";
			Stream << "Preset.LODControlClampV = " << Options.LODControlClampV << "\n";
		}

		File.close();

		return true;
	}
}
