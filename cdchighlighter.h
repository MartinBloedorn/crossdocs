/**
  *****************************************************************************
  *  _____                  ______
  * /  __ \                 |  _  \
  * | /  \/_ __ ___  ___ ___| | | |___   ___ ___
  * | |   | '__/ _ \/ __/ __| | | / _ \ / __/ __|
  * | \__/\ | | |_| \__ \__ \ |/ / |_| | (__\__ \
  *  \____/_|  \___/|___/___/___/ \___/ \___|___/
  *
  *****************************************************************************
  *
  * @file    cdchighlighter.h
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    31-August-2014
  * @brief   Syntax highlighting engine for CDC GUI.
**/

#ifndef CDCHIGHLIGHTER_H
#define CDCHIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>

#include "cdcdefs.h"

class cdcHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    cdcHighlighter(QTextDocument *parent = 0);
    ~cdcHighlighter();

    void setSyntax(CDC_fileSyntax syntax);

protected:
    void highlightBlock(const QString &text);

private:
    CDC_fileSyntax currentSyntax;

    /** Strucutre that contains the information to highlight the structural elements of a text in each syntax.
     *  Let's assume a \em doxygen syntax example. The regexp used for detecting a \b section keyword is:
     *  \code
     *      (\\\\section)(\\s+)(\\w+)(\\s+)(.*)
     *  \endcode
     *  The regexp divides the search in five groups:
     *      + \b 1: Has the \b section keyword stored. The SEFormat will be applied to it.
     *      + \b 2: Has all the whitespaces between the \b SE command and the SE tag. No format is applied, it is used to offset the next format accordingly.
     *      + \b 3: Has the \b tag. The tagFormat will be applied to it.
     *      + \b 4: Has all the whitespaces between the \b SE tag and the SE name. No format is applied, it is used to offset the next format accordingly.
     *      + \b 3: Has the \b name. The nameFormat will be applied to it.
     *  That group numeration is stored in all the integers in this struct. If it doesnt apply for any given syntax,
     *  just set it to -1.
     */
    struct structuralRule {
        QRegExp regexp;
        QTextCharFormat SEFormat;
        QTextCharFormat tagFormat;
        QTextCharFormat nameFormat;
        int SEGroup;
        int tagGroup;
        int nameGroup;
        int SEtagPaddingGroup;   /// Index of the group in the regexp that captures the whitespaces between the SE and tag
        int tagNamePaddingGroup; /// Index of the group in the regexp that captures the whitespaces between the tag and name
    };
    QList<structuralRule> structuralRules;

    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

};

#endif // CDCHIGHLIGHTER_H
