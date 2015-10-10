#ifndef CLUAEDITOR_H
#define CLUAEDITOR_H

#include <QPlainTextEdit>
#include <QObject>
#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class CLineNumberArea;
class CLuaSyntaxHighlighter;

/**
 * A Lua script editor for the AI scripts
 */
class CLuaEditor : public QPlainTextEdit
{
    Q_OBJECT;
public:
    CLuaEditor(QWidget* parent = 0);

    void LineNumberAreaPaintEvent(QPaintEvent*);
    int LineNumberAreaWidth();

    void LoadFile(const QString&);
    QString SaveFile(const QString&, const QString&);

protected:
    void resizeEvent(QResizeEvent*);

private slots:
    void UpdateLineNumberAreaWidth(const int&);
    void HighlightCurrentLine();
    void UpdateLineNumberArea(const QRect&, const int&);

private:
    QWidget* DLineNumberArea;
};

class CLineNumberArea : public QWidget
{
public:
    CLineNumberArea(CLuaEditor* editor) : QWidget(editor)
    {
        DEditor = editor;
    }

    QSize sizeHint() const
    {
        return QSize(DEditor->LineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent* event)
    {
        DEditor->LineNumberAreaPaintEvent(event);
    }

private:
    CLuaEditor* DEditor;
};

/**
 * Syntax highlighter for the {@link CLuaEditor}
 */
class CLuaSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    CLuaSyntaxHighlighter(CLuaEditor* parent = 0);

protected:
    void highlightBlock(const QString&);

private:
    struct HighlightRule
    {
        QRegExp DPattern;
        QTextCharFormat DFormat;
    };

    QVector<HighlightRule> DHighlightingRules;
    QRegExp DCommentStartExp;
    QRegExp DCommentEndExp;

    QTextCharFormat DKeywordFormat;
    QTextCharFormat DClassFormat;
    QTextCharFormat DSingleCommentFormat;
    QTextCharFormat DMultiCommentFormat;
    QTextCharFormat DQuoteFormat;
    QTextCharFormat DFunctionFormat;
};

#endif // CLUAEDITOR_H
