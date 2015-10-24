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
   File name    : vacEdit.cpp
   Created      : 15 June 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Dialog to make / edit vacation requests
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGridLayout>
#include "vacEdit.h"

#define	START_UNSET		1
#define	END_UNSET		4642999


vacEditDialog::vacEditDialog(Link *lk, mainWindow *win)
{
	window = win;
	link = lk;

	QHBoxLayout *mainLayout = new QHBoxLayout;
	QVBoxLayout *leftLayout = new QVBoxLayout;
	QGridLayout *rightLayout = new QGridLayout;
	QPushButton *unsetStartButton = new QPushButton;
	QPushButton *unsetEndButton = new QPushButton;
	QPushButton *okButton = new QPushButton;
	QPushButton *cancelButton = new QPushButton;

	unsetStartButton->setText(tr("Unset"));
	unsetEndButton->setText(tr("Unset"));
	okButton->setText(tr("Ok"));
	//okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	cancelButton->setText(tr("Cancel"));
	halfDay.setText(tr("Half Day"));
	weekendCkb.setText(tr("Include Weekends"));

	leftLayout->addWidget(&calendar);
	rightLayout->addWidget(&start, 0, 0);
	rightLayout->addWidget(unsetStartButton, 0, 1);
	rightLayout->addWidget(&end, 1, 0);
	rightLayout->addWidget(unsetEndButton, 1, 1);
	rightLayout->addWidget(&lengthLabel, 2, 0);
	rightLayout->addWidget(&halfDay, 3, 0);
	rightLayout->addWidget(&weekendCkb, 4, 0);
	rightLayout->setRowStretch(5, 1);
	rightLayout->addWidget(okButton, 6, 0, Qt::AlignRight);
	rightLayout->addWidget(cancelButton, 6, 1);

	mainLayout->addLayout(leftLayout);
	mainLayout->addLayout(rightLayout);
	setLayout(mainLayout);

	connect(&calendar, SIGNAL(clicked(QDate)), this, SLOT(update(QDate)));
	connect(unsetStartButton, SIGNAL(clicked()), this, SLOT(unsetStart()));
	connect(unsetEndButton, SIGNAL(clicked()), this, SLOT(unsetEnd()));
	connect(&halfDay, SIGNAL(stateChanged(int)), this, SLOT(halfDayChanged(int)));
	connect(&weekendCkb, SIGNAL(stateChanged(int)), this, SLOT(weekendChanged(int)));
	connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
}

void vacEditDialog::setNew(qint32 index)
{
	edit = 0;
	id = index;
	startDay = START_UNSET;
	endDay = END_UNSET;
	refreshStartLabel();
	refreshEndLabel();

	halfDay.setChecked(0);
	halfDay.setEnabled(0);
}

void vacEditDialog::setEdit(Withdrawal *withdrawal, qint32 index)
{
	edit = 1;
	id = index;
	startDay = withdrawal->getStart();
	endDay = withdrawal->getEnd();
	refreshStartLabel();
	refreshEndLabel();

	if (withdrawal->getLength() == calcLength(0))	// calc length with weekends disabled
		weekendCkb.setChecked(0);
	else
		weekendCkb.setChecked(1);
	length = withdrawal->getLength();

	if (startDay == endDay && startDay > 1)
		halfDay.setEnabled(1);
	else
		halfDay.setEnabled(0);
	if (length == 0.5)
		halfDay.setChecked(1);
	else
		halfDay.setChecked(0);
	refreshLengthLabel();
}

void vacEditDialog::update(QDate newDay)
{
	qint32 day = newDay.toJulianDay();

	if (startDay == START_UNSET) {
		startDay = day;
		refreshStartLabel();
		refreshEndLabel();
	}
	else if (endDay == END_UNSET) {
		endDay = day;
		refreshEndLabel();
	}
	length = calcLength(weekendCkb.isChecked());
	refreshLengthLabel();

	if (startDay == endDay && startDay > 1)
		halfDay.setEnabled(1);
	else
		halfDay.setEnabled(0);
}

quint32 vacEditDialog::calcLength(bool wknd)
{
	qint32 i, len;
	QDate test;

	if (startDay == START_UNSET || endDay == END_UNSET)
		return 0;

	if (wknd)
		len = endDay - startDay + 1;
	else {
		len = 0;
		for (i=startDay; i<endDay+1; i++) {
			test = QDate::fromJulianDay(i);
			if (test.dayOfWeek() != 6 && test.dayOfWeek() != 7)
				len++;
		}
	}

	// build a list of stats from the year(s) the vacation spans
	GeoRegion *region;
	region = link->getRegion(link->getEmployee()->getRegion());

	QDate start = QDate::fromJulianDay(startDay);
	QDate end = QDate::fromJulianDay(endDay);
	QList <qint32> stats;

	for (i=start.year(); i<=end.year(); i++) {
		if (!region->hasRegionYear(QString::number(i)))
			continue;
		stats << region->getRegionYear(QString::number(i))->getStats();
	}

	// subtract a day for any stats that lie in between the start and end
	for (i=0; i<stats.size(); i++) {
		if (stats[i] >= startDay && stats[i] <= endDay)
			len--;
	}

	return len;
}

void vacEditDialog::refreshStartLabel(void)
{
	QDate date;

	date = QDate::fromJulianDay(startDay);
	calendar.setMinimumDate(date);
	if (startDay == START_UNSET)
		start.setText(tr("Start") + ": " + tr("Select start date"));
	else
		start.setText(tr("Start") + ": " + mainWindow::getDayMonthStringfromDate(date));
}

void vacEditDialog::refreshEndLabel(void)
{
	QDate date;

#if QT_VERSION >= 0x050200
	if (endDay == END_UNSET) {
		GeoRegion *region;
		region = link->getRegion(link->getEmployee()->getRegion());
		date.setDate(region->getLastRegionYearKey().toInt(), 12, 31);
	}
	else
#endif
		date = QDate::fromJulianDay(endDay);

	calendar.setMaximumDate(date);
	if (endDay == END_UNSET && startDay == START_UNSET)
		end.setText(tr("End") + ":                         ");
	else if (endDay == END_UNSET)
		end.setText(tr("End") + ": " + tr("Select end date"));
	else
		end.setText(tr("End") + ": " + mainWindow::getDayMonthStringfromDate(date));
}

void vacEditDialog::refreshLengthLabel(void)
{
	if (length != 0)
		lengthLabel.setText(tr("Length") + ": " + QString::number(length));
	else
		lengthLabel.setText(tr("Length") + ": ?");
}

void vacEditDialog::unsetStart(void)
{
	startDay = START_UNSET;
	length = 0;
	refreshStartLabel();
	refreshEndLabel();
	refreshLengthLabel();
	halfDay.setChecked(0);
	halfDay.setEnabled(0);
}

void vacEditDialog::unsetEnd(void)
{
	endDay = END_UNSET;
	length = 0;
	refreshEndLabel();
	refreshLengthLabel();
	halfDay.setChecked(0);
	halfDay.setEnabled(0);
}

void vacEditDialog::halfDayChanged(int state)
{
	if (length != 1 && length != 0.5)
		return;

	if (state)
		length = 0.5;
	else
		length = 1;

	refreshLengthLabel();
}
void vacEditDialog::weekendChanged(int state)
{
	if (halfDay.checkState() == Qt::Checked)		// if set to half day do nothing
		return;

	if (state)
		length = calcLength(1);
	else
		length = calcLength(0);

	refreshLengthLabel();
}

void vacEditDialog::cancel(void)
{
	link->unlockUser();
	hide();
	window->setEnabled(1);
}

void vacEditDialog::closeEvent(QCloseEvent *event)
{
	event->accept();
	cancel();
}

void vacEditDialog::ok(void)
{
	QString cmd;

	connect(link, SIGNAL(success()), link, SLOT(unlockUser()));
	connect(link, SIGNAL(error()), link, SLOT(unlockUser()));

	cmd = "user[" + link->getEmployee()->getID() + "].withdrawal[" + QString::number(id) + "].start=" + QString::number(startDay);
	cmd+= "&end=" + QString::number(endDay);
	cmd+= "&length=" + QString::number(length).replace('.', '#');
	if (!edit)
		cmd+= "&request_date=" + QString::number(QDate::currentDate().toJulianDay());
	cmd+= "&state=request\n";

	link->tx(cmd.toUtf8());

	hide();
	window->setEnabled(1);
}
