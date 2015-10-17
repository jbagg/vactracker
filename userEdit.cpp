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
   File name    : userEdit.cpp
   Created      : 17 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Dialog to make / edit users (employees)
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QHBoxLayout>
#include <QCryptographicHash>
#include "userEdit.h"

#if QT_VERSION < 0x050000
	#define	QRegularExpression	QRegExp
#endif

userEditDialog::userEditDialog(Link *lk, mainWindow *win)
{
	QLabel *label;
	quint32 i = 0;

	window = win;
	link = lk;

	QGridLayout *mainLayout = new QGridLayout;

	label = new QLabel(tr("User Name") + ':');
	mainLayout->addWidget(label, i, 0, Qt::AlignRight);
	mainLayout->addWidget(&userID, i++, 1, Qt::AlignLeft);

	label = new QLabel(tr("First Name") + ':');
	mainLayout->addWidget(label, i, 0, Qt::AlignRight);
	mainLayout->addWidget(&firstName, i++, 1, Qt::AlignLeft);

	label = new QLabel(tr("Last Name") + ':');
	mainLayout->addWidget(label, i, 0, Qt::AlignRight);
	mainLayout->addWidget(&lastName, i++, 1, Qt::AlignLeft);

	label = new QLabel(tr("Email") + ':');
	mainLayout->addWidget(label, i, 0, Qt::AlignRight);
	mainLayout->addWidget(&email, i++, 1, Qt::AlignLeft);

	label = new QLabel(tr("Type") + ':');
	mainLayout->addWidget(label, i, 0, Qt::AlignRight);
	mainLayout->addWidget(&typeSelector, i++, 1, Qt::AlignLeft);
	typeSelector.addItems(QStringList() << "Normal" << "Manager" << "Admin" << "Disabled");
	typeSelector.setSizeAdjustPolicy(QComboBox::AdjustToContents);

	label = new QLabel(tr("Reports To") + ':');
	mainLayout->addWidget(label, i, 0, Qt::AlignRight);
	mainLayout->addWidget(&managerSelector, i++, 1, Qt::AlignLeft);
	managerSelector.setSizeAdjustPolicy(QComboBox::AdjustToContents);

	label = new QLabel(tr("Region") + ':');
	mainLayout->addWidget(label, i, 0, Qt::AlignRight);
	mainLayout->addWidget(&regionSelector, i++, 1, Qt::AlignLeft);
	regionSelector.setSizeAdjustPolicy(QComboBox::AdjustToContents);

	label = new QLabel(tr("Annual Allotment") + ':');
	mainLayout->addWidget(label, i, 0, Qt::AlignRight);
	mainLayout->addWidget(&allotment, i++, 1, Qt::AlignLeft);

	mainLayout->setRowStretch(i++, 1);

	QPushButton *okButton = new QPushButton;
	okButton->setText(tr("Ok"));
	mainLayout->addWidget(okButton, i, 0, Qt::AlignRight);
	connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));

	QPushButton *cancelButton = new QPushButton;
	cancelButton->setText(tr("Cancel"));
	mainLayout->addWidget(cancelButton, i, 1, Qt::AlignLeft);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

	setLayout(mainLayout);

	// Error Message Box setup
	msgbox.setIcon(QMessageBox::Critical);
	msgbox.setStandardButtons(QMessageBox::Ok);
}

void userEditDialog::setNew(void)
{
	edit = 0;
	userID.setEnabled(1);
	userID.clear();
	firstName.clear();
	//firstName.setEchoMode(QLineEdit::Password);
	lastName.clear();
	email.clear();
	typeSelector.setCurrentIndex(UT_NORMAL);
	typeSelector.setEnabled(1);
	managerSelector.clear();
	managerSelector.addItem("");
	managerSelector.addItems(link->getAllManagers());
	regionSelector.clear();
	regionSelector.addItems(link->getRegionList());
	allotment.clear();
}

void userEditDialog::setEdit(User *user)
{
	edit = 1;
	userID.setEnabled(0);
	userID.setText(user->getID());
	firstName.setText(user->getFirstName());
	lastName.setText(user->getLastName());
	email.setText(user->getEmail().replace('#', '.'));
	typeSelector.setCurrentIndex(user->getType());
	if (user == link->getLoggedInUser())
		typeSelector.setEnabled(0);		// admins can't change themselves to non admins
	else
		typeSelector.setEnabled(1);
	managerSelector.clear();
	managerSelector.addItem("");
	managerSelector.addItems(user->getPossibleManagers());
	managerSelector.setCurrentIndex(user->indexOfPossibleManagers(user->getReportsTo()) + 1);	// +1 for the blank at the start
	regionSelector.clear();
	regionSelector.addItems(link->getRegionList());
	regionSelector.setCurrentIndex(link->indexOfRegionList(user->getRegion()));
	allotment.setText(QString::number(user->getAnnualAllotment()));
}

void userEditDialog::cancel(void)
{
	if (edit)
		link->unlockUser();
	hide();
	window->setEnabled(1);
}

void userEditDialog::closeEvent(QCloseEvent *event)
{
	event->accept();
	cancel();
}

void userEditDialog::ok(void)
{
	QString cmd, emailEncoded;

	// check for blank entries
	if (!userID.text().size()) {
		msgbox.setText(tr("User ID is blank"));
		msgbox.exec();
		return;
	}
	if (!firstName.text().size()) {
		msgbox.setText(tr("First Name is blank"));
		msgbox.exec();
		return;
	}
	if (!lastName.text().size()) {
		msgbox.setText(tr("Last name is blank"));
		msgbox.exec();
		return;
	}

	if (!edit && link->getLoggedInUser()->hasEmployee(userID.text())) {	// check that user name doesn't already exist for add function
		msgbox.setText(tr("user name already exists"));
		msgbox.exec();
		return;
	}

	if (!allotment.text().size()) {
		msgbox.setText(tr("Annual Allotment is blank"));
		msgbox.exec();
		return;
	}


/*	if (!email.text().size()) {
		msgbox.setText(tr("Email address is blank"));
		msgbox.exec();
		return;
	}*/

	// check for invalid characters
	if (userID.text().contains(QRegularExpression("[[\\]&.,# -/\\\\]"))) {
		//[a-zA-Z0-9]   [[:alphanum:]]  [[:xdigit:]]""
		msgbox.setText(tr("User ID can not contain spaces or the characters") + " & . , [ ] / \\ #");
		msgbox.exec();
		return;
	}
	if (firstName.text().contains(QRegularExpression("[[\\]&.,#]"))) {
		msgbox.setText(tr("First name can not contain the characters") + " & . , [ ] #");
		msgbox.exec();
		return;
	}
	if (lastName.text().contains(QRegularExpression("[[\\]&.,#]"))) {
		msgbox.setText(tr("Last name can not contain the characters") + " & . , [ ] #");
		msgbox.exec();
		return;
	}
	if (email.text().contains(QRegularExpression("[[\\]&,#]"))) {
		msgbox.setText(tr("Email address can not contain the characters") + " & , [ ] #");
		msgbox.exec();
		return;
	}
	if (allotment.text().contains(QRegularExpression("[a-zA-Z[\\]&.,#]"))) {
		msgbox.setText(tr("Annual Allotment can only contain numbers"));
		msgbox.exec();
		return;
	}

	if (edit) {
		connect(link, SIGNAL(success()), link, SLOT(unlockUser()));
		connect(link, SIGNAL(error()), link, SLOT(unlockUser()));
	}

	cmd = "user[" + userID.text() + "].firstName=" + firstName.text();
	cmd+= "&lastName=" + lastName.text();
	if (!edit)	// password = userID when creating a new user
		cmd+= "&pwdHash=" + QCryptographicHash::hash(userID.text().toUtf8(), QCryptographicHash::Md5).toHex();
	if (email.text().size()) {
		emailEncoded = email.text().replace('.', '#');
		cmd+= "&email=" + emailEncoded;
	}
	cmd+= "&reportsTo=" + managerSelector.currentText();
	if (typeSelector.isEnabled()) {
		switch (typeSelector.currentIndex()) {
		case UT_NORMAL: cmd+= "&type=normal"; break;
		case UT_MANAGER: cmd+= "&type=manager"; break;
		case UT_ADMIN: cmd+= "&type=admin"; break;
		case UT_DISABLED: cmd+= "&type=disabled"; break;
		}
	}
	cmd+= "&region=" + regionSelector.currentText();
	cmd+= "&annualAllotment=" + allotment.text() + '\n';
	if (!edit)											// above auto-unlock unlocks current user, not new user
		cmd+= "user[" + userID.text() + "].unlock\n";	// need unlock for new user to make daemon write xml
	link->tx(cmd.toUtf8());

	hide();
	window->setEnabled(1);
}
