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
   File name    : addDeposit.cpp
   Created      : 05 Oct 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Dialog for the admin to add desposits (user earned vacation days)
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QDesktopWidget>
#include <QPushButton>
#include <QGridLayout>
#include "addDeposit.h"

#if QT_VERSION < 0x050000
	#define	QRegularExpression	QRegExp
#endif

addDepositDialog::addDepositDialog(Link *lk, mainWindow *win)
{
	QLabel *label;

	window = win;
	link = lk;

	QGridLayout *mainLayout = new QGridLayout;

	label = new QLabel(tr("Year") + ":");
	mainLayout->addWidget(label, 0, 0, Qt::AlignRight);
	mainLayout->addWidget(&yearEntry, 0, 1, Qt::AlignLeft);

	label = new QLabel(tr("Amount") + ":");
	mainLayout->addWidget(label, 1, 0, Qt::AlignRight);
	mainLayout->addWidget(&amountEntry, 1, 1, Qt::AlignLeft);

	QPushButton *exitButton = new QPushButton;
	exitButton->setText(tr("Cancel"));
	mainLayout->addWidget(exitButton, 2, 0, Qt::AlignLeft);
	connect(exitButton, SIGNAL(clicked()), this, SLOT(cancel()));

	QPushButton *okButton = new QPushButton;
	okButton->setText(tr("Ok"));
	mainLayout->addWidget(okButton, 2, 1, Qt::AlignRight);
	connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));

	setLayout(mainLayout);
}

void addDepositDialog::prep(void)
{
	amountEntry.clear();
	yearEntry.clear();

	connect(link, SIGNAL(success()), this, SLOT(ready()));
	link->lockUser();
}

void addDepositDialog::ready(void)
{
	disconnect(link, SIGNAL(success()), 0, 0);
	window->setEnabled(0);
	show();
	QDesktopWidget screen;
	move((screen.screenGeometry().width() - size().width())/2, (screen.screenGeometry().height() - size().height())/2);
}

void addDepositDialog::cancel(void)
{
	link->unlockUser();
	hide();
	window->setEnabled(1);
}

void addDepositDialog::closeEvent(QCloseEvent *event)
{
	event->accept();
	cancel();
}

void addDepositDialog::ok(void)
{
	QString cmd;

	// check for blank entries
	if (!yearEntry.text().size()) {
		msgbox.setText(tr("Year is blank"));
		msgbox.exec();
		return;
	}
	if (!amountEntry.text().size()) {
		msgbox.setText(tr("Amount is blank"));
		msgbox.exec();
		return;
	}

	// check entries are numeric
	if (yearEntry.text().contains(QRegularExpression("[a-zA-Z[\\]&.,#]"))) {
		msgbox.setText(tr("Year can only contain numbers"));
		msgbox.exec();
		return;
	}
	if (amountEntry.text().contains(QRegularExpression("[a-zA-Z[\\]&,#]"))) {
		msgbox.setText(tr("Amount can only contain numbers"));
		msgbox.exec();
		return;
	}

	connect(link, SIGNAL(success()), link, SLOT(unlockUser()));
	connect(link, SIGNAL(error()), link, SLOT(unlockUser()));

	cmd = "user[" + link->getEmployee()->getID() + "].deposit[" + QString::number(link->getEmployee()->depositsSize()) + "].year=" + yearEntry.text();
	cmd+= "&amount=" + amountEntry.text().replace('.', '#') + '\n';
	link->tx(cmd.toUtf8());

	hide();
	window->setEnabled(1);
}
