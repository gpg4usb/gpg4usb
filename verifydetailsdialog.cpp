#include "verifydetailsdialog.h"

VerifyDetailsDialog::VerifyDetailsDialog(QWidget *parent, GpgME::Context* ctx, KeyList* keyList, gpgme_signature_t signature) :
    QDialog(parent)
{
    this->mCtx = ctx;
    this->mKeyList = keyList;

    mVbox = new QVBoxLayout();
    QDateTime timestamp;
    timestamp.setTime_t(signature->timestamp);

    mVbox->addWidget(new QLabel(tr("Text was completly signed on %1 by:\n").arg(timestamp.toString(Qt::SystemLocaleShortDate))));

    while (signature) {
        VerifyKeyDetailBox *sbox = new VerifyKeyDetailBox(this,mCtx,mKeyList,signature);
        //QGroupBox* sbox = addDetailBox(signature);
        signature = signature->next;
        mVbox->addWidget(sbox);
    }

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    mVbox->addWidget(buttonBox);

    this->setLayout(mVbox);
    this->setWindowTitle(tr("Signaturedetails"));
    this->show();

    exec();
}


