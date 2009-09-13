#include <qthread.h>
#include <iostream>
#include <string>
#include <cmath>
#include <QtGui>

class QMessageBox;

#include "context.h"

class KeyGenThread : public QThread {
	Q_OBJECT

public:
  KeyGenThread(QString keyGenParams, GpgME::Context *ctx);

signals:
	void keyGenerated();
	
private:
	QString keyGenParams;
	GpgME::Context *ctx;
	bool abort;
	QMutex mutex;

protected:
	void run();

};
