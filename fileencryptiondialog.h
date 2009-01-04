#include <QDialog>
#include <QLineEdit>
#include <QRadioButton>

#include "context.h"
#include "keylist.h"

class FileEncryptionDialog : public QDialog
{
    Q_OBJECT

public:
    FileEncryptionDialog(GpgME::Context *ctx, QString iconPath);

public slots:    
   void selectInputFile();
   void selectOutputFile();
   void executeAction();
   void hideKeyList();
   void showKeyList();

private:
    QLineEdit *outputFileEdit;
    QLineEdit *inputFileEdit;
    QRadioButton *radioEnc;
    QRadioButton *radioDec;
    
protected:
	GpgME::Context *mCtx;
	KeyList *mKeyList;

};
