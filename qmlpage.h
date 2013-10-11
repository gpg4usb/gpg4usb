#ifndef QMLPAGE_H
#define QMLPAGE_H

#include <QWidget>

class QMLPage : public QWidget
{

    Q_OBJECT

public:
    QMLPage(const QString qmlfile, QWidget *parent = 0);

};

#endif // QMLPAGE_H
