#ifndef PYTHONTEXT_H
#define PYTHONTEXT_H

#include <QQuickTextDocument>
#include <QDebug>
#include "pythonhighlighter.h"

#include <iostream>
#include <exception>


class PythonText : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument* textDocToHighlight READ mainTextEdit WRITE setMainTextEdit)
    Q_PROPERTY(QString pythonCode MEMBER m_pythonCode NOTIFY pythonCodeChanged)
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)

    public:
        PythonText(QObject* parent = nullptr);

        QQuickTextDocument* mainTextEdit() const { return nullptr; }
        void setMainTextEdit(QQuickTextDocument* text);

        QString pythonCode() { return m_pythonCode; }
        void setPythonCode(QString in) { m_pythonCode = in; }

        QString filename() { return m_filename; }
        void setFilename(QString in);

        Q_INVOKABLE void run(QString code, QString loopFilename="", bool useResult=false);

    Q_SIGNALS:
        void pythonCodeChanged();
        void filenameChanged();

    private:
        QString m_pythonCode;
        QString m_filename;
        PythonHighlighter* pythonHighlighter;
};

#endif // PYTHONTEXT_H
