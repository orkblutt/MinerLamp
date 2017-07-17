#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QObject>
#include <QTextCharFormat>
#include <QSyntaxHighlighter>
#include <QRegularExpression>

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };


    QVector<HighlightingRule> _highlightingRules;

    QTextCharFormat _0mhsFormat;
    QTextCharFormat _shareFormat;

};

#endif // HIGHLIGHTER_H
