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

#include "SingleInstance.h"

#include "MainWindow.h"

#include <QLocalServer>
#include <QLocalSocket>
#include <QThread>

namespace VTFEdit
{
	namespace
	{
		QString singleInstanceServerName()
		{
			return QStringLiteral("VTFEditPlusPlus");
		}

		QStringList receiveFilePaths(QLocalSocket *pSocket)
		{
			QByteArray Payload;
			while(pSocket->waitForReadyRead(500))
			{
				Payload.append(pSocket->readAll());
			}
			Payload.append(pSocket->readAll());

			return QString::fromUtf8(Payload).split(QLatin1Char('\n'), Qt::SkipEmptyParts);
		}

		bool sendToRunningInstance(const QStringList &sFilePaths)
		{
			QLocalSocket Socket;
			Socket.connectToServer(singleInstanceServerName());

			if(!Socket.waitForConnected(500))
			{
				return false;
			}

			Socket.write(sFilePaths.join(QLatin1Char('\n')).toUtf8());
			Socket.flush();
			Socket.waitForBytesWritten(1000);

			Socket.disconnectFromServer();
			if(Socket.state() != QLocalSocket::UnconnectedState)
			{
				Socket.waitForDisconnected(1000);
			}

			return true;
		}

		void startOrStopServer(QLocalServer &Server, bool bEnabled)
		{
			if(!bEnabled)
			{
				Server.close();
			}
			else if(!Server.isListening())
			{
				Server.listen(singleInstanceServerName());
			}
		}
	}

	bool handOffOrBecomePrimary(QLocalServer &Server, const QStringList &sFilePaths)
	{
		Server.setSocketOptions(QLocalServer::UserAccessOption);

		// TODO there has to be a better way of doing this
		for(int iAttempt = 0; iAttempt < 10; iAttempt++)
		{
			if(sendToRunningInstance(sFilePaths))
			{
				return true;
			}

			if(Server.listen(singleInstanceServerName()))
			{
				return false;
			}

			QThread::msleep(50);
		}

		QLocalServer::removeServer(singleInstanceServerName());
		Server.listen(singleInstanceServerName());

		return false;
	}

	void setupSingleInstanceServer(QLocalServer &Server, MainWindow &Window, bool bEnabled)
	{
		Server.setSocketOptions(QLocalServer::UserAccessOption);

		const auto takePendingFiles = [&Server, &Window]()
		{
			while(QLocalSocket *pSocket = Server.nextPendingConnection())
			{
				const QStringList sFilePaths = receiveFilePaths(pSocket);
				pSocket->deleteLater();

				Window.activateWithFiles(sFilePaths);
			}
		};

		QObject::connect(&Server, &QLocalServer::newConnection, &Window, takePendingFiles);

		takePendingFiles();

		QObject::connect(&Window, &MainWindow::singleInstanceChanged, &Window,
			[&Server](bool bSingleInstance)
		{
			startOrStopServer(Server, bSingleInstance);
		});

		startOrStopServer(Server, bEnabled);
	}
}
