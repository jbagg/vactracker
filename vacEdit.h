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
   File name    : vacEdit.h
   Created      : 15 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Dialog to make / edit vacation requests
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef VACEDIT_H_
#define VACEDIT_H_

#include <QLabel>
#include <QCalendarWidget>
#include <QCheckBox>
#include <QWidget>
#include <QCloseEvent>
#include "mainWindow.h"
#include "link.h"
#include "withdrawal.h"
#include "user.h"

class mainWindow;

class vacEditDialog : public QWidget
{
	Q_OBJECT

public:
	vacEditDialog(Link *lk, mainWindow *win);
	void setNew(qint32 index);
	void setEdit(Withdrawal *withdrawal, qint32 index);

private:
	void closeEvent(QCloseEvent *event);
	void refreshStartLabel(void);
	void refreshEndLabel(void);
	void refreshLengthLabel(void);
	quint32 calcLength(bool wknd);
	QLabel start, end, lengthLabel;
	QCalendarWidget calendar;
	QCheckBox halfDay, weekendCkb;
	qint32 startDay, endDay, id;
	qreal length;
	bool edit;
	mainWindow *window;
	Link *link;


signals:

private slots:
	void update(QDate day);
	void unsetStart(void);
	void unsetEnd(void);
	void halfDayChanged(int state);
	void weekendChanged(int state);
	void ok(void);
	void cancel(void);
	
protected:


};

#endif /* VACEDIT_H_ */
