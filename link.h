/**************************************************************************************************
---------------------------------------------------------------------------------------------------
	Copyright (C) 2015  Jonathan Bagg
	This file is part of Vactracker.

	Vactracker is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	Vactracker is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Vactracker; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
---------------------------------------------------------------------------------------------------
   Project name : Vacation Tracker Client
   File name    : link.h
   Created      : 08 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Handles communication to the server
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef LINK_H_
#define LINK_H_

#include <QTimer>
#include <QTcpSocket>
#include <QList>
#include <QMap>
#include "parse.h"
#include "user.h"
#include "userparser.h"
#include "region.h"
#include "regionparser.h"

class User;
class GeoRegion;

class Link : public Parse
{
	Q_OBJECT

public:
	Link();
	bool srvConnect(QString ip, qint32 port);
	void tx(QString cmd);
	void lockUser(void);
	bool lastSucceeded(void);
	inline QString getEmployeeIdShowing(void) {return employeeIdShowing;}
	inline void setEmployeeIdShowing(QString value) {employeeIdShowing = value;}
	inline QString getRegionIdAt(qint32 i) const {return regionList[i];}
	GeoRegion *getRegionAt(qint32 i);
	inline GeoRegion *getRegion(QString id) {return regions[id];}
	inline void appendToRegionList(QString regionID) {regionList.append(regionID);}
	inline qint32 regionListCount(void) {return regionList.count();}
	inline QStringList getRegionList(void) {return regionList;}
	inline qint32 indexOfRegionList(QString str) {return regionList.indexOf(str);}
	inline QStringList getAllManagers(void) {return allManagers;}
	inline User *getLoggedInUser(void) {return loggedInUser;}
	inline User *getEmployee(void) {return employee;}


public slots:
	void unlockUser(void);

private:
	bool parseObjects(QObject *ref, QString type, QString id, QString tail);
	bool parseAction(QObject *ref, QString cmd, QString value, qint32 n);
	QTimer timeout;
	QTcpSocket server;
	bool succeeded;
	QString employeeIdShowing;
	QStringList allManagers, regionList;
	User *loggedInUser, *employee;
	QMap <QString, GeoRegion *> regions;
	QMap <QString, User *> users;
	UserParser userParser;
	RegionParser regionParser;

signals:
	void success(void);
	void error(QString error);
	void error(void);
	void completed(void);
	void disconnected(void);
	void updateEmployeeList(void);
	void updateEmployee(void);

private slots:
	void rx(void);
	void stopSignals(void);
	void srvDisconnect(void);
	void checkChange(User *changedUser);

	
protected:


};

#endif /* LINK_H_ */
