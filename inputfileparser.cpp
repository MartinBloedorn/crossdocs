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

#include "inputfileparser.h"

/**************************************** CONSTRUCTOR *******************************************/
inputFileParser::inputFileParser(QObject *parent) :
    QObject(parent),
    structure(new QStandardItemModel)
{
}

inputFileParser::~inputFileParser()
{
}

/**************************************** SLOTS *************************************************/

/**************************************** METHODS ***********************************************/

bool inputFileParser::parseInputFile(QFile *ifile) {
    currentInputFile = ifile;
    if(!currentInputFile->open(QFile::ReadOnly | QFile::Text))
        return false;
    QTextStream in(currentInputFile);
    currentIFContents = in.readAll();
    currentInputFile->close();
    return parseInputFile();
}

bool inputFileParser::parseInputFile(QString ifcontents) {
    if(!ifcontents.isEmpty())
        currentIFContents = ifcontents;

    bool retval = false;
    QTextStream stream(&currentIFContents);
    QString line;
    CDC_pStates state = CDC_pStates::idle;

    int index;
    int currLine = 0;

    structure->clear();
    structure->setColumnCount(1);
    structureList.clear();

    // Run parsing FSM
    // For doxyen, only the first occurrance of a structural tag in a line is taken into account
    // Also, one cannot jump directly into a daughter element without its parent
    while(!stream.atEnd()) {
        currLine++;
        line = stream.readLine();

        // The lack of 'break' statements is on purpose
        switch (state) {
        case CDC_pStates::at_paragraph:
        case CDC_pStates::at_subsubsection:
            if(applySectionRuleOnLine(CDC_docStructuralElementType::paragraph,line,currLine)) {
                state = CDC_pStates::at_paragraph;
                break;
            }

        case CDC_pStates::at_subsection:
            index = syntaxRules.subsubsection.indexIn(line);
            if(applySectionRuleOnLine(CDC_docStructuralElementType::subsubsection,line,currLine)) {
                state = CDC_pStates::at_subsubsection;
                break;
            }

        case CDC_pStates::at_section:
            if(applySectionRuleOnLine(CDC_docStructuralElementType::subsection,line,currLine)) {
                state = CDC_pStates::at_subsection;
                break;
            }

        case CDC_pStates::idle:
            if(applySectionRuleOnLine(CDC_docStructuralElementType::section,line,currLine)) {
                state = CDC_pStates::at_section;
                retval = true; break;
            }

        default:
            break;
        }
    }
    return retval;
}

void inputFileParser::setSyntax(CDC_fileSyntax fsyntax) {
    switch (fsyntax) {
    case CDC_fileSyntax::doxygen:
            syntax = fsyntax;
            syntaxRules.section.setPattern("(\\\\section)(\\s+)(\\w+)(\\s+)(.*)");
            syntaxRules.subsection.setPattern("(\\\\subsection)(\\s+)(\\w+)(\\s+)(.*)");
            syntaxRules.subsubsection.setPattern("(\\\\subsubsection)(\\s+)(\\w+)(\\s+)(.*)");
            syntaxRules.paragraph.setPattern("(\\\\paragraph)(\\s+)(\\w+)(\\s+)(.*)");
            syntaxRules.tagGroup  = 3;
            syntaxRules.nameGroup = 5;
        break;
    case CDC_fileSyntax::none:
    default:
            syntax = CDC_fileSyntax::none;
        break;
    }
}

bool inputFileParser::applySectionRuleOnLine(CDC_docStructuralElementType type, QString line, int currLine) {
    QRegExp regexp;
    QString tooltip;
    int depth;
    CDC_docStructuralElement currSE;
    QStandardItem *  newItem;
    QStandardItem *  parentItem;

    switch (type) {
    case CDC_docStructuralElementType::section:
            regexp = syntaxRules.section;
            tooltip = "Section tag: ";
            depth = 0;
        break;
    case CDC_docStructuralElementType::subsection:
            regexp = syntaxRules.subsection;
            tooltip = "Subsection tag: ";
            depth = 1;
        break;
    case CDC_docStructuralElementType::subsubsection:
            regexp = syntaxRules.subsubsection;
            tooltip = "Subsubsection tag: ";
            depth = 2;
        break;
    case CDC_docStructuralElementType::paragraph:
            regexp = syntaxRules.paragraph;
            tooltip = "Paragraph tag: ";
            depth = 3;
        break;
    default:
        return false;
        break;
    }

    int index = regexp.indexIn(line);
    if(index >= 0) {
        currSE.tag  = regexp.cap(syntaxRules.tagGroup );
        currSE.name = regexp.cap(syntaxRules.nameGroup);

        newItem = new QStandardItem(currSE.name);
        newItem->setData(QVariant(currSE.tag), CDC_docStructuralElementRole::Tag );
        newItem->setData(QVariant(currLine),   CDC_docStructuralElementRole::Line);
        newItem->setData(QVariant(static_cast<int>(type)), CDC_docStructuralElementRole::Type);
        newItem->setData(QVariant(parentDocumentTag),      CDC_docStructuralElementRole::Doc);
        newItem->setData(QVariant(indexOfCurrentFile),     CDC_docStructuralElementRole::Index);
        newItem->setEditable(false);
        newItem->setToolTip(tooltip + currSE.tag);

        parentItem = structure->invisibleRootItem();
        while(depth) {
            if(parentItem->hasChildren())
                parentItem = parentItem->child(parentItem->rowCount()-1);
            else {
                qDebug() << QString(__FUNCTION__)
                         << "Unable to append structural element" << currSE.tag << "to document's struture!";
                return false;
            }
            depth--;
        }
        parentItem->appendRow(newItem);
        //std::cout << "Found " << QString(tooltip + currSE.tag).toStdString() << std::endl;
        return true;
    }
    else return false;
}
