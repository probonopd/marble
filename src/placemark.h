#ifndef PLACEMARK_H
#define PLACEMARK_H

// #include "geopoint.h"
// #include <QDebug>
#include <QChar>
#include <QPixmap>
#include <QRect>
#include "geopoint.h"
#include "placemarkstorage.h"

class PlaceMark : public PlaceMarkStorage {


public:
	PlaceMark();
	PlaceMark( QString );

	void coordinate( float& lng, float& lat );
	void setCoordinate( float lng, float lat );

	const QPoint symbolPos(){ return m_sympos; }
	void setSymbolPos( QPoint sympos ){ m_sympos = sympos; }

	const QChar role() const { return m_role; }
	void setRole( QChar role ){ m_role = role; }

	const int symbol() const { return m_symbol; }
	void setSymbol( int symbol ){ m_symbol = symbol; }

	const int population() const { return m_population; }
	void setPopulation( int population ){ m_population = population; }

	const Quaternion& getQuatPoint() const { return m_coordinate.getQuatPoint(); }	

	const QPixmap textPixmap() const { return m_textPixmap; }
	const QRect textRect() const { return m_rect; }
	void setTextPixmap( QPixmap textPixmap ){ m_textPixmap = textPixmap; }
	void setTextRect( QRect textRect ){ m_rect = textRect;}
	void clearTextPixmap(){ if ( m_textPixmap.isNull() == false ) { m_textPixmap = QPixmap(); } }

private:
	GeoPoint m_coordinate;
	QPoint m_sympos;
	QPixmap m_textPixmap;
	QRect m_rect;
	int m_population;
	int m_symbol;
	QChar m_role;
};

#endif // PLACEMARK_H
