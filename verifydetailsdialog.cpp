#include "verifydetailsdialog.h"

VerifyDetailsDialog::VerifyDetailsDialog(QWidget *parent, GpgME::Context* ctx, KeyList* keyList, gpgme_signature_t signature) :
    QDialog(parent)
{
    this->mCtx = ctx;
    this->mKeyList = keyList;

    QVBoxLayout* mVbox = new QVBoxLayout();

    QDateTime timestamp;
    timestamp.setTime_t(signature->timestamp);

    mVbox->addWidget(new QLabel(tr("Text was completly signed on %1 by:\n").arg(timestamp.toString(Qt::SystemLocaleShortDate))));

    while (signature) {
        QGroupBox* sbox = addDetailBox(signature);
        signature = signature->next;
        mVbox->addWidget(sbox);
    }

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    mVbox->addWidget(buttonBox);

    this->setLayout(mVbox);
    this->setWindowTitle(tr("Signaturedetails"));
    this->show();

    exec();
}

QGroupBox* VerifyDetailsDialog::addDetailBox( gpgme_signature_t signature ) {

    QGridLayout *grid = new QGridLayout();

    switch (gpg_err_code(signature->status))
    {
        case GPG_ERR_NO_PUBKEY:
        {
            //verifyStatus=VERIFY_ERROR_WARN;
            //vn->addVerifyDetailLabel(tr("Key not present in keylist: ")+QString(signature->fpr),VERIFY_ERROR_WARN, false);
            grid->addWidget(new QLabel(tr("Status:")), 0, 0);
            grid->addWidget(new QLabel(tr("Fingerprint:")), 1, 0);
            grid->addWidget(new QLabel("Key not present in keylist"), 0, 1);
            grid->addWidget(new QLabel(signature->fpr), 1, 1);

            break;
        }
        case GPG_ERR_NO_ERROR:
        {
            /*QString name = mKeyList->getKeyNameByFpr(signature->fpr);
            QString email =mKeyList->getKeyEmailByFpr(signature->fpr);
            vn->addVerifyDetailLabel(tr("Name: ")+name+"\n"+tr("EMail: ")+email+"\n"+tr("Fingerprint: ")+QString(signature->fpr),VERIFY_ERROR_OK, false);
            break;*/

            grid->addWidget(new QLabel(tr("Name:")), 0, 0);
            grid->addWidget(new QLabel(tr("EMail:")), 1, 0);
            grid->addWidget(new QLabel(tr("Fingerprint:")), 2, 0);
            grid->addWidget(new QLabel(tr("Status:")), 3, 0);

            grid->addWidget(new QLabel(mKeyList->getKeyNameByFpr(signature->fpr)), 0, 1);
            grid->addWidget(new QLabel(mKeyList->getKeyEmailByFpr(signature->fpr)), 1, 1);
            grid->addWidget(new QLabel(signature->fpr), 2, 1);
            grid->addWidget(new QLabel("OK"), 3, 1);


        }
        default:
        {
            //verifyStatus=VERIFY_ERROR_WARN;
            /*vn->addVerifyDetailLabel(tr("Key with Fingerprint: ")+
                                     QString(signature->fpr)+"\n"+tr("Signature status: ")+gpg_strerror(signature->status)+"\n"
                                     +tr("Signature validity reason: ")+QString(gpgme_strerror(signature->validity_reason)),
                                     VERIFY_ERROR_WARN, false);
                                     */
            break;
        }
    }



    QGroupBox *sbox = new QGroupBox(tr("Key"));
    sbox->setLayout(grid);
    return sbox;

}
