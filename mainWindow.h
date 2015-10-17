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
   File name    : mainWindow.h
   Created      : 05 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Main Window
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>
#include <QComboBox>
#include <QMenuBar>

#include <QMainWindow>
#include "link.h"
#include "discover.h"
#include "vacEdit.h"
#include "userEdit.h"
#include "regionEdit.h"
#include "pwdChange.h"
#include "addDeposit.h"

class Discover;
class vacEditDialog;
class userEditDialog;
class regionEditDialog;
class pwdChangeDialog;
class addDepositDialog;

class mainWindow : public QMainWindow
{
	Q_OBJECT

public:
	mainWindow();
	static QString getDayMonthStringfromDate(QDate date);

private:
	void closeEvent(QCloseEvent *event);
	Link link;
	QString userID;
	qint32 index;

	//gui
	void buildGui(void);
	void showDeposit(Deposit *deposit);
	void showWithdrawal(Withdrawal *withdrawal);
	void makeRunningBalance(void);
	QGroupBox summaryBox, withdrawalBox, depositBox;
	QLabel historyLabel, yearLabel;
	QLabel balanceLabel, balanceValue, requestedLabel, requestedValue, rejectedLabel, rejectedValue;
	QLabel stateLabel, startLabel, endLabel, lengthLabel, amountLabel, requestDateLabel, approvedDateLabel, approvedByLabel;
	QLabel stateValue, startValue, endValue, lengthValue, amountValue, requestDateValue, approvedDateValue, approvedByValue;
	QPushButton chgVac, cancel, approve, reject, newVacBtn, chgPwd;
	QTreeWidget history;
	QComboBox employeeSelector;
	QMenuBar menu;
	QMessageBox errorMsgbox;
	Discover *discover;
	vacEditDialog *vacEdit;
	userEditDialog *userEdit;
	regionEditDialog *regionEdit;
	pwdChangeDialog *pwdChange;
	addDepositDialog *addDeposit;
	bool vacNew, userNew;

private slots:
	void srvDisconnect(void);
	void buildEmployeeList(void);
	void buildHistory(void);
	void showDetails(void);

	void newVacClicked(void);
	void changeClicked(void);
	void doChange(void);

	void newUserSelected();
	void getRegionList(void);
	void editUserSelected();
	void getPossibleManagers();
	void doUserEdit(void);

	void cancelClicked(void);
	void doCancel(void);
	void approvedClicked(void);
	void doApproved(void);
	void rejectClicked(void);
	void doReject(void);
	void changeEmployee(int i);
	void showError(QString error);
};

#endif /* MAINWINDOW_H_ */
