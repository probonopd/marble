//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "GeoWriter.h"

#include "GeoTagWriter.h"
#include "KmlElementDictionary.h"

#include "MarbleDebug.h"

namespace Marble
{

GeoWriter::GeoWriter()
{
    //FIXME: work out a standard way to do this.
    m_documentType = kml::kmlTag_nameSpace22;
}

bool GeoWriter::write(QIODevice* device, const QList<GeoDataFeature> &features)
{
    setDevice( device );
    setAutoFormatting( true );
    writeStartDocument();

    //FIXME: write the starting tags. Possibly register a tag handler to do this
    // with a null string as the object name?
    GeoTagWriter::QualifiedName name( "", m_documentType );
    const GeoTagWriter* writer = GeoTagWriter::recognizes(name);
    if( writer ) {
        //FIXME is this too much of a hack?
        //geodataobject is never used in this context
        writer->write( GeoDataObject(), *this );
    } else {
        qDebug() << "There is no GeoWriter registered for: " << name;
        return false;
    }

    QListIterator<GeoDataFeature> it(features);

    while ( it.hasNext() ) {
        GeoDataFeature f = it.next();

        if( ! writeElement( f ) ) {
            return false;
        }
    }

    //close the document
    writeEndElement();
    return true;
}

bool GeoWriter::write( QIODevice *device, const GeoDataFeature &feature)
{
    QList<GeoDataFeature> list;
    list.append(feature);
    return write(device, list);
}

bool GeoWriter::writeElement(const GeoDataObject &object)
{
    //Add checks to see that everything is ok here
    //

    GeoTagWriter::QualifiedName name( object.nodeType(), m_documentType );
    const GeoTagWriter* writer = GeoTagWriter::recognizes( name );

    if( writer ) {
        if( ! writer->write( object, *this ) ) {
            qDebug() << "An error has been reported by the GeoWriter for: "
                    << name;
            return false;
        }
    } else {
        qDebug() << "There is no GeoWriter registered for: " << name;
        return false;
    }
    return true;
}

void GeoWriter::setDocumentType( const QString &documentType )
{
    m_documentType = documentType;
}

}
