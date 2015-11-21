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
   File name    : mainWindow.cpp
   Created      : 05 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Main Window
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QDate>
#include <QCryptographicHash>
#include <QDesktopWidget>
#include "mainWindow.h"
#include "qtreespecialitem.h"

mainWindow::mainWindow()
{
	discover = new Discover(&link, this);
	vacEdit = new vacEditDialog(&link, this);
	userEdit = new userEditDialog(&link, this);
	regionEdit = new regionEditDialog(&link, this);
	pwdChange = new pwdChangeDialog(&link, this);
	addDeposit = new addDepositDialog(&link, this);
	buildGui();

	connect(&link, SIGNAL(disconnected()), this, SLOT(srvDisconnect()));
	connect(&link, SIGNAL(updateEmployeeList()), this, SLOT(buildEmployeeList()));
	connect(&link, SIGNAL(updateEmployee()), this, SLOT(buildHistory()));
	connect(&link, SIGNAL(error(QString)), this, SLOT(showError(QString)));
	connect(&history, SIGNAL(itemSelectionChanged()), this, SLOT(showDetails()));

	connect(&chgVac, SIGNAL(clicked()), this, SLOT(changeClicked()));
	connect(&cancel, SIGNAL(clicked()), this, SLOT(cancelClicked()));
	connect(&approve, SIGNAL(clicked()), this, SLOT(approvedClicked()));
	connect(&reject, SIGNAL(clicked()), this, SLOT(rejectClicked()));
	connect(&newVacBtn, SIGNAL(clicked()), this, SLOT(newVacClicked()));
	connect(&chgPwd, SIGNAL(clicked()), pwdChange, SLOT(open()));

	setWindowState(Qt::WindowMaximized);
	discover->start();
}

void mainWindow::buildGui(void)
{
	QVBoxLayout *layout = new QVBoxLayout;
	QHBoxLayout *mainLayout = new QHBoxLayout;
	QVBoxLayout *historyLayout = new QVBoxLayout;
	QVBoxLayout *infoLayout = new QVBoxLayout;
	QGridLayout *summaryLayout = new QGridLayout;
	QGridLayout *depositLayout = new QGridLayout;
	QGridLayout *withdrawalLayout = new QGridLayout;
	
	historyLabel.setText(tr("History"));
	yearLabel.setText("2015");
	history.setHeaderLabels(QStringList() << tr("Year") << tr("Amount") << tr("Description") << tr("Running Balance"));
	
	stateLabel.setText(tr("Status") + ":");
	startLabel.setText(tr("Start") + ":");
	endLabel.setText(tr("End") + ":");
	lengthLabel.setText(tr("Length") + ":");
	requestDateLabel.setText(tr("Request Date") + ":");
	approvedDateLabel.setText(tr("Approved Date") + ":");
	approvedByLabel.setText(tr("Approved By") + ":");

	chgVac.setText(tr("Change"));
	cancel.setText(tr("Cancel"));
	approve.setText(tr("Approve"));
	reject.setText(tr("Reject"));
	#ifdef Q_OS_ANDROID
	newVacBtn.setText(tr("Request"));
	#else
	newVacBtn.setText(tr("New Vacation Request"));
	#endif
	chgPwd.setText(tr("Change Password"));

	summaryBox.setTitle(tr("Summary"));
	depositBox.setTitle(tr("Allotment Details"));
	withdrawalBox.setTitle(tr("Vacation Details"));

	balanceLabel.setText(tr("Balance") + ":");
	requestedLabel.setText(tr("Awaiting approval") + ":");
	rejectedLabel.setText(tr("Rejected") + ":");
	summaryLayout->addWidget(&balanceLabel, 0, 0, Qt::AlignRight);
	summaryLayout->addWidget(&balanceValue, 0, 1, Qt::AlignLeft);
	summaryLayout->addWidget(&requestedLabel, 1, 0, Qt::AlignRight);
	summaryLayout->addWidget(&requestedValue, 1, 1, Qt::AlignLeft);
	summaryLayout->addWidget(&rejectedLabel, 2, 0, Qt::AlignRight);
	summaryLayout->addWidget(&rejectedValue, 2, 1, Qt::AlignLeft);
	summaryBox.setLayout(summaryLayout);

	amountLabel.setText(tr("Amount") + ":");
	depositLayout->addWidget(&amountLabel, 0, 0, Qt::AlignRight);
	depositLayout->addWidget(&amountValue, 0, 1, Qt::AlignLeft);
	depositBox.setLayout(depositLayout);
	depositBox.hide();

	withdrawalLayout->addWidget(&stateLabel, 0, 0, Qt::AlignRight);
	withdrawalLayout->addWidget(&stateValue, 0, 1, Qt::AlignLeft);
	withdrawalLayout->addWidget(&startLabel, 1, 0, Qt::AlignRight);
	withdrawalLayout->addWidget(&startValue, 1, 1, Qt::AlignLeft);
	withdrawalLayout->addWidget(&endLabel, 2, 0, Qt::AlignRight);
	withdrawalLayout->addWidget(&endValue, 2, 1, Qt::AlignLeft);
	withdrawalLayout->addWidget(&lengthLabel, 3, 0, Qt::AlignRight);
	withdrawalLayout->addWidget(&lengthValue, 3, 1, Qt::AlignLeft);
	withdrawalLayout->addWidget(&requestDateLabel, 4, 0, Qt::AlignRight);
	withdrawalLayout->addWidget(&requestDateValue, 4, 1, Qt::AlignLeft);
	withdrawalLayout->addWidget(&approvedDateLabel, 5, 0, Qt::AlignRight);
	withdrawalLayout->addWidget(&approvedDateValue, 5, 1, Qt::AlignLeft);
	withdrawalLayout->addWidget(&approvedByLabel, 6, 0, Qt::AlignRight);
	withdrawalLayout->addWidget(&approvedByValue, 6, 1, Qt::AlignLeft);
	#ifndef Q_OS_ANDROID
	withdrawalLayout->addWidget(&chgVac, 1, 2, Qt::AlignHCenter);
	withdrawalLayout->addWidget(&cancel, 6, 2, Qt::AlignHCenter);
	withdrawalLayout->addWidget(&approve, 7, 1, Qt::AlignHCenter);
	withdrawalLayout->addWidget(&reject, 7, 2, Qt::AlignHCenter);
	#endif
	withdrawalBox.setLayout(withdrawalLayout);
	withdrawalBox.hide();

	#ifndef Q_OS_ANDROID
	QMenu *userMenu = new QMenu(tr("Employees"));
	userMenu->addAction(tr("Add Employee"), this, SLOT(newUserSelected()));
	userMenu->addAction(tr("Edit Employee"), this, SLOT(editUserSelected()));
	userMenu->addAction(tr("Add Deposit"), addDeposit, SLOT(prep()));
	QMenu *regionMenu = new QMenu(tr("Regions"));
	regionMenu->addAction(tr("Edit Region"), regionEdit, SLOT(prep()));
	menu.addMenu(userMenu);
	menu.addMenu(regionMenu);
	#endif

	//infoLayout->setAlignment(Qt::AlignTop);
	summaryBox.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
#ifndef Q_OS_ANDROID
	infoLayout->addWidget(&summaryBox);
	infoLayout->addWidget(&yearLabel);
	infoLayout->addWidget(&withdrawalBox);
	infoLayout->addWidget(&depositBox);
#endif
	infoLayout->addWidget(&newVacBtn);
#ifdef Q_OS_ANDROID
	infoLayout->addWidget(&chgVac);
	infoLayout->addWidget(&cancel);
	infoLayout->addWidget(&approve);
	infoLayout->addWidget(&reject);
#endif
	infoLayout->addStretch();
	infoLayout->addWidget(&chgPwd);
	historyLayout->addWidget(&employeeSelector);
	historyLayout->addWidget(&historyLabel);
	history.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);	// this fills up window for full screen
	historyLayout->addWidget(&history);
	mainLayout->addLayout(historyLayout);
	mainLayout->addLayout(infoLayout);

	setMenuBar(&menu);
	layout->addLayout(mainLayout);
	QWidget *widget = new QWidget;
	widget->setLayout(layout);
	setCentralWidget(widget);

	errorMsgbox.setIcon(QMessageBox::Warning);
}

void mainWindow::buildEmployeeList(void)
{
	qint32 i;
	QString oldID;

	if (link.getLoggedInUser()->employeeSize()) {
		oldID = employeeSelector.currentText();
		disconnect(&employeeSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(changeEmployee(int)));
		employeeSelector.clear();
		employeeSelector.addItem(link.getLoggedInUser()->getID());
		for (i=0; i<link.getLoggedInUser()->employeeSize(); i++)
			employeeSelector.addItem(link.getLoggedInUser()->getEmployeeAt(i));

		if (oldID != link.getLoggedInUser()->getID()) {	// don't fix selected employee if employee is the logged in user
			if (link.getLoggedInUser()->hasEmployee(oldID))
				employeeSelector.setCurrentIndex(link.getLoggedInUser()->indexOfEmployee(oldID) + 1);	// show the same user
			else
				changeEmployee(0);	// oldID (employee) is not in the new list, so switch to logged in user and refresh
		}

		connect(&employeeSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(changeEmployee(int)));
		employeeSelector.show();
	}
	else
		employeeSelector.hide();
}

void mainWindow::buildHistory(void)
{
	QList <QTreeWidgetItem *> search;
	QTreeWidgetItem *yearBranch, *last = NULL;
	QDate startDate, endDate;
	QString year;
	qint32 i;
	qreal earned, pending, approved, rejected;

	history.clear();
	earned = pending = approved = rejected = 0;
	for(i=0; i<link.getEmployee()->depositsSize(); i++) {
		QTreeDepositItem *item;
		year = QString::number(link.getEmployee()->getDepositAt(i)->getYear());
		search = history.findItems(year, 0);
		if (!search.size()) {
			yearBranch = new QTreeWidgetItem(&history);
			yearBranch->setText(0, year);
			item = new QTreeDepositItem(yearBranch);
		}
		else {
			item = new QTreeDepositItem(search.first());
		}
		last = item;
		item->deposit = link.getEmployee()->getDepositAt(i);
		item->setText(0, tr("Earned"));
		item->setText(1, QString::number(link.getEmployee()->getDepositAt(i)->getAmount()));
		//item->setText(2, tr("Earned"));

		earned+= link.getEmployee()->getDepositAt(i)->getAmount();
	}
	for(i=0; i<link.getEmployee()->withdrawalsSize(); i++) {
		QTreeWithdrawalItem *item;
		startDate = QDate::fromJulianDay(link.getEmployee()->getWithdrawalAt(i)->getStart());
		year = QString::number(startDate.year());
		search = history.findItems(year, 0);
		if (!search.size()) {
			yearBranch = new QTreeWidgetItem(&history);
			yearBranch->setText(0, year);
			item = new QTreeWithdrawalItem(yearBranch);
		}
		else {
			item = new QTreeWithdrawalItem(search.first());
		}
		last = item;
		item->withdrawal = link.getEmployee()->getWithdrawalAt(i);
		item->setText(0, tr("Vac"));
		item->setText(1, QString::number(link.getEmployee()->getWithdrawalAt(i)->getLength()));
		if (link.getEmployee()->getWithdrawalAt(i)->getLength() > 1) {
			endDate = QDate::fromJulianDay(link.getEmployee()->getWithdrawalAt(i)->getEnd());
			item->setText(2, QDate::shortMonthName(startDate.month()) + ' ' + QString::number(startDate.day())
							+ " - " + QDate::shortMonthName(endDate.month()) + ' ' + QString::number(endDate.day()));
		}
		else {
			item->setText(2, QDate::shortMonthName(startDate.month()) + ' ' + QString::number(startDate.day()));
		}

		qreal length = link.getEmployee()->getWithdrawalAt(i)->getLength();
		switch(link.getEmployee()->getWithdrawalAt(i)->getState()) {
			case WDRL_REQUEST: pending+= length; break;
			case WDRL_APPROVED:
			case WDRL_CANCEL_REQUEST: approved+= length; break;
			case WDRL_REJECTED: rejected+= length; break;
			case WDRL_CANCELLED: break;
		}
	}
	makeRunningBalance();
	//if (search.size())
	//	search.first()->setExpanded(1);	// expand the last activity
	if (last) {
		history.setCurrentItem(last);	// show the lastest activity
		showDetails();
	}
	else {
		withdrawalBox.hide();
		depositBox.hide();
	}
	for(i=0; i<history.columnCount(); i++)
		history.resizeColumnToContents(i);
	
	if (link.getLoggedInUser() == link.getEmployee())
		newVacBtn.setEnabled(1);
	else
		newVacBtn.setEnabled(0);

	if (link.getLoggedInUser() == link.getEmployee() || link.getLoggedInUser()->getType() == UT_ADMIN)
		chgPwd.setEnabled(1);
	else
		chgPwd.setEnabled(0);

	if (link.getLoggedInUser()->getType() == UT_ADMIN)
		menu.show();
	else
		menu.hide();

	balanceValue.setText(QString::number(earned - pending - approved));
	requestedValue.setText(QString::number(pending));
	rejectedValue.setText(QString::number(rejected));

	historyLabel.setText(tr("History - ") + link.getEmployee()->getFirstName() + " " + link.getEmployee()->getLastName());
}

void mainWindow::makeRunningBalance(void)
{
	QTreeDepositItem *depositItem;
	QTreeWithdrawalItem *withdrawalItem;
	qreal balance = 0;

	QTreeWidgetItemIterator it(&history);
	while (*it) {
		if ((*it)->type() == DepositItem) {
			depositItem = static_cast<QTreeDepositItem*>(*it);
			balance+= depositItem->deposit->getAmount();
			(*it)->setText(3, "  " + QString::number(balance));
		}
		else if ((*it)->type() == WithdrawalItem) {
			withdrawalItem = static_cast<QTreeWithdrawalItem*>(*it);
			switch(withdrawalItem->withdrawal->getState()) {
				case WDRL_REQUEST:
				case WDRL_APPROVED:
				case WDRL_CANCEL_REQUEST: balance-= withdrawalItem->withdrawal->getLength(); break;
				case WDRL_REJECTED: break;
				case WDRL_CANCELLED: break;
			}
			(*it)->setText(3, "  " + QString::number(balance));
		}
		++it;
	}
}

void mainWindow::showDetails(void)
{
	QTreeWidgetItem *item;
	#ifndef Q_OS_ANDROID
	QTreeDepositItem *depositItem;
	#endif
	QTreeWithdrawalItem *withdrawalItem;

	item = history.currentItem();
	if (item->type() == WithdrawalItem) {
		withdrawalItem = static_cast<QTreeWithdrawalItem*>(item);
		showWithdrawal(withdrawalItem->withdrawal);
	}
	#ifndef Q_OS_ANDROID
	else if (item->type() == DepositItem) {
		depositItem = static_cast<QTreeDepositItem*>(item);
		showDeposit(depositItem->deposit);
	}
	#endif
}

void mainWindow::showDeposit(Deposit *deposit)
{
	withdrawalBox.hide();
	depositBox.show();

	yearLabel.setText(QString::number(deposit->getYear()));
	amountValue.setText(QString::number(deposit->getAmount()));
}

void mainWindow::showWithdrawal(Withdrawal *withdrawal)
{
	QDate date;

	#ifndef Q_OS_ANDROID
	depositBox.hide();
	withdrawalBox.show();
	#endif

	index = withdrawal->getID();
	switch (withdrawal->getState()) {
		case WDRL_REQUEST: stateValue.setText(tr("Requested")); break;
		case WDRL_APPROVED: stateValue.setText(tr("Approved")); break;
		case WDRL_REJECTED: stateValue.setText(tr("Rejected")); break;
		case WDRL_CANCEL_REQUEST: stateValue.setText(tr("Cancel Requested")); break;
		case WDRL_CANCELLED: stateValue.setText(tr("Cancelled")); break;
	}
	date = QDate::fromJulianDay(withdrawal->getStart());
	yearLabel.setText(QString::number(date.year()));
	startValue.setText(getDayMonthStringfromDate(date));
	date = QDate::fromJulianDay(withdrawal->getEnd());
	endValue.setText(getDayMonthStringfromDate(date));
	lengthValue.setText(QString::number(withdrawal->getLength()));
	date = QDate::fromJulianDay(withdrawal->getRequestDate());
	requestDateValue.setText(getDayMonthStringfromDate(date));
	if (withdrawal->getApprovedDate() != 0) {
		date = QDate::fromJulianDay(withdrawal->getApprovedDate());
		approvedDateValue.setText(getDayMonthStringfromDate(date));
	}
	else
		approvedDateValue.setText(tr("NA"));
	approvedByValue.setText(withdrawal->getApprovedBy());

	if (link.getLoggedInUser() == link.getEmployee() || link.getLoggedInUser()->getType() == UT_ADMIN) {
		chgVac.show();
		if (withdrawal->getState() == WDRL_REQUEST)
			chgVac.setEnabled(1);
		else
			chgVac.setEnabled(0);

		cancel.show();
		if ((withdrawal->getState() == WDRL_REQUEST || withdrawal->getState() == WDRL_APPROVED))
			cancel.setEnabled(1);
		else
			cancel.setEnabled(0);
	}
	else {
		chgVac.hide();
		cancel.hide();
	}
/*
	if ((link.loggedInUser == link.employee && (withdrawal->state == WDRL_REQUEST || withdrawal->state == WDRL_APPROVED)) || link.loggedInUser->type == UT_ADMIN)
		cancel.show();
	else
		cancel.hide();
*/
	if ((link.getLoggedInUser()->getType() == UT_MANAGER && link.getLoggedInUser() != link.getEmployee())	// (link.loggedInUser != link.employee) stops managers from approving their own vacation
			|| !link.getEmployee()->hasReportsTo()			// CEO needs to be able to approve their own vacation
			|| link.getLoggedInUser()->getType() == UT_ADMIN) {
		approve.show();
		reject.show();
		//if (withdrawal->state == WDRL_REQUEST || withdrawal->state == WDRL_CANCEL_REQUEST || link.loggedInUser->type == UT_ADMIN) {
		if (withdrawal->getState() == WDRL_REQUEST || withdrawal->getState() == WDRL_CANCEL_REQUEST) {
			approve.setEnabled(1);
			reject.setEnabled(1);
		}
		else {
			approve.setEnabled(0);
			reject.setEnabled(0);
		}
	}
	else {
		approve.hide();
		reject.hide();
	}
}


void mainWindow::newVacClicked(void)
{
	vacNew = 1;
	connect(&link, SIGNAL(success()), this, SLOT(doChange()));
	link.lockUser();
}

void mainWindow::changeClicked(void)
{
	vacNew = 0;
	connect(&link, SIGNAL(success()), this, SLOT(doChange()));
	link.lockUser();
}

void mainWindow::doChange(void)
{
	disconnect(&link, SIGNAL(success()), 0, 0);
	if (vacNew)
		vacEdit->setNew(link.getEmployee()->withdrawalsSize());
	else
		vacEdit->setEdit(link.getEmployee()->getWithdrawalAt(index), index);
	setEnabled(0);
	vacEdit->show();

	#ifndef Q_OS_ANDROID
	QDesktopWidget screen;
	vacEdit->move((screen.screenGeometry().width() - vacEdit->size().width())/2, (screen.screenGeometry().height() - vacEdit->size().height())/2);
	#endif
}

void mainWindow::newUserSelected()
{
	userNew = 1;
	connect(&link, SIGNAL(success()), this, SLOT(getRegionList()));
	link.tx("allManagers?\n");
}

void mainWindow::editUserSelected()
{
	userNew = 0;
	connect(&link, SIGNAL(success()), this, SLOT(getPossibleManagers()));
	link.lockUser();
}

void mainWindow::getPossibleManagers(void)
{
	disconnect(&link, SIGNAL(success()), 0, 0);
	connect(&link, SIGNAL(success()), this, SLOT(getRegionList()));
	link.tx("user[" + link.getEmployee()->getID() + "].possibleManagers?\n");
}

void mainWindow::getRegionList(void)
{
	disconnect(&link, SIGNAL(success()), 0, 0);
	connect(&link, SIGNAL(success()), this, SLOT(doUserEdit()));
	link.tx("regionList?\n");
}

void mainWindow::doUserEdit()
{
	disconnect(&link, SIGNAL(success()), 0, 0);
	if (userNew)
		userEdit->setNew();
	else
		userEdit->setEdit(link.getEmployee());
	setEnabled(0);
	userEdit->show();
	QDesktopWidget screen;
	userEdit->move((screen.screenGeometry().width() - userEdit->size().width())/2, (screen.screenGeometry().height() - userEdit->size().height())/2);
}

void mainWindow::cancelClicked(void)
{
	connect(&link, SIGNAL(success()), this, SLOT(doCancel()));
	link.lockUser();
}

void mainWindow::doCancel(void)
{
	disconnect(&link, SIGNAL(success()), 0, 0);
	connect(&link, SIGNAL(success()), &link, SLOT(unlockUser()));
	connect(&link, SIGNAL(error()), &link, SLOT(unlockUser()));
	link.tx("user[" + link.getEmployee()->getID() + "].withdrawal[" + QString::number(index) + "].state=cancel_request\n");
}

void mainWindow::approvedClicked(void)
{
	connect(&link, SIGNAL(success()), this, SLOT(doApproved()));
	link.lockUser();
}

void mainWindow::doApproved(void)
{
	disconnect(&link, SIGNAL(success()), 0, 0);
	connect(&link, SIGNAL(success()), &link, SLOT(unlockUser()));
	connect(&link, SIGNAL(error()), &link, SLOT(unlockUser()));
	if (link.getEmployee()->getWithdrawalAt(index)->getState() == WDRL_REQUEST)
		link.tx("user[" + link.getEmployee()->getID() + "].withdrawal[" + QString::number(index) + "].state=approved" +
				"&approved_date=" + QString::number(QDate::currentDate().toJulianDay()) +
				"&approved_by=" + link.getLoggedInUser()->getID() + '\n');
	else
		link.tx("user[" + link.getEmployee()->getID() + "].withdrawal[" + QString::number(index) + "].state=cancelled\n");
}

void mainWindow::rejectClicked(void)
{
	connect(&link, SIGNAL(success()), this, SLOT(doReject()));
	link.lockUser();
}

void mainWindow::doReject(void)
{
	disconnect(&link, SIGNAL(success()), 0, 0);
	connect(&link, SIGNAL(success()), &link, SLOT(unlockUser()));
	connect(&link, SIGNAL(error()), &link, SLOT(unlockUser()));
	if (link.getEmployee()->getWithdrawalAt(index)->getState() == WDRL_REQUEST)
		link.tx("user[" + link.getEmployee()->getID() + "].withdrawal[" + QString::number(index) + "].state=rejected" +
				"&approved_date=" + QString::number(QDate::currentDate().toJulianDay()) +
				"&approved_by=" + link.getLoggedInUser()->getID() + '\n');
	else
		link.tx("user[" + link.getEmployee()->getID() + "].withdrawal[" + QString::number(index) + "].state=approved\n");
}

void mainWindow::srvDisconnect(void)
{
	QMessageBox msgbox;
	msgbox.setIcon(QMessageBox::Critical);
	msgbox.setText(tr("Server Disconnected - exiting"));
	msgbox.setStandardButtons(QMessageBox::Ok);
	msgbox.exec();
	exit(1);
}

void mainWindow::changeEmployee(qint32 i)
{
	if (i < 0)
		return;
	if (i == 0) {
		link.setEmployeeIdShowing(link.getLoggedInUser()->getID());
	}
	else {
		link.setEmployeeIdShowing(link.getLoggedInUser()->getEmployeeAt(i-1));
	}
	link.tx("user[" + link.getEmployeeIdShowing() + "].refresh?\n");
}

void mainWindow::showError(QString error)
{
	errorMsgbox.setText(error);
	errorMsgbox.open();
}

void mainWindow::closeEvent(QCloseEvent *event)
{
	event->accept();
	exit(1);
}

QString mainWindow::getDayMonthStringfromDate(QDate date)
{
	#ifdef Q_OS_ANDROID
	return QDate::shortMonthName(date.month()) + ' ' + QString::number(date.day());
	#else
	return QDate::longDayName(date.dayOfWeek()) + ", " + QDate::longMonthName(date.month()) + ' ' + QString::number(date.day());
	#endif
}
