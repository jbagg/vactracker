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
   File name    : pwdChange.h
   Created      : 14 July 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Dialog to edit user password
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef PWDCHANGE_H_
#define PWDCHANGE_H_

#include <QLabel>
#include <QWidget>
#include <QCloseEvent>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include "mainWindow.h"
#include "link.h"

class mainWindow;

class pwdChangeDialog : public QWidget
{
	Q_OBJECT

public:
	pwdChangeDialog(Link *lk, mainWindow *win);

public slots:
	void open(void);

signals:

private:
	void closeEvent(QCloseEvent *event);
	QLineEdit pwd, pwdConfirm;
	QMessageBox msgbox;
	QPushButton resetButton;
	mainWindow *window;
	Link *link;

private slots:
	void locked(void);
	void ok(void);
	void cancel(void);
	void reset(void);
	
protected:


};

#endif /* PWDCHANGE_H_ */
