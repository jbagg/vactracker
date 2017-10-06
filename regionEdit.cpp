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
   File name    : regionEdit.cpp
   Created      : 18 Aug 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Dialog to add / edit Regions and Region Stat Holidays
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QDesktopWidget>
#include "regionEdit.h"

#if QT_VERSION < 0x050000
	#define	QRegularExpression	QRegExp
#endif

class QListStatItem : public QListWidgetItem
{
public:

	QListStatItem(QString name, quint32 day, QListWidget *item) : QListWidgetItem(name, item)
	{
		stat = day;
	}
	quint32 stat;
};

regionEditDialog::regionEditDialog(Link *lk, mainWindow *win)
{
	QLabel *label;

	window = win;
	link = lk;
	upload = 0;

	QHBoxLayout *mainLayout = new QHBoxLayout;
	QVBoxLayout *regionLayout = new QVBoxLayout;
	QVBoxLayout *yearLayout = new QVBoxLayout;
	QVBoxLayout *statsLayout = new QVBoxLayout;
	QVBoxLayout *calendarLayout = new QVBoxLayout;

	label = new QLabel(tr("Select Region"));
	regionLayout->addWidget(label);
	regionLayout->addWidget(&regionSelector);
	regionSelector.setSizeAdjustPolicy(QComboBox::AdjustToContents);

	label = new QLabel(tr("New Region"));
	regionLayout->addWidget(label);
	regionLayout->addWidget(&regionEntry);

	addRegionButton.setText(tr("Add"));
	regionLayout->addWidget(&addRegionButton);
	connect(&addRegionButton, SIGNAL(clicked()), this, SLOT(addRegion()));

	regionLayout->addStretch();

	QPushButton *exitButton = new QPushButton;
	exitButton->setText(tr("Done"));
	regionLayout->addWidget(exitButton, Qt::AlignRight);
	connect(exitButton, SIGNAL(clicked()), this, SLOT(cancel()));

	label = new QLabel(tr("Select Year"));
	yearLayout->addWidget(label);
	yearLayout->addWidget(&yearSelector);
	connect(&yearSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(yearChanged(int)));

	label = new QLabel(tr("New Year"));
	yearLayout->addWidget(label);
	yearLayout->addWidget(&yearEntry);

	addYearButton.setText(tr("Add"));
	yearLayout->addWidget(&addYearButton);
	connect(&addYearButton, SIGNAL(clicked()), this, SLOT(addYear()));

	yearLayout->addStretch();

	label = new QLabel(tr("Statutory days"));
	statsLayout->addWidget(label);
	regionSelector.setSizeAdjustPolicy(QComboBox::AdjustToContents);
	statsLayout->addWidget(&stats);

	QPushButton *removeButton = new QPushButton;
	removeButton->setText(tr("Remove"));
	statsLayout->addWidget(removeButton);
	connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));

	calendarLayout->addWidget(&calendar);

	QPushButton *addButton = new QPushButton;
	addButton->setText(tr("Add"));
	calendarLayout->addWidget(addButton);
	connect(addButton, SIGNAL(clicked()), this, SLOT(add()));

	QFrame* line = new QFrame();
	line->setFrameShape(QFrame::VLine);
	line->setFrameShadow(QFrame::Sunken);

	mainLayout->addLayout(regionLayout);
	mainLayout->addWidget(line);
	mainLayout->addLayout(yearLayout);
	mainLayout->addLayout(statsLayout);
	mainLayout->addLayout(calendarLayout);
	setLayout(mainLayout);

	// Error Message Box setup
	msgbox.setIcon(QMessageBox::Critical);
	msgbox.setStandardButtons(QMessageBox::Ok);
}

void regionEditDialog::prep(void)
{
	regionEntry.setEnabled(1);
	addRegionButton.setEnabled(1);
	yearEntry.setEnabled(0);
	addYearButton.setEnabled(0);

	regionIndex = -1;
	disconnect(link, SIGNAL(success()), 0, 0);
	connect(link, SIGNAL(success()), this, SLOT(listReady()));
	link->tx("regionList?\n");
}

void regionEditDialog::listReady(void)
{
	disconnect(link, SIGNAL(success()), 0, 0);

	regionSelector.clear();
	regionSelector.addItem("");
	regionSelector.addItems(link->getRegionList());
	regionSelector.setCurrentIndex(0);
	connect(&regionSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(changeRegion(int)));

	window->setEnabled(0);
	show();
	QDesktopWidget screen;
	move((screen.screenGeometry().width() - size().width())/2, (screen.screenGeometry().height() - size().height())/2);
}

void regionEditDialog::changeRegion(int i)
{
	if (uploadChanges()) {
		// need to wait for the ok / error signal from uploadChanges() before doing a refresh or get lock
		nextRegionIndex = i - 1;
		disconnect(link, SIGNAL(completed()), 0, 0);
		connect(link, SIGNAL(completed()), this, SLOT(doRegionChange()));
	}
	else {
		nextRegionIndex = i - 1;
		doRegionChange();
	}
}

void regionEditDialog::doRegionChange(void)
{

	if (nextRegionIndex < 0) {
		regionEntry.setEnabled(1);
		addRegionButton.setEnabled(1);
		yearEntry.setEnabled(0);
		addYearButton.setEnabled(0);
	}
	else {
		regionEntry.setEnabled(0);
		addRegionButton.setEnabled(0);
		yearEntry.setEnabled(1);
		addYearButton.setEnabled(1);
	}
	if (nextRegionIndex != regionIndex) {
		yearSelector.clear();
		yearEntry.clear();
		stats.clear();
		if (regionIndex >= 0) {
			if (nextRegionIndex >= 0)
				connect(link, SIGNAL(completed()), this, SLOT(refresh()));		// valid -> valid
			if (regionIndex < link->regionListCount())
				link->tx("region[" + link->getRegionIdAt(regionIndex) + "].unlock\n");
			regionIndex = nextRegionIndex;
		}
		else if (nextRegionIndex >= 0) {	// (-1) -> valid
			regionIndex = nextRegionIndex;
			refresh();
		}
	}
}

void regionEditDialog::refresh(void)
{
	disconnect(link, SIGNAL(completed()), 0, 0);
	connect(link, SIGNAL(completed()), this, SLOT(getLock()));
	if (regionIndex < link->regionListCount())
		link->tx("region[" + link->getRegionIdAt(regionIndex) + "].refresh?\n");
}

void regionEditDialog::getLock(void)
{
	GeoRegion *region;

	disconnect(link, SIGNAL(completed()), 0, 0);
	if (link->lastSucceeded()) {
		connect(link, SIGNAL(completed()), this, SLOT(updateYears()));
		region = link->getRegionAt(regionIndex);
		if (!region)
			return;
		link->tx("region[" + link->getRegionIdAt(regionIndex) + "].lock=" + QString::number(region->getVersion()) + "\n");
	}
}

void regionEditDialog::updateYears(void)
{
	qint32 newest, n, i;
	GeoRegion *region;
	QMap <QString, RegionYear *>::iterator rY;

	disconnect(link, SIGNAL(completed()), 0, 0);

	n = -1;
	newest = i = 0;
	region = link->getRegionAt(regionIndex);
	if (!region)
		return;
	for (rY = region->firstRegionYear(); rY != region->endRegionYear(); rY++) {
		if (!(*rY)) {	// fixme strange bug - Select Ontario, blank, then Ontario - extra NULL year appears?
			qWarning("rY is NULL");
			continue;
		}
		yearSelector.addItem(QString::number((*rY)->getYear()));

		// mark highest year
		if ((*rY)->getYear() > newest) {
			n = i;
			newest = (*rY)->getYear();
		}
		i++;
	}

	// set to current year
	if (n >= 0) {
		yearSelector.setCurrentIndex(n);
		yearEntry.setText(QString::number(newest+1));
		yearChanged(n);
	}
}

void regionEditDialog::yearChanged(int n)
{
	RegionYear *rY;
	qint32 i;

	uploadChanges();
	yearIndex = n;
	stats.clear();
	rY = lookupRegionYear(regionIndex, yearSelector.currentText());
	if (!rY)
		return;
	for (i=0; i<rY->getStatsSize(); i++)
		new QListStatItem(mainWindow::getDayMonthStringfromDate(QDate::fromJulianDay(rY->getStatAt(i))), rY->getStatAt(i), &stats);

	// adjust calendar when year is changed
	QDate oldDate, newDate;
	oldDate = calendar.selectedDate();
	newDate.setDate(rY->getYear(), oldDate.month(), oldDate.day());
	calendar.setSelectedDate(newDate);
}

RegionYear *regionEditDialog::lookupRegionYear(quint32 i, QString year)
{
	GeoRegion *region;

	region = link->getRegionAt(i);
	if (!region)
		return NULL;
	return region->getRegionYear(year);
}

void regionEditDialog::remove(void)
{
	qint32 row;
	RegionYear *rY;

	row = stats.currentRow();
	if (row < 0)
		return;
	upload = 1;
	//stats.removeItemWidget(stats.currentItem());		// this doesn't work?
	delete stats.takeItem(row);
	// remove local data structure as there is no refresh until region change
	rY = lookupRegionYear(regionIndex, yearSelector.currentText());
	if (rY)
		rY->removeStatAt(row);
}

void regionEditDialog::add(void)
{
	QDate dayToAdd;
	GeoRegion *region;
	RegionYear *rY;

	dayToAdd = calendar.selectedDate();


	if (regionIndex < 0 || regionIndex >= link->regionListCount())
		return;
	region = link->getRegionAt(regionIndex);
	if (!region)
		return;
	rY = region->getRegionYear(yearSelector.currentText());
	if (!rY)
		return;
	// check that date isn't already in list
	if (rY->hasStat(dayToAdd.toJulianDay()))
		return;

	upload = 1;
	new QListStatItem(mainWindow::getDayMonthStringfromDate(dayToAdd), dayToAdd.toJulianDay(), &stats);

	// add data structure locally as there is no refresh until region change
	rY->addStat(dayToAdd.toJulianDay());
}

bool regionEditDialog::uploadChanges(void)
{
	QListStatItem *statItem;
	QString cmd;
	qint32 i;

	if (!upload)
		return 0;
	upload = 0;

	if (regionIndex >= link->regionListCount())
		return 0;
	cmd = "region[" + link->getRegionIdAt(regionIndex) + "].year[" + yearSelector.itemText(yearIndex) + "].stat=";
	for (i=0; i<stats.count(); i++) {
		statItem = static_cast<QListStatItem*>(stats.item(i));
		cmd+= QString::number(statItem->stat) + ',';
	}
	if (stats.count())
		cmd.remove(cmd.size()-1, 1);  // remove last comma if > 0 items in stats
	cmd.append('\n');
	link->tx(cmd);
	return 1;
}

void regionEditDialog::addRegion(void)
{
	if (!regionEntry.text().size()) {
		msgbox.setText(tr("Region is blank"));
		msgbox.exec();
		return;
	}
	if (regionSelector.findText(regionEntry.text()) != -1) {
		msgbox.setText(tr("Region is already in the list"));
		msgbox.exec();
		return;
	}
	if (regionEntry.text().contains(QRegularExpression("[[\\]&.,# -/\\\\]"))) {
		//[a-zA-Z0-9]   [[:alphanum:]]  [[:xdigit:]]""
		msgbox.setText(tr("Region name can not contain spaces or the characters") + " & . , [ ] / \\ #");
		msgbox.exec();
		return;
	}

	link->appendToRegionList(regionEntry.text());
	link->tx("region[" + regionEntry.text()  + "].unlock\n");
	regionSelector.addItem(regionEntry.text());
	regionEntry.clear();
}

void regionEditDialog::addYear(void)
{
	GeoRegion *region;

	if (!yearEntry.text().size()) {
		msgbox.setText(tr("Year is blank"));
		msgbox.exec();
		return;
	}
	if (yearEntry.text().contains(QRegularExpression("[a-zA-Z[\\]&.,#]"))) {
		msgbox.setText(tr("Year can only contain numbers"));
		msgbox.exec();
		return;
	}
	if (yearSelector.findText(yearEntry.text()) != -1) {
		msgbox.setText(tr("Year is already in the list"));
		msgbox.exec();
		return;
	}

	region = link->getRegionAt(regionIndex);
	if (!region)
		return;
	region->addRegionYear(yearEntry.text());

	yearSelector.addItem(yearEntry.text());
	yearSelector.setCurrentIndex(yearSelector.count() - 1);
	yearEntry.setText(QString::number(yearEntry.text().toInt() + 1));
	// fixme - list can be made out of order.  Can we sort without too much trouble?
}

void regionEditDialog::cancel(void)
{
	uploadChanges();
	disconnect(link, SIGNAL(completed()), 0, 0);
	if (regionIndex >= 0 && regionIndex < link->regionListCount())
		link->tx("region[" + link->getRegionIdAt(regionIndex) + "].unlock\n");
	disconnect(&regionSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(changeRegion(int)));
	yearSelector.clear();
	stats.clear();
	hide();
	window->setEnabled(1);
}

void regionEditDialog::closeEvent(QCloseEvent *event)
{
	event->accept();
	cancel();
}
