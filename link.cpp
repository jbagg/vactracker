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
   File name    : link.cpp
   Created      : 08 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Handles communication to the server
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include "link.h"

Link::Link()
{
	loggedInUser = NULL;
	employee = NULL;
	succeeded = 0;

	actions << "managers" << "regionList" << "error";

	connect(&server, SIGNAL(readyRead()), this, SLOT(rx()));
	connect(&server, SIGNAL(disconnected()), this, SLOT(srvDisconnect()));
}

bool Link::srvConnect(QString ip, qint32 port)
{
	server.connectToHost(ip, port);
	return server.waitForConnected(1000);
}

void Link::tx(QString cmd)
{
	server.write(cmd.toUtf8());
}

void Link::rx(void)
{
	qint32	i;

	rxStream+= server.read(4096);
	QStringList cmds = rxStream.split(QRegExp("[\r\n]"));
	for (i=0; i<cmds.size()-1; i++)
	{
		if (cmds[i] == "ok") {
			succeeded = 1;
			emit completed();
			emit success();
		}
		else if (cmds[i].contains("error=")) {
			succeeded = 0;
			emit completed();
			emit error();
		}
		else
			parseCmd(this, cmds[i]);

	}

	if (cmds.size() > 1)
	{
		rxStream.clear();
		if (cmds.at(i).size())		// if string size != 0, there is a partial command leftover
			rxStream+= cmds.at(i);
	}
}
void Link::lockUser(void)
{
	tx("user[" + employee->getID() + "].lock=" + QString::number(employee->getVersion()) + "\n");
}

void Link::unlockUser(void)
{
	disconnect(this, SIGNAL(completed()), 0, 0);
	disconnect(this, SIGNAL(success()), 0, 0);
	disconnect(this, SIGNAL(error()), 0, 0);
	tx("user[" + employee->getID() + "].unlock\n");
}

void Link::srvDisconnect(void)
{
	emit disconnected();
}

bool Link::parseObjects(QObject *, QString type, QString id, QString tail)
{
	bool found = 0;

	if (type == "user") {
		if (!users.contains(id)) {
			User *newUser = new User();
			newUser->setID(id);
			users[id] = newUser;
			connect(newUser, SIGNAL(changed(User *)), this, SLOT(checkChange(User *)));
		}
		found = userParser.parseCmd(users[id], tail);
	}
	else if (type == "region") {
		if (!regions.contains(id)) {
			GeoRegion *newRegion = new GeoRegion(id);
			regions[id] = newRegion;
			// no change signal needed because only one admin at a time can open the region edit dialog
		}
		found = regionParser.parseCmd(regions[id], tail);
	}
	
	return found;
}

bool Link::parseAction(QObject *, QString cmd, QString value, qint32 n)
{
	bool found = 0;

	if (cmd == "managers") {
		found = 1;
		if (!n)
			allManagers.clear();
		if (value.size()) {
			allManagers.append(value);
		}
	}
	else if (cmd == "regionList") {
		found = 1;
		if (!n)
			regionList.clear();
		if (value.size()) {
			regionList.append(value);
		}
	}
	else if (cmd == "error" && value.size()) {
		emit error(value);
		emit error();
	}

	return found;
}

void Link::checkChange(User *changedUser)
{
	if (!loggedInUser) {		// initial login update employee
		loggedInUser = changedUser;
		employee = changedUser;
	}
	if (employeeIdShowing != employee->getID()) {		// employee being shown changed
		if (users.contains(employeeIdShowing))
			employee = users[employeeIdShowing];
	}
	if (changedUser == loggedInUser)
		emit updateEmployeeList();	// refresh employee list drop down
	if (changedUser == employee)
		emit updateEmployee();	// only refresh gui if an update comes for the currently selected employee
}

bool Link::lastSucceeded(void)
{
	return succeeded;
}

GeoRegion *Link::getRegionAt(qint32 i)
{
	if (i < 0 || i >= regionList.count())
		return NULL;
	return regions[regionList[i]];
}
