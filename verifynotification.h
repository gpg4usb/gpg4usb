#ifndef VERIFYNOTIFICATION_H
#define VERIFYNOTIFICATION_H

#include <QLabel>
#include <QHBoxLayout>
#include <QMenu>
#include <QPushButton>

#include <QWidget>

class VerifyNotification : public QWidget
{
    Q_OBJECT
public:
    explicit VerifyNotification(QWidget *parent = 0);
    void setVerifyLabel(QString text);
signals:

public slots:

private:
    QLabel *verifyLabel;
};

#endif // VERIFYNOTIFICATION_H
