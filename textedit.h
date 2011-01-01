#ifndef __TEXTEDIT_H__
#define __TEXTEDIT_H__

#include <QPlainTextEdit>

#include <QtGui>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QApplication>
#include <QFile>

class QWidget;
class QString;

class TextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent=0);
    void setCurrentFile(const QString &fileName);
    void loadFile(const QString &fileName);
    bool maybeSave();

public slots:
    void quote();
    bool save();
    bool saveAs();
    void open();
    void print();

private:
    bool isKey(QString key);
    bool saveFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    QString curFile;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent* event);

};
#endif // TEXTEDIT
