#include "verifykeydetailbox.h"

VerifyKeyDetailBox::VerifyKeyDetailBox(QWidget *parent, GpgME::Context* ctx, KeyList* keyList, gpgme_signature_t signature) :
    QGroupBox(parent)
{
    this->mCtx = ctx;
    this->mKeyList = keyList;
    this->fpr=signature->fpr;
    this->setTitle(signature->fpr);

    grid = new QGridLayout();

    switch (gpg_err_code(signature->status))
    {
        case GPG_ERR_NO_PUBKEY:
        {
            QPushButton *importButton = new QPushButton(tr("Import from keyserver"));
            connect(importButton, SIGNAL(clicked()), this, SLOT(importFormKeyserver()));

            grid->addWidget(new QLabel(tr("Status:")), 0, 0);
            grid->addWidget(new QLabel(tr("Fingerprint:")), 1, 0);
            grid->addWidget(new QLabel(tr("Key not present in keylist")), 0, 1);
            grid->addWidget(new QLabel(signature->fpr), 1, 1);
            grid->addWidget(importButton, 2,0,2,1);
            break;
        }
        case GPG_ERR_NO_ERROR:
        {
            grid->addWidget(new QLabel(tr("Name:")), 0, 0);
            grid->addWidget(new QLabel(tr("EMail:")), 1, 0);
            grid->addWidget(new QLabel(tr("Fingerprint:")), 2, 0);
            grid->addWidget(new QLabel(tr("Status:")), 3, 0);

            grid->addWidget(new QLabel(mKeyList->getKeyNameByFpr(signature->fpr)), 0, 1);
            grid->addWidget(new QLabel(mKeyList->getKeyEmailByFpr(signature->fpr)), 1, 1);
            grid->addWidget(new QLabel(beautifyFingerprint(signature->fpr)), 2, 1);
            grid->addWidget(new QLabel(tr("OK")), 3, 1);

            break;
        }
        default:
        {
            grid->addWidget(new QLabel(tr("Status:")), 0, 0);
            grid->addWidget(new QLabel(tr("Fingerprint:")), 1, 0);

            grid->addWidget(new QLabel(gpg_strerror(signature->status)), 0, 1);
            grid->addWidget(new QLabel(beautifyFingerprint(signature->fpr)), 1, 1);

            break;
        }
    }
    this->setLayout(grid);
}

void VerifyKeyDetailBox::importFormKeyserver()
{
    KeyServerImportDialog *importDialog =new KeyServerImportDialog(mCtx,this);
    importDialog->import(fpr);
}

QString VerifyKeyDetailBox::beautifyFingerprint(QString fingerprint)
{
    uint len = fingerprint.length();
    if ((len > 0) && (len % 4 == 0))
        for (uint n = 0; 4 *(n + 1) < len; ++n)
            fingerprint.insert(5 * n + 4, ' ');
    return fingerprint;
}
