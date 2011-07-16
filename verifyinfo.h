class QString;
#include <QtGui>

class VerifyInfo
{
    Q_OBJECT

public:
    int getKeyId();
    void setKeyId(int keyId);
    int getStatus();
    void setStatus(int status);
    QString getSummary();
    void setSummary(QString summary);

private:
    QString *keyid;
    int status;
    int summary;

};

