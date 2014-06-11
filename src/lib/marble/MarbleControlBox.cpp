/* This file is part of the KDE project
 *
 * Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>
 * Copyright 2007      Inge Wallin   <ingwa@kde.org>
 * Copyright 2007      Thomas Zander <zander@kde.org>
 * Copyright 2010      Bastian Holst <bastianholst@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// Self
#include "MarbleControlBox.h"

// Tabs
#include "CurrentLocationWidget.h"
#include "NavigationWidget.h"
#include "FileViewWidget.h"
#include "LegendWidget.h"
#include "MapViewWidget.h"
#ifndef SUBSURFACE
#include "RoutingWidget.h"
#endif

// Marble
#include "MapThemeManager.h"
#include "MarbleGlobal.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "MarbleDebug.h"


namespace Marble
{

class MarbleControlBoxPrivate
{
 public:
    MarbleControlBoxPrivate();

    MapThemeManager m_mapThemeManager;

    MarbleWidget  *m_widget;

    NavigationWidget            *m_navigationWidget;
    LegendWidget                *m_legendWidget;
    MapViewWidget               *m_mapViewWidget;

    CurrentLocationWidget       *m_currentLocationWidget;

    FileViewWidget              *m_fileViewWidget;

#ifndef SUBSURFACE
    RoutingWidget  *m_routingWidget;
#endif
};

MarbleControlBoxPrivate::MarbleControlBoxPrivate()
    : m_widget( 0 ),
      m_navigationWidget( 0 ),
      m_legendWidget( 0 ),
      m_mapViewWidget( 0 ),
      m_currentLocationWidget( 0 ),
      m_fileViewWidget( 0 )
#ifndef SUBSURFACE
	,
      m_routingWidget( 0 )
#endif
{
}

// ================================================================


MarbleControlBox::MarbleControlBox(QWidget *parent)
    : QToolBox( parent ),
      d( new MarbleControlBoxPrivate )
{
    d->m_widget = 0;

    setFocusPolicy( Qt::NoFocus );
//    setFocusProxy( d->uiWidget.searchLineEdit );

    //  Iterate through all of the Side Widget values  //
    d->m_navigationWidget = new NavigationWidget( this );
    addItem( d->m_navigationWidget, d->m_navigationWidget->windowTitle() );

    d->m_legendWidget = new LegendWidget( this );
    addItem( d->m_legendWidget, d->m_legendWidget->windowTitle() );

    d->m_mapViewWidget = new MapViewWidget( this );
    addItem( d->m_mapViewWidget, d->m_mapViewWidget->windowTitle() );

    d->m_fileViewWidget = new FileViewWidget( this );
    addItem( d->m_fileViewWidget, d->m_fileViewWidget->windowTitle() );

    d->m_currentLocationWidget = new CurrentLocationWidget( this );

    addItem( d->m_currentLocationWidget, d->m_currentLocationWidget->windowTitle() );

    setCurrentIndex(0);

    //default
    setCurrentLocationTabShown( true );
    setFileViewTabShown( false );
    
    connect( d->m_mapViewWidget, SIGNAL(showMapWizard()), this, SIGNAL(showMapWizard()) );
    connect( d->m_mapViewWidget, SIGNAL(showUploadDialog()), this, SIGNAL(showUploadDialog()) );
    connect( d->m_mapViewWidget, SIGNAL(celestialBodyChanged(QString)),
             d->m_navigationWidget, SLOT(clearSearch()) );
    connect( d->m_navigationWidget, SIGNAL(searchFinished()), this, SIGNAL(searchFinished()) );
}

MarbleControlBox::~MarbleControlBox()
{
    delete d;
}

void MarbleControlBox::setMarbleWidget(MarbleWidget *widget)
{
    d->m_widget = widget;

#ifndef SUBSURFACE
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( !smallScreen ) {
        d->m_routingWidget = new RoutingWidget( widget, this );
        addItem( d->m_routingWidget, tr( "Routing" ) );
    }
#endif

    d->m_fileViewWidget->setMarbleWidget( widget );
    d->m_legendWidget->setMarbleModel( widget->model() );
    d->m_navigationWidget->setMarbleWidget( widget );
    d->m_mapViewWidget->setMarbleWidget( widget, &d->m_mapThemeManager );
    d->m_currentLocationWidget->setMarbleWidget( widget );

    connect( d->m_legendWidget, SIGNAL(propertyValueChanged(QString,bool)),
             widget,            SLOT(setPropertyValue(QString,bool)) );

    connect( d->m_widget, SIGNAL(themeChanged(QString)),
             this,        SLOT(selectTheme(QString)) );
}

void MarbleControlBox::setWidgetTabShown( QWidget * widget,
                                          int insertIndex, bool show,
                                          QString &text )
{
    int index = indexOf( widget );

    if( show ) {
        if ( !(index >= 0) ){
            if ( insertIndex < count() ) {
                insertItem( insertIndex, widget, text );
            } else {
                insertItem( 3 ,widget, text );
            }
            widget->show();
        }
    } else {
        if ( index >= 0 ) {
            widget->hide();
            removeItem( index );
        }
    }
}

void MarbleControlBox::setNavigationTabShown( bool show )
{
    QString  title = tr( "Navigation" );
    setWidgetTabShown( d->m_navigationWidget, 0, show, title);
}

void MarbleControlBox::setLegendTabShown( bool show )
{
    QString  title = tr( "Legend" );
    setWidgetTabShown( d->m_legendWidget, 1, show, title );
}

void MarbleControlBox::setMapViewTabShown( bool show )
{
    QString  title = tr( "Map View" );
    setWidgetTabShown( d->m_mapViewWidget, 2, show, title );
}

void MarbleControlBox::setFileViewTabShown( bool show )
{
    QString  title = tr( "File View" );
    setWidgetTabShown( d->m_fileViewWidget, 3, show, title );
}

void MarbleControlBox::setCurrentLocationTabShown( bool show )
{
    QString  title = tr( "Current Location" );
    setWidgetTabShown( d->m_currentLocationWidget, 4, show, title );
    if ( d->m_widget && d->m_widget->mapTheme() ) {
        bool enabled = d->m_widget->mapTheme()->head()->target() == "earth";
        int locationIndex = indexOf( d->m_currentLocationWidget );
        if ( locationIndex >= 0 ) {
            setItemEnabled( locationIndex, enabled );
        }
    }
}


void MarbleControlBox::setRoutingTabShown( bool show )
{
#ifndef SUBSURFACE
    if ( d->m_routingWidget ) {
        QString  title = tr( "Routing" );
        setWidgetTabShown( d->m_routingWidget, 5, show, title );
    }
#endif
}

void MarbleControlBox::selectTheme( const QString &theme )
{
    Q_UNUSED( theme )

    if ( !d->m_widget )
        return;

    QString selectedId = d->m_widget->mapTheme()->head()->target();
#ifndef SUBSURFACE
    if ( d->m_routingWidget ) {
        int routingIndex = indexOf( d->m_routingWidget );
        setItemEnabled( routingIndex, selectedId == "earth" );
    }
#endif
    int locationIndex = indexOf( d->m_currentLocationWidget );
    if ( locationIndex >= 0 ) {
        setItemEnabled( locationIndex, selectedId == "earth" );
    }
}

void MarbleControlBox::setWorkOffline(bool offline)
{
    d->m_widget->model()->setWorkOffline( offline );
    if ( !offline ) {
        d->m_widget->clearVolatileTileCache();
    }
}

CurrentLocationWidget * MarbleControlBox::currentLocationWidget()
{
    return d->m_currentLocationWidget;
}

void MarbleControlBox::search(const QString &searchTerm, SearchMode searchMode )
{
    setCurrentWidget( d->m_navigationWidget );
    d->m_navigationWidget->search( searchTerm, searchMode );
}

}

#include "MarbleControlBox.moc"
