#ifndef PYTHONTEXT_H
#define PYTHONTEXT_H

#include <QQuickTextDocument>
#include <QDebug>
#include <QThread>
#include "pythonhighlighter.h"

#include <iostream>
#include <exception>
#include <pybind11/embed.h>

class PythonTextWorkerThread : public QThread
{
    Q_OBJECT
    void run() override;
};

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

        void setThreadRunning(bool running) { m_threadRunning = running; }
        bool isThreadRunning(void) { return m_threadRunning; }

        Q_INVOKABLE void run(QString code, QString loopFilename="", QString loopStage="DataCollection");

    Q_SIGNALS:
        void pythonCodeChanged();
        void filenameChanged();

    public Q_SLOTS:
        void postCode(void);

    public:
        QString m_code;

    private:
        bool m_threadRunning;
        QString m_threadRunningStage;
        QString m_pythonCode;
        QString m_filename;
        PythonHighlighter* pythonHighlighter;
        std::string replaceKeywords(std::string);
        PythonTextWorkerThread m_pollingThread;
};

#endif // PYTHONTEXT_H
