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
   File name    : pwdChange.cpp
   Created      : 14 July 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Dialog to edit user password
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QHBoxLayout>
#include <QCryptographicHash>
#include <QDesktopWidget>
#include "pwdChange.h"

#if QT_VERSION < 0x050000
	#define	QRegularExpression	QRegExp
#endif

pwdChangeDialog::pwdChangeDialog(Link *lk, mainWindow *win)
{
	QLabel *label;
	quint32 i = 0;

	window = win;
	link = lk;

	QGridLayout *mainLayout = new QGridLayout;

	mainLayout->setRowStretch(i++, 1);		// center grid on full screen (Android)

	label = new QLabel(tr("Password") + ':');
	mainLayout->addWidget(label, i, 0, Qt::AlignRight);
	mainLayout->addWidget(&pwd, i++, 1, Qt::AlignLeft);
	pwd.setEchoMode(QLineEdit::Password);

	label = new QLabel(tr("Confirm") + ':');
	mainLayout->addWidget(label, i, 0, Qt::AlignRight);
	mainLayout->addWidget(&pwdConfirm, i++, 1, Qt::AlignLeft);
	pwdConfirm.setEchoMode(QLineEdit::Password);

	QPushButton *okButton = new QPushButton;
	okButton->setText(tr("Ok"));
	mainLayout->addWidget(okButton, i, 0, Qt::AlignRight);
	connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));

	QPushButton *cancelButton = new QPushButton;
	cancelButton->setText(tr("Cancel"));
	mainLayout->addWidget(cancelButton, i, 1, Qt::AlignLeft);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

	resetButton.setText(tr("Reset"));
	mainLayout->addWidget(&resetButton, i, 2, Qt::AlignLeft);
	connect(&resetButton, SIGNAL(clicked()), this, SLOT(reset()));

	mainLayout->setRowStretch(++i, 1);		// center grid on full screen (Android)

	setLayout(mainLayout);

	// Error Message Box setup
	msgbox.setIcon(QMessageBox::Critical);
	msgbox.setStandardButtons(QMessageBox::Ok);
}

void pwdChangeDialog::open(void)
{
	pwd.clear();
	pwdConfirm.clear();

	connect(link, SIGNAL(success()), this, SLOT(locked()));
	link->lockUser();
}

void pwdChangeDialog::locked(void)
{
	disconnect(link, SIGNAL(success()), 0, 0);
	if (link->getLoggedInUser()->getType() == UT_ADMIN)
		resetButton.show();
	else
		resetButton.hide();
	show();
	#ifndef Q_OS_ANDROID
	QDesktopWidget screen;
	move((screen.screenGeometry().width() - size().width())/2, (screen.screenGeometry().height() - size().height())/2);
	#endif
}

void pwdChangeDialog::cancel(void)
{
	link->unlockUser();
	hide();
	window->setEnabled(1);
}

void pwdChangeDialog::closeEvent(QCloseEvent *event)
{
	event->accept();
	cancel();
}

void pwdChangeDialog::ok(void)
{
	// check for blank entries
	if (!pwd.text().size()) {
		msgbox.setText(tr("Password is blank"));
		msgbox.exec();
		return;
	}
	if (!pwdConfirm.text().size()) {
		msgbox.setText(tr("Confirm Password is blank"));
		msgbox.exec();
		return;
	}

	// min length check
	if (pwd.text().size() < 4) {
		msgbox.setText(tr("Password must be at least 4 characters"));
		msgbox.exec();
		return;
	}

	// check if passwords match
	if (pwd.text() != pwdConfirm.text()) {
		msgbox.setText(tr("Passwords do not match"));
		msgbox.exec();
		return;
	}

	// check for invalid characters
	if (pwd.text().contains(QRegularExpression("[[\\]&.,#]"))) {
		msgbox.setText(tr("Password contains invalid characters"));
		msgbox.exec();
		return;
	}

	QString cmd;

	connect(link, SIGNAL(success()), link, SLOT(unlockUser()));
	connect(link, SIGNAL(error()), link, SLOT(unlockUser()));

	cmd = "user[" + link->getEmployee()->getID() + "].pwdHash=" + QCryptographicHash::hash(pwd.text().toUtf8(), QCryptographicHash::Md5).toHex() + '\n';;
	link->tx(cmd.toUtf8());

	hide();
	window->setEnabled(1);
}

void pwdChangeDialog::reset(void)
{
	QString cmd;

	connect(link, SIGNAL(success()), link, SLOT(unlockUser()));
	connect(link, SIGNAL(error()), link, SLOT(unlockUser()));

	cmd = "user[" + link->getEmployee()->getID() + "].pwdHash=" + QCryptographicHash::hash(link->getEmployee()->getID().toUtf8(), QCryptographicHash::Md5).toHex() + '\n';;
	link->tx(cmd.toUtf8());

	hide();
	window->setEnabled(1);
}
