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
   File name    : discovery.cpp
   Created      : 14 July 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Dialog to enter user name and password + zeroconf server discovery
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QHBoxLayout>
#include <QCryptographicHash>
#include <QDir>
#include <QDesktopWidget>
#include "discover.h"

#if QT_VERSION < 0x050000
	#define	QRegularExpression	QRegExp
#endif

Discover::Discover(Link *lk, mainWindow *win)
{
	QLabel *label;
	quint32 i = 0;

	window = win;
	link = lk;
	host = NULL;

	QGridLayout *mainLayout = new QGridLayout;

	label = new QLabel(tr("User Name") + ':');
	mainLayout->addWidget(label, i, 0, Qt::AlignRight);
	mainLayout->addWidget(&userID, i++, 1, Qt::AlignLeft);
	userID.setText(QDir::home().dirName().section('.', 0, 0));

	label = new QLabel(tr("Password") + ':');
	mainLayout->addWidget(label, i, 0, Qt::AlignRight);
	mainLayout->addWidget(&pwd, i++, 1, Qt::AlignLeft);
	pwd.setEchoMode(QLineEdit::Password);

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

	connect(&discover, SIGNAL(itemAdded(QZeroConfItem *)), this, SLOT(discovery(QZeroConfItem *)));
	connect(&discover, SIGNAL(error(QZeroConf::error_t)), this, SLOT(error(QZeroConf::error_t)));
	connect(&timeout, SIGNAL(timeout()), this, SLOT(discoveryTimedOut()));
}

void Discover::start(void)
{
	timeout.start(2500);
	discover.startBrowser("_vactracker._tcp");
}

void Discover::keyPressEvent(QKeyEvent *e)
{
	switch(e->key()) {
		case Qt::Key_Escape: close(); break;
		case Qt::Key_Enter: ok(); break;
		default: QWidget::keyPressEvent(e);
	}
}

void Discover::discoveryTimedOut(void)
{
	timeout.stop();
	if (host && (host->ip.size() || host->ipv6.size()))
		startLogin();
	else
		error(QZeroConf::browserFailed);
}

void Discover::error(QZeroConf::error_t)
{
	msgbox.setText(tr("Unable to find server - exiting"));
	msgbox.exec();
	exit(1);
}

void Discover::discovery(QZeroConfItem *item)
{
	if (!host)
		host = item;
	if (item->ip.size() && item->ipv6.size())
		startLogin();
}

void Discover::startLogin(void)
{
	timeout.stop();
	if (host->ipv6.size() && link->srvConnect(host->ipv6, host->port)) {
		qDebug("IPv6 Connection");
	}
	else if (host->ip.size() && link->srvConnect(host->ip, host->port)) {
		qDebug("IPv4 Connection");
	}
	else {
		msgbox.setText(tr("TCP connection to server failed - exiting"));
		msgbox.exec();
		exit(1);
	}
	disconnect(&discover, SIGNAL(itemAdded(QZeroConfItem *)), 0, 0);
	discover.stopBrowser();
	pwd.setFocus();
	show();
	QDesktopWidget screen;
	move((screen.screenGeometry().width() - size().width())/2, (screen.screenGeometry().height() - size().height())/2);
}

void Discover::cancel(void)
{
	exit(1);
}

void Discover::closeEvent(QCloseEvent *event)
{
	event->accept();
	cancel();
	exit(1);
}

void Discover::ok(void)
{
	// check for blank entries
	if (!userID.text().size()) {
		msgbox.setText(tr("User ID is blank"));
		msgbox.exec();
		return;
	}
	if (!pwd.text().size()) {
		msgbox.setText(tr("Password is blank"));
		msgbox.exec();
		return;
	}

	// check for invalid characters
	if (userID.text().contains(QRegularExpression("[[\\]&.,# -/\\\\]"))) {
		//[a-zA-Z0-9]   [[:alphanum:]]  [[:xdigit:]]""
		msgbox.setText(tr("User ID contains invalid characters"));
		msgbox.exec();
		return;
	}
	if (pwd.text().contains(QRegularExpression("[[\\]&.,#]"))) {
		msgbox.setText(tr("Password contains invalid characters"));
		msgbox.exec();
		return;
	}

    link->setEmployeeIdShowing(userID.text());
	connect(link, SIGNAL(success()), this, SLOT(loginSucceeded()));
	connect(link, SIGNAL(error()), this, SLOT(loginFailed()));

	QString cmd;
	cmd = "user[" + userID.text() + "].login=" + QCryptographicHash::hash(pwd.text().toUtf8(), QCryptographicHash::Md5).toHex() + "\n";
    link->tx(cmd);
}

void Discover::loginFailed(void)
{
	disconnect(link, SIGNAL(success()), 0, 0);
	disconnect(link, SIGNAL(error()), 0, 0);
	QMessageBox msgbox;
	msgbox.setIcon(QMessageBox::Critical);
	msgbox.setText(tr("User name or password incorrect"));
	msgbox.setStandardButtons(QMessageBox::Ok);
	msgbox.exec();
}

void Discover::loginSucceeded(void)
{
	disconnect(link, SIGNAL(success()), 0, 0);
	disconnect(link, SIGNAL(error()), 0, 0);
	window->show();
	hide();
	//void mainWindow::rqstUserRefresh(void)
	link->tx("user[" + link->getEmployeeIdShowing() + "].refresh?\n");
}
