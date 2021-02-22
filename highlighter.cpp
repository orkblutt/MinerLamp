#include "highlighter.h"
#include <QDebug>
#include <QColor>

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    QString escaped;
    QTextCharFormat strFormat;


    strFormat.setFontWeight(QFont::Bold);
    strFormat.setForeground(Qt::cyan);
    rule.pattern = QRegularExpression(" [0-9]{1,5}.[0-9]{1,2} Mh/s");
    rule.format = strFormat;
    _highlightingRules.append(rule);

    strFormat.setFontWeight(QFont::Bold);
    strFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression(" 0.00 Mh");
    rule.format = strFormat;
    _highlightingRules.append(rule);

    escaped = QRegularExpression::escape("**Accepted");
    strFormat.setForeground(Qt::green);
    rule.pattern = QRegularExpression(escaped);
    rule.format = strFormat;
    _highlightingRules.append(rule);

    escaped = QRegularExpression::escape("**Rejected");
    strFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression(escaped);
    rule.format = strFormat;
    _highlightingRules.append(rule);

    escaped = QRegularExpression::escape("(stale)");
    strFormat.setForeground(QColor(255, 165, 0)); //orange
    rule.pattern = QRegularExpression(escaped);
    rule.format = strFormat;
    _highlightingRules.append(rule);


}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, _highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);
}
