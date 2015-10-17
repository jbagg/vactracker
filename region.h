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
   File name    : region.h
   Created      : 07 July 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Region class holds a QMap of regionYears...each regionYears has a list of statutory days
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef REGION_H_
#define REGION_H_

#include <QObject>
#include <QMap>
#include "regionYear.h"
#include "regionparser.h"

class GeoRegion : public QObject
{
	Q_OBJECT

friend RegionParser;

public:
	GeoRegion(QString value) {name = value;}
	inline quint32 getVersion(void) {return version;}
	inline bool hasRegionYear(QString id) {return regionYears.contains(id);}
	inline RegionYear *getRegionYear(QString id) {return regionYears[id];}
	inline QMap<QString, RegionYear*>::iterator firstRegionYear(void) {return regionYears.begin();}
	inline QMap<QString, RegionYear*>::iterator endRegionYear(void) {return regionYears.end();}
	inline QString getLastRegionYearKey(void) {return regionYears.lastKey();}
	inline void addRegionYear(QString id)
	{
		regionYears[id] = new RegionYear(id.toInt());
	}

signals:
	void replicate(void);

private:
	QString name;
	quint32 version;
	QMap <QString, RegionYear *> regionYears;
};


#endif /* REGION_H_ */
