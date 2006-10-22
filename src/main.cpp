#include <QApplication>
#include <QFile>
#include <QTime>
#include "katlascontrol.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	KAtlasControl katlascontrol(0);
	katlascontrol.show();
	for ( int i = 1; i < argc; ++i ) {
		if ( QFile::exists( app.arguments().at( i ) ) == true )
			katlascontrol.addPlaceMarkFile( argv[i] );

		if ( strcmp( argv[ i ], "--timedemo" ) == 0 ) {
			qDebug( "Running timedemo, stand by..." );
			QTime t;
			t.start();

			for ( int j = 0; j < 10; ++j ) {
				for ( int k = 0; k < 10; ++k ) {
					katlascontrol.moveRight();
				}
				for ( int k = 0; k < 10; ++k ) {
					katlascontrol.moveLeft();
				}
//				katlascontrol.moveUp();
			}
/*
			for ( int j = 0; j < 10; ++j ) {

				for ( int i = 0; i < 5; ++i ){
					katlascontrol.moveLeft();
				}
				for ( int i = 0; i < 5; ++i ){
					katlascontrol.moveRight();
				}
			}
*/
			qDebug( "Timedemo finished in %ims", t.elapsed() );
			qDebug() <<  QString("= %1 fps").arg(100000.0/(float)(t.elapsed()));
			return 0;
		}			 
	}
	return app.exec();
}
