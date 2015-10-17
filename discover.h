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
   File name    : discovery.h
   Created      : 14 July 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Dialog to enter user name and password + zeroconf server discovery
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef DISCOVER_H_
#define DISCOVER_H_

#include <QLabel>
#include <QWidget>
#include <QCloseEvent>
#include <QLineEdit>
#include <QMessageBox>
#include <QTimer>
#include <QHostAddress>
#include "mainWindow.h"
#include "link.h"
#include "qtzeroconf/qzeroconf.h"

class mainWindow;

class Discover : public QWidget
{
	Q_OBJECT

public:
	Discover(Link *lk, mainWindow *win);
	void start(void);

private:
	void keyPressEvent(QKeyEvent *e);
	void closeEvent(QCloseEvent *event);
	QLineEdit userID, pwd;
	QMessageBox msgbox;
	mainWindow *window;
	Link *link;
	QZeroConf discover;
	QZeroConfItem *host;
	QTimer timeout;


signals:

private slots:
	void discoveryTimedOut(void);
	void error(QZeroConf::error_t);
	void discovery(QZeroConfItem *item);
	void startLogin(void);
	void ok(void);
	void cancel(void);
	void loginFailed(void);
	void loginSucceeded(void);
	
protected:


};

#endif /* DISCOVER_H_ */
