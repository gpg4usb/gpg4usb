#include "fileencryptionwidget.h"

FileEncryptionWidget::FileEncryptionWidget(GpgME::GpgContext *ctx, KeyList *keyList, QWidget *parent)
    : QWidget(parent), mCtx(ctx), mKeyList(keyList)
{
    QDeclarativeView *qmlView = new QDeclarativeView;
    qmlView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    qmlView->setSource(QUrl("qrc:/qml/fileencryption.qml"));

    qmlRoot = qmlView->rootObject();
    connect( qmlRoot, SIGNAL(okClicked()), this, SLOT(slotEncryptFile()));
    connect( qmlRoot, SIGNAL(fileChooserClicked()), this, SLOT(slotChooseFile()));

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(qmlView);

    connect(mKeyList, SIGNAL(keySelectionChanged()), this, SLOT(slotKeySelectionChanged()));
}

void FileEncryptionWidget::slotKeySelectionChanged() {
    qDebug() << "keySelChanges Signal received in fileencwidget";
     qmlRoot->setProperty("showNoKeySelected", (mKeyList->getSelected()->size() < 1));
}

void FileEncryptionWidget::slotEncryptFile() {

    if(mKeyList->getSelected()->size() < 1) {
        qmlRoot->setProperty("showNoKeySelected", true);
    } else {
        qDebug() << "fine, fine";
    }

}

void FileEncryptionWidget::slotChooseFile() {
    QString path = "";
    if (qmlRoot->property("inputFilePath").toString().size() > 0) {
        path = QFileInfo(qmlRoot->property("inputFilePath").toString()).absolutePath();
    }

    //QString infileName = QFileDialog::getOpenFileName(this, tr("Open File"), path, tr("Files") + tr("All Files (*)"));
    QString infileName = QFileDialog::getOpenFileName(this, tr("Open File"), path);
    qmlRoot->setProperty("inputFilePath", infileName);
}
