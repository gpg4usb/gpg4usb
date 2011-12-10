#include <QObject>
#include <QtTest/QtTest>
#include <../gpgcontext.h>

class TestGpgContext : public QObject
{
    Q_OBJECT

public: 
	TestGpgContext();

private:
	GpgME::GpgContext* mCtx;

private slots:
    void passwordSize();

};

TestGpgContext::TestGpgContext() {
	mCtx = new GpgME::GpgContext();
}

void TestGpgContext::passwordSize() {

        QVERIFY(mCtx->listKeys().size() == 0);

        qDebug() << "import:";
        QFile* file = new QFile("../testdata/seckey-1.asc");
        file->open(QIODevice::ReadOnly);
        mCtx->importKey(file->readAll());

        qDebug() << "list:";
        foreach(GpgKey key, mCtx->listKeys()) {
            qDebug() << key.id;
        }

        QVERIFY(mCtx->listKeys().size() == 1);

        QString password = "abcabc";
        QString params = "<GnupgKeyParms format=\"internal\">\n"
                       "Key-Type: DSA\n"
                       "Key-Length: 1024\n"
                       "Subkey-Type: ELG-E\n"
                       "Subkey-Length: 1024\n"
                       "Name-Real: testa\n"
                       "Expire-Date: 0\n";
                       "Passphrase: " + password + "\n"
                      "</GnupgKeyParms>";

        /*qDebug() << "gen:";
        mCtx->generateKey(&params);
        QVERIFY(mCtx->listKeys().size() == 1);
        qDebug() << "done.";*/
}

QTEST_MAIN(TestGpgContext)
#include "testgpgcontext.moc"
