#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QTextEdit>
#include <QCompleter>
#include <QKeyEvent>
#include <QFocusEvent>


class MyTextEdit:public QTextEdit
{
    Q_OBJECT

    public:
        MyTextEdit(const QString & text,QWidget *parent = nullptr);
        ~MyTextEdit();

        void setCompleter(QCompleter *c);
        QCompleter *completer() const;

        void setTextContent(QString text);
        QString textContent();

    protected:
        void keyPressEvent(QKeyEvent *e) override;
        void focusInEvent(QFocusEvent *e) override;

    private slots:
        void insertCompletion(const QString &completion);

    private:
        QString textUnderCursor() const;

    private:
        QCompleter *c = nullptr;
};

#endif // MYTEXTEDIT_H
