#ifndef HELPPAGE_H
#define HELPPAGE_H

#include <QWidget>
#include <QTextBrowser>
#include <QVBoxLayout>

class HelpPage : public QWidget
{
    Q_OBJECT
public:
    explicit HelpPage(const QString path, QWidget *parent = 0);

signals:

public slots:

};

#endif // HELPPAGE_H
