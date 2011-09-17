#ifndef __VERIFYDETAILSDIALOG_H__
#define __VERIFYDETAILSDIALOG_H__

#include "context.h"
#include "keylist.h"
#include "verifykeydetailbox.h"
#include <QDialog>

class VerifyDetailsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VerifyDetailsDialog(QWidget *parent, GpgME::Context* ctx, KeyList* mKeyList,  gpgme_signature_t signature);

private:
    GpgME::Context* mCtx;
    KeyList* mKeyList;
    QVBoxLayout* mVbox;
    QDialogButtonBox* buttonBox;
};

#endif // __VERIFYDETAILSDIALOG_H__
