#ifndef __TEXTEDIT_H__
#define __TEXTEDIT_H__

#include <QPlainTextEdit>

class QtGui;
class QApplication;
class QWidget;

class TextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent=0);

private:
    bool isKey(QString key);

public slots:
    void comment();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent* event);

};
#endif // TEXTEDIT
