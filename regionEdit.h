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
   File name    : regionEdit.h
   Created      : 18 Aug 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Dialog to add / edit Regions and Region Stat Holidays
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef REGIONEDIT_H_
#define REGIONEDIT_H_

#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QListWidget>
#include <QCalendarWidget>
#include "mainWindow.h"
#include "link.h"

class regionEditDialog : public QWidget
{
	Q_OBJECT

public:
	regionEditDialog(Link *lk, mainWindow *win);

public slots:
	void prep(void);

private:
	bool uploadChanges(void);
	void closeEvent(QCloseEvent *event);
	RegionYear *lookupRegionYear(quint32 i, QString year);
	QComboBox regionSelector, yearSelector;
	QLineEdit regionEntry, yearEntry;
	QPushButton addRegionButton, addYearButton;
	QListWidget stats;
	QCalendarWidget calendar;
	QMessageBox msgbox;
	mainWindow *window;
	Link *link;
	qint32 regionIndex, nextRegionIndex, yearIndex;
	bool upload;

private slots:
	void listReady(void);
	void changeRegion(int i);
	void doRegionChange(void);
	void refresh(void);
	void getLock(void);
	void updateYears(void);
	void yearChanged(int i);
	void remove(void);
	void add(void);
	void addRegion(void);
	void addYear(void);
	void cancel(void);
	
};

#endif /* REGIONEDIT_H_ */
