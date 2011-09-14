#ifndef __VERIFYDETAILSDIALOG_H__
#define __VERIFYDETAILSDIALOG_H__

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
    QString beautifyFingerprint(QString fingerprint);
};

#endif // __VERIFYDETAILSDIALOG_H__
