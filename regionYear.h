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
   File name    : regionYear.h
   Created      : 07 July 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   regionYears holds a list of statutory days
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef REGIONYEAR_H_
#define REGIONYEAR_H_

#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class RegionYear : public QObject
{
	Q_OBJECT

public:
	RegionYear(qint32 value) {year = value;}
	inline qint32 getYear(void) {return year;}
	inline bool hasStat(qint32 day) {return stats.contains(day);}
	inline QList <qint32> getStats(void) {return stats;}
	inline qint32 getStatsSize(void) {return stats.size();}
	inline qint32 getStatAt(qint32 i) {return stats[i];}
	inline void addStat(qint32 i) {stats.append(i);}
	inline void removeStatAt(qint32 i) {stats.removeAt(i);}
	inline void clearStats(void) {stats.clear();}

private:
	QString fileName;
	qint32 year;
	QList <qint32> stats;

private slots:


};


#endif /* REGIONYEAR_H_ */
