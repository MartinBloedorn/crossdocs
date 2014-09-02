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
  * @file    projectworker.cpp
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    18-August-2014
  * @brief   CDC document functions: management, build engine, etc.
**/

#ifndef DOCUMENTWORKER_H
#define DOCUMENTWORKER_H

#include <QObject>
#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QStandardItemModel>

#include "cdcdefs.h"
#include "configurationfileparser.h"
#include "inputfileparser.h"

class documentWorker : public QObject
{
    Q_OBJECT
public:
    documentWorker(QObject *parent = 0);
    ~documentWorker();

    bool configureDocument(QString docConfPath = "", CDC_status *retStatus = NULL);

    /**
     * @brief Reads contents of file into inputFileContents and the returns them.
     * Reads the file once from the filesystem into application, then return the struct.
     * @param index Index of the file in the internal inputFiles structure.
     * @return String containing the whole plain-text file.
     */
    QString getInputFileContents(int index, CDC_status * retStatus = NULL);

    /**
     * @brief Sets the n-th inputFileContents element to the given \em content argument.
     * The contents of the n-th input file are altered and the n-th \em modified flag is set.
     * @param index Index of the document to be changed (i.e., element in inputFileContents)
     * @param content QString containing the contents of the file.
     */
    void setInputFileContents(int index, QString content, CDC_status * retStatus = NULL);

    /**
     * @brief Saves the content of the n-th inputFileContents to the file.
     * @param index Index of the file to be saved.
     * @param retStatus Status of the operation.
     * @return Whether the operation succeded.
     */
    bool saveInputFile(int index, CDC_status * retStatus = NULL);

    void setInputFileSyntax(int index, CDC_fileSyntax syntax);
    CDC_fileSyntax getInputFileSyntax(int index);

    /**
     * @brief Returns a string list containing the names of all input files.
     * @return QStringList with full paths of all input files. Empty list if smth went wrong.
     */
    QStringList getInputFilesList();

    /**
     * @brief isModified
     * @param index
     * @return TRUE if the file at <index> was modified through setInputFileContents() .
     */
    bool isModified(int index);

    /**
     * @brief Creates and returns the document's structure (sections, subsections, etc.)
     * Uses the inputFileParser::parseInputFile() method to generate the structure for each
     * input file. Applies this sequentially to each input file and then stitches those structures
     * together, which generates the structure of the "bigger picture".
     * Simultaneously updates the internal structural element list, that is later used for
     * line number and index information checking.
     * @attention This whole thing is based on the idea that ALL stuctural tags in a document are different.
     * Violating this will generate unexpected behaviors!
     * @return The \b QStandardItemModel object containing the representation of the document's structure.
     */
    QStandardItemModel * getDocumentStructure();
    int getLineOfStructuralElement(QString elemtag);
    int getIndexOfStructuralElement(QString elemtag);


    // Getters and setters
    QString getTag() { return tag; }
    void setTag(QString value) { tag = value; }
    QString getName() { return name; }
    void setName(QString value) { name = value; }
    QString getConfFilePath() { return cddFilePath; }
    void setConfFilePath(QString value) { cddFilePath = value; }
    CDC_buildEngine getBuildEngine() { return buildEngine; }
    void setBuildEngine(CDC_buildEngine value) { buildEngine = value; }

private:
    QString tag;
    QString name;
    QString cddFilePath;
    CDC_buildEngine buildEngine;

    QString basePath;   /// Relative to the cdd file.
    QFile * cddFile;    /// Configuration file for document.

    typedef struct {
        QFile * file;                /// Stores QFile of n-th input file
        QString contents;            /// Stores loaded plain-text contents of n-th file
        bool modified;               /// Stores TRUE if file the n-th file was modified and not saved
        CDC_buildEngine buildEngine; /// Input file's build engine. Defaults to document's setting .
        CDC_fileSyntax syntax;       /// Syntax analysis and highlighting
    } CDC_inputFile;

    QList<CDC_inputFile> inputFiles;
    QList<CDC_docStructuralElement> structureList;
    QStandardItemModel * structure;

    configurationFileParser * fp;
    inputFileParser * infp;

signals:

public slots:

};

#endif // DOCUMENTWORKER_H
