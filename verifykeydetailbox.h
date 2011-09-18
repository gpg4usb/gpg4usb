#ifndef __VERIFYKEYDETAILBOX_H__
#define __VERIFYKEYDETAILBOX_H__

#include "keylist.h"
#include "keyserverimportdialog.h"
#include <QDialog>
#include <QGroupBox>

class VerifyKeyDetailBox: public QGroupBox
{
    Q_OBJECT
public:
    explicit VerifyKeyDetailBox(QWidget *parent, GpgME::Context* ctx, KeyList* mKeyList,  gpgme_signature_t signature);

private slots:
    void importFormKeyserver();

private:
    GpgME::Context* mCtx;
    KeyList* mKeyList;
    QString beautifyFingerprint(QString fingerprint);
    QVBoxLayout* mVbox;
    QString fpr;
};

#endif // __VERIFYKEYDETAILBOX_H__

