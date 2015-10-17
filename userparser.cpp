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
   File name    : userparser.cpp
   Created      : 05 Oct 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Command parser for the User class
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include "user.h"
#include "userparser.h"

UserParser::UserParser()
{
	intProperties << "version" << "annualAllotment";
	stringProperties << "firstName" << "lastName" << "email" << "reportsTo" << "region";
	actions << "update" << "type" << "employees" << "possibleManagers";
}

bool UserParser::parseObjects(QObject *ref, QString type, QString id, QString tail)
{
	User *user;
	Deposit *deposit;
	Withdrawal *withdrawal;
	qint32 i;
	bool found = 0;

	user = static_cast<User*>(ref);
	i = id.toInt();
	if (type == "withdrawal") {
		if (i > user->withdrawals.size()) {
			return 0;
		}
		if (i == user->withdrawals.size()) {
			withdrawal = new Withdrawal(i);
			user->withdrawals.append(withdrawal);
		}
		if (i < user->withdrawals.size())
			found = withdrawalParser.parseCmd(user->withdrawals[i], tail);
	}
	else if (type == "deposit") {
		if (i > user->deposits.size()) {
			return 0;
		}
		if (i == user->deposits.size()) {
			deposit = new Deposit;
			deposit->setID(i);
			user->deposits.append(deposit);
		}
		if (i < user->deposits.size())
			found = depositParser.parseCmd(user->deposits[i], tail);
	}

	return found;
}

bool UserParser::parseAction(QObject *ref, QString cmd, QString value, qint32 n)
{
	User *user;
	bool found = 0;

	user = static_cast<User*>(ref);
	if (cmd == "update") {
		found = 1;
		emit user->changed(user);
	}
	else if (cmd == "type") {
		if (value == "normal")
			user->type = UT_NORMAL;
		else if (value == "manager")
			user->type = UT_MANAGER;
		else if (value == "admin")
			user->type = UT_ADMIN;
		else if (value == "disabled")
			user->type = UT_DISABLED;
	}
	else if (cmd == "employees") {
		found = 1;
		if (!n)
			user->employees.clear();
		if (value.size() && !user->employees.contains(value))
			user->employees.append(value);
	}
	else if (cmd == "possibleManagers") {
		found = 1;
		if (!n)
			user->possibleManagers.clear();
		if (value.size() && !user->possibleManagers.contains(value))
			user->possibleManagers.append(value);
	}

	return found;
}

void UserParser::parseInt(QObject *ref, QString id, qint32 value)
{
	User *user;

	user = static_cast<User*>(ref);
	if (id == "version")
		user->version = value;
	else if (id == "annualAllotment")
		user->annualAllotment = value;
}

void UserParser::parseString(QObject *ref, QString id, QString value)
{
	User *user;

	user = static_cast<User*>(ref);
	if (id == "firstName")
		user->firstName = value;
	else if (id == "lastName")
		user->lastName = value;
	else if (id == "email")
		user->email = value;
	else if (id == "reportsTo")
		user->reportsTo = value;
	else if (id == "region")
		user->region = value;
}
