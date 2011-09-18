#include "verifydetailsdialog.h"

VerifyDetailsDialog::VerifyDetailsDialog(QWidget *parent, GpgME::Context* ctx, KeyList* keyList, gpgme_signature_t signature) :
    QDialog(parent)
{
    this->mCtx = ctx;
    this->mKeyList = keyList;

    mVbox = new QVBoxLayout();

    // Timestamp of creation of the signature
    QDateTime timestamp;
    timestamp.setTime_t(signature->timestamp);

    // Information for general verify information
    mVbox->addWidget(new QLabel(tr("Text was completly signed on %1 by:\n").arg(timestamp.toString(Qt::SystemLocaleShortDate))));

    // Add informationbox for every single key
    while (signature) {
        VerifyKeyDetailBox *sbox = new VerifyKeyDetailBox(this,mCtx,mKeyList,signature);
        signature = signature->next;
        mVbox->addWidget(sbox);
    }

    // Button Box for close button
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    mVbox->addWidget(buttonBox);

    this->setLayout(mVbox);
    this->setWindowTitle(tr("Signaturedetails"));
    this->show();
    this->exec();
}
