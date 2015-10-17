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
   File name    : addDeposit.h
   Created      : 05 Oct 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Dialog for the admin to add desposits (user earned vacation days)
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef ADDDEPOSIT_H_
#define ADDDEPOSIT_H_

#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QMessageBox>
#include "mainWindow.h"
#include "link.h"

class addDepositDialog : public QWidget
{
	Q_OBJECT

public:
	addDepositDialog(Link *lk, mainWindow *win);

public slots:
	void prep(void);

private:
	void closeEvent(QCloseEvent *event);
	QLineEdit amountEntry, yearEntry;
	QMessageBox msgbox;
	mainWindow *window;
	Link *link;

private slots:
	void ready(void);
	void cancel(void);
	void ok(void);

	
};

#endif /* ADDDEPOSIT_H_ */
