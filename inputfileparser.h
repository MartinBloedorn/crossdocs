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
  * @file    inputfileparser.cpp
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    24-August-2014
  * @brief   Analyses input file for its sections, subsections, etc.
**/


#ifndef INPUTFILEPARSER_H
#define INPUTFILEPARSER_H

#include <iostream>

#include <QObject>
#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QStandardItemModel>

#include "cdcdefs.h"

class inputFileParser : public QObject
{
    Q_OBJECT
public:
    inputFileParser(QObject *parent = 0);
    ~inputFileParser();

    /**
     * @brief parseInputFile
     * @param ifcontents
     * @return Whether any kind of structural element was found in the analysed file.
     */
    bool parseInputFile(QString ifcontents = QString::QString(""));

    /**
     * @brief parseInputFile
     * @param ifile
     * @return The result of parseInputFile() on the \b QString contents of the file.
     */
    bool parseInputFile(QFile * ifile);

    void setSyntax(CDC_fileSyntax fsyntax);
    CDC_fileSyntax getSyntax() { return syntax; }

    QFile *getCurrentInputFile()           { return currentInputFile;  }
    void setCurrentInputFile(QFile *value) { currentInputFile = value; }

    QStandardItemModel * getStructure() { return structure; }

    QList<CDC_docStructuralElement> getStructureList() { return structureList; }

    QString getParentDocumentTag()                  { return parentDocumentTag;  }
    void setParentDocumentTag(const QString &value) { parentDocumentTag = value; }

    int  getIndexOfCurrentFile()                 { return indexOfCurrentFile;  }
    void setIndexOfCurrentFile(const int &value) { indexOfCurrentFile = value; }

private:
    QStandardItemModel * structure; /// Structure of the input file (sections, subsecions...)

    QList<CDC_docStructuralElement> structureList;

    CDC_buildEngine buidEngine;
    CDC_fileSyntax  syntax;

    struct {
        QRegExp section;
        QRegExp subsection;
        QRegExp subsubsection;
        QRegExp paragraph;
        int tagGroup;
        int nameGroup;
    } syntaxRules;

    QFile * currentInputFile;  /// Handle to the current input file
    QString currentIFContents; /// Current active input file's contents
    QString parentDocumentTag; /// Tag of the parent document of the file being parsed
    int indexOfCurrentFile;    /// Holds the index of the curr IF in the parent document's IFlist

    /// States for the parsing state machine
    enum class CDC_pStates {
        idle,               //! Didn't find anything
        at_section,         //! Found a section header. Will append subsections to it, if any
        at_subsection,      //! Found a subsection header. Will append subsubsections to it, if any
        at_subsubsection,   //! Found a subsubsection header. Will append paragraphs to it, if any
        at_paragraph        //! Found a paragraph header
    };

    // Methods

    /**
     * @brief applySectionRuleOnLine
     * @param type
     * @param line
     * @return If the desired section was found in that particular line.
     */
    bool applySectionRuleOnLine(CDC_docStructuralElementType type, QString line, int currLine);

signals:

public slots:


};

#endif // INPUTFILEPARSER_H
