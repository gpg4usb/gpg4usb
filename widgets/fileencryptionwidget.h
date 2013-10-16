#ifndef FILEENCRYPTIONWIDGET_H
#define FILEENCRYPTIONWIDGET_H

#include <QWidget>
#include <QDeclarativeContext>
#include <QtDeclarative/QDeclarativeView>
#include <QGraphicsObject>
#include "gpgcontext.h"
#include "keylist.h"

class FileEncryptionWidget : public QWidget
{

    Q_OBJECT

public:
    FileEncryptionWidget(GpgME::GpgContext *ctx, KeyList *keyList, QWidget *parent = 0);

public slots:
    void slotEncryptFile();
    void slotChooseFile();

private:
    GpgME::GpgContext *mCtx;
    QGraphicsObject *qmlRoot;
    KeyList *mKeyList;

private slots:
    void slotKeySelectionChanged();
};

#endif // FILEENCRYPTIONWIDGET_H
