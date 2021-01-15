#include "MyHighLighter.h"

MyHighLighter::MyHighLighter(QStringList keywordPatterns,
                             QStringList subkeywordPatterns,
                             QTextDocument *document)
    : QSyntaxHighlighter(document)
{
    HighlightingRule rule;

    clearSubRules();

    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);

    foreach (const QString &pattern, keywordPatterns) {
        addRule(QString("\\b")+pattern+QString("\\b"),keywordFormat);
    }

    subkeywordFormat.setForeground(Qt::black);
    subkeywordFormat.setFontWeight(QFont::Bold);

    foreach (const QString &pattern, subkeywordPatterns) {
        addSubRule(QString("\\b")+pattern+QString("\\b"),subkeywordFormat);
    }

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);

//    chiffresFormat.setForeground(Qt::white);
//    rule.pattern = QRegularExpression("[0-9]");
//    rule.format = chiffresFormat;
//    highlightingRules.append(rule);

    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}

void MyHighLighter::addRule(QString pattern,QTextCharFormat format)
{
    HighlightingRule rule;
    rule.pattern = QRegularExpression(pattern);
    rule.format = format;
    highlightingRules.append(rule);
}

void MyHighLighter::addSubRule(QString pattern,QTextCharFormat format)
{
    HighlightingRule rule;
    rule.pattern = QRegularExpression(pattern);
    rule.format = format;
    subhighlightingRules.append(rule);
}

void MyHighLighter::clearRules()
{
    highlightingRules.clear();
}

void MyHighLighter::clearSubRules()
{
    subhighlightingRules.clear();
}

void MyHighLighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    foreach (const HighlightingRule &rule, subhighlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                    + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
