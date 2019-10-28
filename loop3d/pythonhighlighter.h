#ifndef PYTHONHIGHLIGHTER_H
#define PYTHONHIGHLIGHTER_H

#include <QObject>
#include <QQuickTextDocument>
#include <QSyntaxHighlighter>
#include <QRegularExpression>

class PythonHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

    public:
        PythonHighlighter(QTextDocument* parent = nullptr);

    Q_SIGNALS:
        void textHighlighted();

    public Q_SLOTS:

    public:
        void highlightBlock(const QString &text) override;

    private:
        bool matchMultiline(const QString &text, const QRegExp &delimiter, const int inState, const QTextCharFormat &style);
        const QTextCharFormat getTextCharFormat(const QString &colorName, const QString &style = QString());
        class HighlightingRule
        {
            public:
                HighlightingRule(const QString &patternStr, int n, const QTextCharFormat &matchingFormat)
                {
                    originalRuleStr = patternStr;
                    pattern = QRegExp(patternStr);
                    nth = n;
                    format = matchingFormat;
                 }
                QString originalRuleStr;
                QRegExp pattern;
                int nth;
                QTextCharFormat format;
        };

        QStringList keywords;
        QStringList operators;
        QStringList braces;

        QHash<QString, QTextCharFormat> basicStyles;

        QList<HighlightingRule> rules;
        QRegExp triSingleQuote;
        QRegExp triDoubleQuote;
};

#endif // PYTHONTEXT_H
