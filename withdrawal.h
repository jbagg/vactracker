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
   File name    : withdrawal.h
   Created      : 10 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Object to hold the details for a vacation
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef WITHDRAWAL_H_
#define WITHDRAWAL_H_

#include <QObject>
#include "withdrawalparser.h"

enum withdrawal_state {
	WDRL_REQUEST,
	WDRL_APPROVED,
	WDRL_REJECTED,
	WDRL_CANCEL_REQUEST,
	WDRL_CANCELLED,
};

class Withdrawal : public QObject
{
	Q_OBJECT

friend WithdrawalParser;

public:
	Withdrawal(quint32 newID) {id = newID;}
	inline quint32 getID(void) {return id;}
	inline withdrawal_state getState(void) {return state;}
	inline quint32 getStart(void) {return start;}
	inline quint32 getEnd(void) {return end;}
	inline qreal getLength(void) {return length;}
	inline quint32 getRequestDate(void) {return requestDate;}
	inline quint32 getApprovedDate(void) {return approvedDate;}
	inline QString getApprovedBy(void) {return approvedBy;}

private:
	quint32 id;
	withdrawal_state state;
	quint32	start, end, requestDate, approvedDate;
	qreal length;
	QString approvedBy;
};

#endif /* WITHDRAWAL_H_ */
