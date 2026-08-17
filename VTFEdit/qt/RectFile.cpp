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

#include "RectFile.h"

#include "VTFLibQt.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStringList>

namespace VTFEdit
{
	namespace
	{
		QString nodeValue(const VTFLib::Nodes::CVMTNode *pNode)
		{
			switch(pNode->GetType())
			{
			case NODE_TYPE_STRING:
				return QString::fromUtf8(
					static_cast<const VTFLib::Nodes::CVMTStringNode *>(pNode)->GetValue());
			case NODE_TYPE_INTEGER:
				return QString::number(
					static_cast<const VTFLib::Nodes::CVMTIntegerNode *>(pNode)->GetValue());
			case NODE_TYPE_SINGLE:
				return QString::number(
					static_cast<const VTFLib::Nodes::CVMTSingleNode *>(pNode)->GetValue());
			default:
				return QString();
			}
		}

		bool nameIs(const VTFLib::Nodes::CVMTNode *pNode, const char *szName)
		{
			return QString::fromUtf8(pNode->GetName()).compare(
				QLatin1String(szName), Qt::CaseInsensitive) == 0;
		}

		bool parsePoint(const QString &sValue, int &iX, int &iY)
		{
			const QStringList Parts = sValue.simplified().split(QLatin1Char(' '), Qt::SkipEmptyParts);
			if(Parts.count() != 2)
			{
				return false;
			}

			bool bOkX = false;
			bool bOkY = false;

			const double fX = Parts.at(0).toDouble(&bOkX);
			const double fY = Parts.at(1).toDouble(&bOkY);

			if(!bOkX || !bOkY)
			{
				return false;
			}

			iX = qRound(fX);
			iY = qRound(fY);
			return true;
		}

		bool parseFlag(const QString &sValue)
		{
			return sValue.trimmed().toInt() != 0;
		}

		QString escape(const QString &sValue)
		{
			QString sResult = sValue;
			sResult.replace(QLatin1Char('"'), QLatin1Char('\''));
			return sResult;
		}
	}

	bool RectFile::load(const QByteArray &Data, QString *psError)
	{
		m_Rectangles.clear();

		VTFLib::CVMTFile File;
		if(!File.Load(Data.constData(), static_cast<vlUInt>(Data.length())))
		{
			if(psError != nullptr)
			{
				const vlUInt uiLine = File.GetParseErrorLine();
				const QString sMessage = QString::fromUtf8(vlGetLastError());

				*psError = uiLine != 0
					? QCoreApplication::translate("RectFile", "Line %1: %2")
						.arg(uiLine).arg(sMessage)
					: sMessage;
			}
			return false;
		}

		const VTFLib::Nodes::CVMTGroupNode *pRectangles = File.GetRoot();

		for(vlUInt i = 0; i < pRectangles->GetNodeCount(); i++)
		{
			const VTFLib::Nodes::CVMTNode *pNode = pRectangles->GetNode(i);

			if(pNode->GetType() != NODE_TYPE_GROUP || !nameIs(pNode, "rectangle"))
			{
				continue;
			}

			const VTFLib::Nodes::CVMTGroupNode *pRectangle =
				static_cast<const VTFLib::Nodes::CVMTGroupNode *>(pNode);

			RectEntry Entry;
			int iMinX = 0, iMinY = 0, iMaxX = 0, iMaxY = 0;
			bool bHasMin = false;
			bool bHasMax = false;

			for(vlUInt j = 0; j < pRectangle->GetNodeCount(); j++)
			{
				const VTFLib::Nodes::CVMTNode *pProperty = pRectangle->GetNode(j);

				if(pProperty->GetType() == NODE_TYPE_GROUP)
				{
					continue;
				}

				const QString sValue = nodeValue(pProperty);

				if(nameIs(pProperty, "min"))
				{
					bHasMin = parsePoint(sValue, iMinX, iMinY);
				}
				else if(nameIs(pProperty, "max"))
				{
					bHasMax = parsePoint(sValue, iMaxX, iMaxY);
				}
				else if(nameIs(pProperty, "rotate"))
				{
					Entry.bRotate = parseFlag(sValue);
				}
				else if(nameIs(pProperty, "reflect"))
				{
					Entry.bReflect = parseFlag(sValue);
				}
				else if(nameIs(pProperty, "alt"))
				{
					Entry.bAlt = parseFlag(sValue);
				}
				else if(nameIs(pProperty, "tile"))
				{
					Entry.bTile = parseFlag(sValue);
				}
			}

			if(!bHasMin || !bHasMax)
			{
				if(psError != nullptr)
				{
					*psError = QCoreApplication::translate("RectFile",
						"Rectangle %1 is missing a valid min or max.")
						.arg(m_Rectangles.count() + 1);
				}
				return false;
			}

			Entry.Rectangle = QRect(QPoint(qMin(iMinX, iMaxX), qMin(iMinY, iMaxY)),
				QPoint(qMax(iMinX, iMaxX) - 1, qMax(iMinY, iMaxY) - 1));

			m_Rectangles.append(Entry);
		}

		return true;
	}

	QByteArray RectFile::save() const
	{
		VTFLib::CVMTFile File;
		File.Create("Rectangles");

		for(const RectEntry &Entry : m_Rectangles)
		{
			const QRect Rectangle = Entry.Rectangle.normalized();

			VTFLib::Nodes::CVMTGroupNode *pRectangle =
				File.GetRoot()->AddGroupNode("rectangle");

			const QByteArray Min = QString::fromLatin1("%1 %2")
				.arg(Rectangle.left()).arg(Rectangle.top()).toUtf8();
			const QByteArray Max = QString::fromLatin1("%1 %2")
				.arg(Rectangle.right() + 1).arg(Rectangle.bottom() + 1).toUtf8();

			pRectangle->AddStringNode("min", Min.constData());
			pRectangle->AddStringNode("max", Max.constData());

			if(Entry.bRotate)
			{
				pRectangle->AddIntegerNode("rotate", 1);
			}
			if(Entry.bReflect)
			{
				pRectangle->AddIntegerNode("reflect", 1);
			}
			if(Entry.bAlt)
			{
				pRectangle->AddIntegerNode("alt", 1);
			}
			if(Entry.bTile)
			{
				pRectangle->AddIntegerNode("tile", 1);
			}
		}

		QByteArray Buffer(65536, '\0');
		vlUInt uiSize = 0;
		File.Save(Buffer.data(), static_cast<vlUInt>(Buffer.size()), uiSize);
		Buffer.truncate(static_cast<int>(uiSize));

		return Buffer;
	}

	TileAxis RectFile::tileAxis(const QRect &Rectangle, int iImageWidth, int iImageHeight)
	{
		const QRect Normalized = Rectangle.normalized();

		const bool bX = Normalized.left() <= 0 && Normalized.right() >= iImageWidth - 1;
		const bool bY = Normalized.top() <= 0 && Normalized.bottom() >= iImageHeight - 1;

		if(bX && bY)
		{
			return TileAxisBoth;
		}

		return bX ? TileAxisX : (bY ? TileAxisY : TileAxisNone);
	}

	QString RectFile::pathForTexture(const QString &sFileName)
	{
		if(sFileName.isEmpty())
		{
			return QString();
		}

		const QFileInfo Info(sFileName);
		return Info.dir().filePath(Info.completeBaseName() + QLatin1String(".rect"));
	}
}
