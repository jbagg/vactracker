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
   File name    : userEdit.h
   Created      : 17 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Dialog to make / edit users (employees)
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef USEREDIT_H_
#define USEREDIT_H_

#include <QLabel>
#include <QWidget>
#include <QCloseEvent>
#include <QLineEdit>
#include <QMessageBox>
#include "mainWindow.h"
#include "link.h"
#include "user.h"

class mainWindow;

class userEditDialog : public QWidget
{
	Q_OBJECT

public:
	userEditDialog(Link *lk, mainWindow *win);
	void setNew(void);
	void setEdit(User *user);

private:
	void closeEvent(QCloseEvent *event);
	QLineEdit userID, firstName, lastName, email, allotment;
	QComboBox typeSelector, managerSelector, regionSelector;
	QMessageBox msgbox;
	mainWindow *window;
	Link *link;
	bool edit;


signals:

private slots:
	void ok(void);
	void cancel(void);
	
protected:


};

#endif /* USEREDIT_H_ */
