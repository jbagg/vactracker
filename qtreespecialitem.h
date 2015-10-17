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
   File name    : qtreespecialitem.h
   Created      : 10 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   QTreeWidgetItem subclasses - QTreeDepositItem & QTreeWithdrawalItem
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include "deposit.h"
#include "withdrawal.h"

enum ItemType
{
	DepositItem = QTreeWidgetItem::UserType + 1,
	WithdrawalItem,
};

class QTreeDepositItem : public QTreeWidgetItem
{
public:

	QTreeDepositItem(QTreeWidgetItem *item) : QTreeWidgetItem(item, DepositItem) {}
	Deposit *deposit;

};

class QTreeWithdrawalItem : public QTreeWidgetItem
{
public:

	QTreeWithdrawalItem(QTreeWidgetItem *item) : QTreeWidgetItem(item, WithdrawalItem) {}
	Withdrawal *withdrawal;

};
