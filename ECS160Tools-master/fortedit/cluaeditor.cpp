#include <QtGui>
#include <QMessageBox>
#include "cmapexception.h"

#include "cluaeditor.h"

#define NOT_IN_COMMENT 0
#define IN_COMMENT 1

/**
 * Constructs a new Lua script editor
 * @param parent parent widget if any, usually null
 */
CLuaEditor::CLuaEditor(QWidget* parent) : QPlainTextEdit(parent)
{
    DLineNumberArea = new CLineNumberArea(this);

    QObject::connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(UpdateLineNumberAreaWidth(int)));
    QObject::connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(UpdateLineNumberArea(QRect,int)));
    QObject::connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(HighlightCurrentLine()));

    UpdateLineNumberAreaWidth(0);
    HighlightCurrentLine();
    setWindowTitle("Lua Script Editor");

    QPalette P = palette();
    P.setColor(QPalette::Base, QColor(25, 25, 25));
    P.setColor(QPalette::Text, QColor(239, 239, 239));
    setPalette(P);

    QFont Font;
    Font.setFamily("Courier");
    Font.setFixedPitch(true);
    Font.setPointSize(12);

    QFontMetrics Metrics(Font);
    setTabStopWidth(4 * Metrics.width(' '));

    setFont(Font);

    CLuaSyntaxHighlighter* Highlighter = new CLuaSyntaxHighlighter(this);
    (void) Highlighter;
}

/**
 * @return the width of the line number column
 */
int CLuaEditor::LineNumberAreaWidth()
{
    int Digits = 1;
    int Max = qMax(1, blockCount());
    while (Max >= 10)
    {
        Max /= 10;
        ++Digits;
    }

    return 3 + fontMetrics().width(QLatin1Char('9')) * Digits;
}

/**
 * Updates the width of the line number column
 */
void CLuaEditor::UpdateLineNumberAreaWidth(const int&)
{
    setViewportMargins(LineNumberAreaWidth(), 0, 0, 0);
}

/**
 * Updates the line number column values
 * @param rect displaying rect
 * @param dy scroll amount
 */
void CLuaEditor::UpdateLineNumberArea(const QRect& rect, const int& dy)
{
    if (dy)
    {
        DLineNumberArea->scroll(0, dy);
    }
    else
    {
        DLineNumberArea->update(0, rect.y(), DLineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect()))
    {
        UpdateLineNumberAreaWidth(0);
    }
}

/**
 * Callback event when the editor is resized
 * @param event QResizeEvent
 */
void CLuaEditor::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);
    QRect Content = contentsRect();
    DLineNumberArea->setGeometry(QRect(Content.left(), Content.top(), LineNumberAreaWidth(), Content.height()));
}

/**
 * Highlights the current line in the editor
 */
void CLuaEditor::HighlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> ExtraSelections;

    if (!isReadOnly())
    {
        QTextEdit::ExtraSelection Selection;
        QColor LineColor = QColor(40, 40, 40);

        Selection.format.setBackground(LineColor);
        Selection.format.setProperty(QTextFormat::FullWidthSelection, true);;
        Selection.cursor = textCursor();
        Selection.cursor.clearSelection();
        ExtraSelections.append(Selection);
    }

    setExtraSelections(ExtraSelections);
}

/**
 * Callback to paint the line number column
 * @param event QPaintEvent
 */
void CLuaEditor::LineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter Painter(DLineNumberArea);
    Painter.fillRect(event->rect(), Qt::black);

    QTextBlock Block = firstVisibleBlock();
    int BlockNum = Block.blockNumber();
    int Top = (int) blockBoundingGeometry(Block).translated(contentOffset()).top();
    int Bottom = Top + (int) blockBoundingRect(Block).height();

    while (Block.isValid() && Top <= event->rect().bottom())
    {
        if (Block.isVisible() && Bottom >= event->rect().top())
        {
            QString Number = QString::number(BlockNum + 1);
            Painter.setPen(Qt::lightGray);
            Painter.drawText(0, Top, DLineNumberArea->width(), fontMetrics().height(),
                              Qt::AlignRight, Number);
        }

        Block = Block.next();
        Top = Bottom;
        Bottom = Top + (int) blockBoundingRect(Block).height();
        ++BlockNum;
    }
}

/**
 * Loads the contents of a file into the editor
 * @param fname path to a Lua script
 */
void CLuaEditor::LoadFile(const QString& fname)
{
    QFile file(fname);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        this->clear();
        return;
    }

    QTextStream ReadFile(&file);
    this->setPlainText(ReadFile.readAll());

    file.close();
}

/**
 * Saves the contents of the editor to a file
 * @param SavePath directory to save to
 * @param MapTitle the title of the file
 * @return the save path
 */
QString CLuaEditor::SaveFile(const QString& SavePath, const QString & MapTitle)
{
    QString path = SavePath;
    QStringList pathList = SavePath.split("/");

    QString temp = pathList[pathList.length() - 1];
    path.remove(temp);

    // To Do: Change file type to whatever it will be
    path += MapTitle + "_AI_Script" + ".lua";

    QFile outputFile(path);
    try
    {
        outputFile.open(QIODevice::WriteOnly);
        if (!outputFile.isOpen())
        {
            return NULL;
        }
        else
        {
            QTextStream outStream(&outputFile);
            outStream << this->toPlainText() << endl;
            outputFile.close();
        }
    }
    catch (CMapException& E)
    {
        QMessageBox* Msg = new QMessageBox;
        Msg->setText(QString("Error saving! %1,\n%2" ).arg(path).arg(E.Message()));
        Msg->show();
    }

    return path;
}

/**
 * Constructs a new syntax highlighter for a {@link CLuaEditor}
 * @param parent Parent widget, usually null
 */
CLuaSyntaxHighlighter::CLuaSyntaxHighlighter(CLuaEditor* parent)
    : QSyntaxHighlighter(parent->document())
{
    HighlightRule Rule;

    DKeywordFormat.setForeground(QColor(214, 57, 100));
    DKeywordFormat.setFontWeight(QFont::Bold);
    QStringList KeywordPatterns;
    KeywordPatterns << "\\band\\b"      << "\\bbreak\\b"
                    << "\\bdo\\b"       << "\\belse\\b"
                    << "\\belseif\\b"   << "\\bend\\b"
                    << "\\bfalse\\b"    << "\\bfor\\b"
                    << "\\bfunction\\b" << "\\bif\\b"
                    << "\\bin\\b"       << "\\blocal\\b"
                    << "\\bnil\\b"      << "\\bnot\\b"
                    << "\\bor\\b"       << "\\brepeat\\b"
                    << "\\breturn\\b"   << "\\bthen\\b"
                    << "\\btrue\\b"     << "\\buntil\\b"
                    << "\\bwhile\\b";

    foreach (const QString& Pattern, KeywordPatterns)
    {
        Rule.DPattern = QRegExp(Pattern);
        Rule.DFormat = DKeywordFormat;
        DHighlightingRules.append(Rule);
    }

    QColor ZenGreen(160, 207, 128);

    DClassFormat.setFontWeight(QFont::Bold);
    DClassFormat.setForeground(ZenGreen);
    Rule.DPattern = QRegExp("\\b[A-Za-z0-9]+[\\.|\\:]");
    Rule.DFormat = DClassFormat;
    DHighlightingRules.append(Rule);

    DQuoteFormat.setForeground(QColor(240, 240, 175));
    Rule.DPattern = QRegExp("\".*\"");
    Rule.DFormat = DQuoteFormat;
    DHighlightingRules.append(Rule);

    DFunctionFormat.setFontItalic(true);
    DFunctionFormat.setForeground(ZenGreen);
    Rule.DPattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    Rule.DFormat = DFunctionFormat;
    DHighlightingRules.append(Rule);

    DSingleCommentFormat.setForeground(QColor(111, 111, 111));
    Rule.DPattern = QRegExp("--[^\n]*");
    Rule.DFormat = DSingleCommentFormat;
    DHighlightingRules.append(Rule);

    DMultiCommentFormat.setForeground(QColor(111, 111, 111));

    DCommentStartExp = QRegExp("--\\[\\[");
    DCommentEndExp = QRegExp("\\]\\]");
}

/**
 * Highlights keywords in the given text block
 * @param text block of text to highlight
 */
void CLuaSyntaxHighlighter::highlightBlock(const QString& text)
{
    foreach (const HighlightRule& Rule, DHighlightingRules)
    {
        QRegExp Exp(Rule.DPattern);
        int Index = Exp.indexIn(text);
        while (Index >= 0)
        {
            int Length = Exp.matchedLength();
            setFormat(Index, Length, Rule.DFormat);
            Index = Exp.indexIn(text, Index + Length);
        }
    }

    setCurrentBlockState(NOT_IN_COMMENT);

    int StartIndex = DCommentStartExp.indexIn(text);
    if (StartIndex >= 0)
    {
        setCurrentBlockState(IN_COMMENT);
    }

    if (previousBlockState() == IN_COMMENT) {
        StartIndex = 0;
    }

    while (StartIndex >= 0)
    {
        int EndIndex = DCommentEndExp.indexIn(text, StartIndex);
        int CommentLength;
        if (EndIndex == -1)
        {
            setCurrentBlockState(IN_COMMENT);
            CommentLength = text.length() - StartIndex;
        }
        else
        {
            CommentLength = EndIndex - StartIndex + DCommentEndExp.matchedLength();
        }

        if (CommentLength == 0)
        {
            break;
        }

        setFormat(StartIndex, CommentLength, DMultiCommentFormat);
        StartIndex = DCommentStartExp.indexIn(text, StartIndex + CommentLength);
    }
}
