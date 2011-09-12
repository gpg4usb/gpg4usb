#ifndef VERIFYDETAILSDIALOG_H
#define VERIFYDETAILSDIALOG_H

#include "context.h"
#include "keylist.h"
#include <QDialog>

class VerifyDetailsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VerifyDetailsDialog(QWidget *parent, GpgME::Context* ctx, KeyList* mKeyList,  gpgme_signature_t signature);

private:
    QGroupBox* addDetailBox( gpgme_signature_t signature );
    GpgME::Context* mCtx;
    KeyList* mKeyList;

signals:

public slots:

};

#endif // VERIFYDETAILSDIALOG_H
