#ifndef MYHIGHLIGHTER_H
#define MYHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QString>
#include <QTranslator>

class QTextDocument;

class MyHighLighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    MyHighLighter(QStringList keywordPatterns,
                  QStringList subkeywordPatterns,
                  QTextDocument * document);

    void addRule(QString pattern,QTextCharFormat format);
    void addSubRule(QString pattern,QTextCharFormat format);
    void clearRules();
    void clearSubRules();

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules,subhighlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat subkeywordFormat;
    QTextCharFormat variablesFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat chiffresFormat;
};
#endif // MYHIGHLIGHTER_H
