/*
 *
 *      keygendialog.cpp
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This file is part of gpg4usb.
 *
 *      Gpg4usb is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      Gpg4usb is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with gpg4usb.  If not, see <http://www.gnu.org/licenses/>
 */

#include "keygendialog.h"
#include "qdebug.h"

KeyGenDialog::KeyGenDialog(GpgME::GpgContext *ctx, QWidget *parent)
 : QDialog(parent)
{
    mCtx = ctx;
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    this->setWindowTitle(tr("Generate Key"));
    this->setModal(true);
    generateKeyDialog();
}

void KeyGenDialog::generateKeyDialog()
{
    errorLabel = new QLabel(tr(""));
    nameEdit = new QLineEdit(this);
    emailEdit = new QLineEdit(this);
    commentEdit = new QLineEdit(this);

    keySizeSpinBox = new QSpinBox(this);
    keySizeSpinBox->setRange(1024, 4096);
    keySizeSpinBox->setValue(2048);

    keySizeSpinBox->setSingleStep(1024);

    keyTypeComboBox = new QComboBox(this);
    keyTypeComboBox->addItem("RSA");
    keyTypeComboBox->addItem("DSA/Elgamal");
    keyTypeComboBox->setCurrentIndex(0);
    dateEdit = new QDateEdit(QDate::currentDate().addYears(5), this);
    dateEdit->setMinimumDate(QDate::currentDate());
    dateEdit->setDisplayFormat("dd/MM/yyyy");
    dateEdit->setCalendarPopup(true);
    dateEdit->setEnabled(true);

    expireCheckBox = new QCheckBox(this);
    expireCheckBox->setCheckState(Qt::Unchecked);

    passwordEdit = new QLineEdit(this);
    repeatpwEdit = new QLineEdit(this);

    passwordEdit->setEchoMode(QLineEdit::Password);
    repeatpwEdit->setEchoMode(QLineEdit::Password);

    pwStrengthSlider = new QSlider(this);
    pwStrengthSlider->setOrientation(Qt::Horizontal);
    pwStrengthSlider->setMaximum(6);
    pwStrengthSlider->setDisabled(true);
    pwStrengthSlider->setToolTip(tr("Password Strength"));
    pwStrengthSlider->setTickPosition(QSlider::TicksBelow);

    QGridLayout *vbox1 = new QGridLayout;

    vbox1->addWidget(new QLabel(tr("Name:")), 0, 0);
    vbox1->addWidget(new QLabel(tr("E-Mailaddress:")), 1, 0);
    vbox1->addWidget(new QLabel(tr("Comment:")), 2, 0);
    vbox1->addWidget(new QLabel(tr("Expiration Date:")), 3, 0);
    vbox1->addWidget(new QLabel(tr("Never Expire")), 3, 3);
    vbox1->addWidget(new QLabel(tr("KeySize (in Bit):")), 4, 0);
    vbox1->addWidget(new QLabel(tr("Key Type:")), 5, 0);
    vbox1->addWidget(new QLabel(tr("Password:")), 6, 0);
    vbox1->addWidget(new QLabel(tr("Password: Strength\nWeak -> Strong")), 6, 3);
    vbox1->addWidget(new QLabel(tr("Repeat Password:")), 7, 0);

    vbox1->addWidget(nameEdit, 0, 1);
    vbox1->addWidget(emailEdit, 1, 1);
    vbox1->addWidget(commentEdit, 2, 1);
    vbox1->addWidget(dateEdit, 3, 1);
    vbox1->addWidget(expireCheckBox, 3, 2);
    vbox1->addWidget(keySizeSpinBox, 4, 1);
    vbox1->addWidget(keyTypeComboBox,5, 1);
    vbox1->addWidget(passwordEdit, 6, 1);
    vbox1->addWidget(repeatpwEdit, 7, 1);
    vbox1->addWidget(pwStrengthSlider, 7, 3);

    QWidget *nameList = new QWidget(this);
    nameList->setLayout(vbox1);

    QVBoxLayout *vbox2 = new QVBoxLayout();
    vbox2->addWidget(nameList);
    vbox2->addWidget(errorLabel);
    vbox2->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotKeyGenAccept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(expireCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotExpireBoxChanged()));
    connect(passwordEdit, SIGNAL(textChanged(QString)), this, SLOT(slotPasswordEditChanged()));
//    connect(keyTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotKeyTypeChanged()));
//    connect(keySizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotKeySizeChanged()));
    this->setLayout(vbox2);
}

void KeyGenDialog::slotKeyGenAccept()
{
    QString errorString = "";
    QString keyGenParams = "";
    /**
     * check for errors in keygen dialog input
     */
    if ((nameEdit->text()).size() < 5) {
        errorString.append(tr("  Name must contain at least five characters.  \n"));
    }
    if (passwordEdit->text() != repeatpwEdit->text()) {
        errorString.append(tr("  Password and Repeat don't match.  "));
    }

    if (errorString.isEmpty()) {

        KgpgCore::KgpgKeyAlgo algo;
        if(keyTypeComboBox->currentText() == "RSA") {
            algo = KgpgCore::ALGO_RSA_RSA;
        } else if (keyTypeComboBox->currentText() == "DSA/Elgamal") {
            algo = KgpgCore::ALGO_DSA_ELGAMAL;
        }

        int expiredays = 0;
        if (!expireCheckBox->checkState()) {
            expiredays = QDate::currentDate().daysTo(dateEdit->date());
        }

        KGpgGenerateKey *genkey = new KGpgGenerateKey(this,
                                                      nameEdit->text(),
                                                      emailEdit->text(),
                                                      commentEdit->text(),
                                                      algo,
                                                      keySizeSpinBox->cleanText().toInt(),
                                                      expiredays,
                                                      'd',
                                                      passwordEdit->text());


        connect(genkey, SIGNAL(done(int)), SLOT(slotGenkeyDone(int)));
        connect(genkey, SIGNAL(infoProgress(qulonglong,qulonglong)), SLOT(slotInfoProgress(qulonglong,qulonglong)));
        genkey->start();

        this->accept();

        m_dialog = new QDialog(this, Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        m_dialog->setModal(true);
        m_dialog->setWindowTitle(tr("Generating Key..."));

        QLabel *waitMessage = new QLabel(tr("Collecting random data for key generation.\n This may take a while.\n To speed up the process use your computer\n (e.g. browse the net, listen to music,...)"));
        QProgressBar *pb = new QProgressBar();
        pb->setRange(0, 0);

        QVBoxLayout *layout = new QVBoxLayout(m_dialog);
        layout->addWidget(waitMessage);
        layout->addWidget(pb);
        m_dialog->setLayout(layout);

        m_dialog->show();

    } else {
        /**
         * create error message
         */
        errorLabel->setAutoFillBackground(true);
        QPalette error = errorLabel->palette();
        error.setColor(QPalette::Background, "#ff8080");
        errorLabel->setPalette(error);
        errorLabel->setText(errorString);

        this->show();
    }
}

void KeyGenDialog::slotGenkeyDone(int result) {

    qDebug() << "genkey done";

    KGpgGenerateKey *genkey = qobject_cast<KGpgGenerateKey *>(sender());
    Q_ASSERT(genkey != NULL);

    const QString infomessage(tr("Generating new key pair"));

    // TODO:
    switch (result) {
    case KGpgTransaction::TS_BAD_PASSPHRASE:
            QMessageBox::warning(this, infomessage, tr("Bad passphrase. Cannot generate a new key pair."));
            break;
        case KGpgTransaction::TS_USER_ABORTED:
            QMessageBox::warning(this, infomessage, tr("Aborted by the user. Cannot generate a new key pair."));
            break;
        case KGpgTransaction::TS_INVALID_EMAIL:
            QMessageBox::warning(this, infomessage, tr("The email address is not valid. Cannot generate a new key pair."));
            break;
        case KGpgGenerateKey::TS_INVALID_NAME:
            QMessageBox::warning(this, infomessage, tr("The name is not accepted by gpg. Cannot generate a new key pair."));
            break;
        case KGpgTransaction::TS_OK:
            mCtx->emitKeyDBChanged();
            m_dialog->close();
            // TODO: could be much more info, have a look at keysmanager::slotGenerateKeyDone from kgpg
            QMessageBox::information(this,tr("Success"),tr("New key created: ").arg(genkey->getName()));
            break;
        default:
            QMessageBox::warning(this, infomessage, tr("gpg process did not finish. Cannot generate a new key pair.").arg(genkey->gpgErrorMessage()));
    }

}

void KeyGenDialog::slotInfoProgress(qulonglong processedAmount, qulonglong totalAmount)
{
    qDebug() << "pA: " << processedAmount << " | tA: " << totalAmount;
}

void KeyGenDialog::slotExpireBoxChanged()
{
    if (expireCheckBox->checkState()) {
        dateEdit->setEnabled(false);
    } else {
        dateEdit->setEnabled(true);
    }
}

void KeyGenDialog::slotPasswordEditChanged()
{
    pwStrengthSlider->setValue(checkPassWordStrength());
    update();
}

int KeyGenDialog::checkPassWordStrength()
{
    int strength = 0;

    // increase strength by two, if password has more than 7 characters
    if ((passwordEdit->text()).length() > 7) {
        strength = strength + 2;
    }

    // increase strength by one, if password contains a digit
    if ((passwordEdit->text()).contains(QRegExp("\\d"))) {
        strength++;
    }

    // increase strength by one, if password contains a lowercase character
    if ((passwordEdit->text()).contains(QRegExp("[a-z]"))) {
        strength++;
    }

    // increase strength by one, if password contains an uppercase character
    if ((passwordEdit->text()).contains(QRegExp("[A-Z]"))) {
        strength++;
    }

    // increase strength by one, if password contains a non-word character
    if ((passwordEdit->text()).contains(QRegExp("\\W"))) {
        strength++;
    }

    return strength;
}

