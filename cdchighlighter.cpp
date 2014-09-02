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
  * @file    cdchighlighter.cpp
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    31-August-2014
  * @brief   Syntax highlighting engine for CDC GUI.
**/

#include "cdchighlighter.h"

/**************************************** CONSTRUCTOR *******************************************/
cdcHighlighter::cdcHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent),
    currentSyntax(CDC_fileSyntax::none)
{
    setSyntax(CDC_fileSyntax::doxygen); // just defaulting
}

cdcHighlighter::~cdcHighlighter() {

}

/**************************************** SLOTS *************************************************/

/**************************************** METHODS ***********************************************/
void cdcHighlighter::setSyntax(CDC_fileSyntax syntax) {
    structuralRules.clear();
    structuralRule strucRule;

    switch (syntax) {
    case CDC_fileSyntax::doxygen:
        currentSyntax = syntax;
        strucRule.SEGroup   = 1;
        strucRule.tagGroup  = 3;
        strucRule.nameGroup = 5;
        strucRule.SEtagPaddingGroup   = 2;
        strucRule.tagNamePaddingGroup = 4;
        strucRule.SEFormat.setForeground(Qt::darkBlue);
        strucRule.SEFormat.setFontWeight(QFont::Bold);
        strucRule.tagFormat.setForeground(Qt::darkGreen);
        strucRule.tagFormat.setFontWeight(QFont::Bold);
        strucRule.nameFormat.setForeground(Qt::darkBlue);
        strucRule.regexp = QRegExp("(\\\\section)(\\s+)(\\w+)(\\s+)(.*)"); // Section rule
        structuralRules.append(strucRule);
        strucRule.regexp = QRegExp("(\\\\subsection)(\\s+)(\\w+)(\\s+)(.*)"); // Subsection rule
        structuralRules.append(strucRule);
        strucRule.regexp = QRegExp("(\\\\subsubsection)(\\s+)(\\w+)(\\s+)(.*)"); // Subsubsection rule
        structuralRules.append(strucRule);
        strucRule.regexp = QRegExp("(\\\\paragraph)(\\s+)(\\w+)(\\s+)(.*)"); // Paragraph rule
        structuralRules.append(strucRule);
        break;
    case CDC_fileSyntax::none:
    default:
            syntax = CDC_fileSyntax::none;
        break;
    }
}

void cdcHighlighter::highlightBlock(const QString &text) {

    setCurrentBlockState(0);

    foreach (const structuralRule &rule, structuralRules) {
       int index = rule.regexp.indexIn(text);
       if(index >= 0) {
           setFormat(index, rule.regexp.cap(rule.SEGroup).length(), rule.SEFormat);
           index += rule.regexp.cap(rule.SEGroup).length() + rule.regexp.cap(rule.SEtagPaddingGroup).length();
           setFormat(index, rule.regexp.cap(rule.tagGroup).length(), rule.tagFormat);
           index += rule.regexp.cap(rule.tagGroup).length() + rule.regexp.cap(rule.tagNamePaddingGroup).length();
           setFormat(index, rule.regexp.cap(rule.nameGroup).length(), rule.nameFormat);
       }
    }
}
