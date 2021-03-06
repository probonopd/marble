//
// This file is part of the Marble Virtual Globe.
//
// Copyright 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library. If not, see <http://www.gnu.org/licenses/>.

#ifndef MARBLE_TILELOADER_H
#define MARBLE_TILELOADER_H

#include <QObject>
#include <QString>
#include <QImage>

#include "TileId.h"
#include "GeoDataContainer.h"
#include "PluginManager.h"
#include "MarbleGlobal.h"

class QByteArray;
class QImage;
class QUrl;

namespace Marble
{
class HttpDownloadManager;
class GeoDataDocument;
class GeoSceneTiled;
class GeoSceneTextureTile;
class GeoSceneVectorTile;

class TileLoader: public QObject
{
    Q_OBJECT

 public:
    enum TileStatus {
        Missing,
        Expired,
        Available
    };

    explicit TileLoader(HttpDownloadManager * const, const PluginManager * );

    QImage loadTileImage( GeoSceneTextureTile const *textureLayer, TileId const & tileId, DownloadUsage const );
    GeoDataDocument* loadTileVectorData( GeoSceneVectorTile const *textureLayer, TileId const & tileId, DownloadUsage const usage );
    void downloadTile( GeoSceneTiled const *textureLayer, TileId const &, DownloadUsage const );

    static int maximumTileLevel( GeoSceneTiled const & texture );

    /**
     * Returns whether the mandatory most basic tile level is fully available for
     * the given @p texture layer.
     */
    static bool baseTilesAvailable( GeoSceneTiled const & texture );

    /**
      * Returns the status of the downloaded tile file:
      * - Missing when it has not been downloaded
      * - Expired when it has been downloaded, but is too old (as per .dgml expiration time)
      * - Available when it has been downloaded and is not expired
      */
    static TileStatus tileStatus( GeoSceneTiled const *textureLayer, const TileId &tileId );

 public Q_SLOTS:
    void updateTile( QByteArray const & imageData, QString const & tileId );

 Q_SIGNALS:
    void downloadTile( QUrl const & sourceUrl, QString const & destinationFileName,
                       QString const & id, DownloadUsage );

    void tileCompleted( TileId const & tileId, QImage const & tileImage );

    void tileCompleted( TileId const & tileId, GeoDataDocument * document, QString const & format );

 private:
    static QString tileFileName( GeoSceneTiled const * textureLayer, TileId const & );
    void triggerDownload( GeoSceneTiled const *textureLayer, TileId const &, DownloadUsage const );
    static QImage scaledLowerLevelTile( GeoSceneTextureTile const * textureLayer, TileId const & );

    // For vectorTile parsing
    const PluginManager * m_pluginManager;
};

}

#endif
