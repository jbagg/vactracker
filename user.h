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
   File name    : user.h
   Created      : 28 May 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   This is the user class which holds withdrawals and deposits as well as general info about the
   user.  An instance of this class exists for each user.
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef USER_H_
#define USER_H_

#include <QObject>
#include <QList>
#include <QStringList>
#include "deposit.h"
#include "withdrawal.h"
#include "userparser.h"

enum user_type {
	UT_NORMAL,
	UT_MANAGER,
	UT_ADMIN,
	UT_DISABLED,
};

class User : public QObject
{
	Q_OBJECT

friend UserParser;

public:
	inline QString getID(void) {return id;}
	inline void setID(QString value) {id = value;}
	inline bool hasReportsTo(void) {return reportsTo.size() ? 1:0;}
	inline QString getFirstName(void) {return firstName;}
	inline QString getLastName(void) {return lastName;}
	inline QString getReportsTo(void) {return reportsTo;}
	inline QString getEmail(void) {return email;}
	inline QString getRegion(void) {return region;}
	inline quint32 getVersion(void) {return version;}
	inline quint32 getAnnualAllotment(void) {return annualAllotment;}
	inline bool hasEmployee(QString id) {return employees.contains(id);}
	inline QString getEmployeeAt(qint32 i) {return employees[i];}
	inline qint32 indexOfEmployee(QString i) {return employees.indexOf(i);}
	inline qint32 employeeSize(void) {return employees.size();}
	inline QStringList getPossibleManagers(void) {return possibleManagers;}
	inline qint32 indexOfPossibleManagers(QString i) {return possibleManagers.indexOf(i);}
	inline Deposit *getDepositAt(qint32 i) {return deposits[i];}
	inline qint32 depositsSize(void) {return deposits.size();}
	inline Withdrawal *getWithdrawalAt(qint32 i) {return withdrawals[i];}
	inline qint32 withdrawalsSize(void) {return withdrawals.size();}
	inline user_type getType(void) {return type;}

signals:
	void changed(User *user);

private:
	QString id, firstName, lastName, reportsTo, email, region;
	quint32 version, annualAllotment;
	QStringList employees;		// for managers only
	QStringList possibleManagers;
	QList <Deposit *> deposits;
	QList <Withdrawal *> withdrawals;
	user_type type;

};

#endif /* USER_H_ */
