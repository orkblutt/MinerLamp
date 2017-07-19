#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    _0mhsFormat.setFontWeight(QFont::Bold);
    _0mhsFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression("\\b0.00MH/s\\b");
    rule.format = _0mhsFormat;
    _highlightingRules.append(rule);

    _shareFormat.setFontWeight(QFont::Bold);
    _shareFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression("Solution[^\n]*");
    rule.format = _shareFormat;
    _highlightingRules.append(rule);

    _submitAdmitted.setFontWeight(QFont::Bold);
    _submitAdmitted.setForeground(Qt::green);
    rule.pattern = QRegularExpression("B-)[^\n]*");
    rule.format = _shareFormat;
    _highlightingRules.append(rule);

    _submitRefused.setFontWeight(QFont::Bold);
    _submitRefused.setForeground(Qt::red);
    rule.pattern = QRegularExpression(":-([^\n]*");
    rule.format = _submitRefused;
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
