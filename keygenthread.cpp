#include "keygenthread.h"


KeyGenThread::KeyGenThread(QString keyGenParams, GpgME::Context *ctx)
{
	qDebug() << keyGenParams;
	this->keyGenParams=keyGenParams;
	this->ctx=ctx;
	abort=false;
}

void KeyGenThread::run()
{
	qDebug() << keyGenParams;
	qDebug("hallo2");
	ctx->generateKey(&keyGenParams); 
	
	qDebug("hallo323");
	
}
