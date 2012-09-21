
#ifndef FINDWIDGET_H
#define FINDWIDGET_H

#include "editorpage.h"

#include <QWidget>

/**
 * @brief Class for handling the find widget shown at buttom of a textedit-page
 */
class FindWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief
     *
     * @param parent The parent widget
     */
    explicit FindWidget(QWidget *parent, QTextEdit *edit);

private:
    QTextEdit *mTextpage; /** Textedit associated to the notification */
    QLineEdit *findEdit; /** Label holding the text shown in verifyNotification */
    bool haveHit;
    int start;
    void keyPressEvent( QKeyEvent* e );
    QTextCursor cursor;

private slots:
    void findNext();
    void find();
};
#endif // FINDWIDGET_H
