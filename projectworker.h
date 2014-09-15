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
  * @file    projectworker.h
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    17-August-2014
  * @brief   CDC project functions: management, build engine, etc.
  *
  * \todo   Break front-end from back-end (CLI/GUI)
  * \todo   Add support to create cdd files in non-existing folders.
**/

#ifndef PROJECTWORKER_H
#define PROJECTWORKER_H

#include <iostream>

#include <QObject>
#include <QDebug>
#include <QProcess>
#include <QDir>
#include <QStandardItemModel>

#include "cdcdefs.h"

#include "configurationfileparser.h"
#include "documentworker.h"

#include "abstractbuilder.h"
#include "doxygenbuilder.h"

class projectWorker : public QObject
{
    Q_OBJECT
public:
    projectWorker(QObject *parent = 0);
    ~projectWorker();

    /**
     * @brief Runs configurationFileParser on prjconffile and builds internal project model.
     *
     * @param prjconffile \b QString containing the full path to the configuration file.
     * @param retStatus CDC_status of the operation.
     * @return Whether parse went ok (CDC_status::ok).
     */
    bool configureProject(QString prjconffile, CDC_status * retStatus = NULL);
    bool configureProject(QDir pcf, CDC_status * rSt = NULL) { return configureProject(pcf.absolutePath(), rSt); }

    bool build(QString doctag = "", CDC_status * retStatus = NULL);

    /**
     * @brief Returns the contents of a certain input file from a document.
     * @param doctag
     * @param ifIndex
     * @param retStatus
     * @return
     */
    QString getDocumentInputFileContents(QString doctag, int ifIndex, CDC_status * retStatus = NULL);

    /**
     * @brief Changes the contents of the input file from a document.
     * Also checks if this input file is referenced by more than one document, and updates all
     * references.
     * Check also documentWorker::getInputFileContents() .
     * @param doctag Tag of the desired document.
     * @param ifIndex Index of the desired input file in that particular document.
     * @param content
     * @param retStatus
     */
    void    setDocumentInputFileContents(QString doctag, int ifIndex, QString content, CDC_status *retStatus = NULL);

    bool saveDocumentInputFile(QString doctag, int ifIndex, CDC_status *retStatus = NULL);

    bool documentInputFileIsModified(QString doctag, int ifIndex);

    QString getDocumentCddFileContents(QString doctag);

    QString getDocumentOutputFolder(QString doctag) { return getDocumentbyTag(doctag)->getOutputPath(); }

    void    setDocumentInputFileSyntax(QString doctag, int ifIndex, CDC_fileSyntax syntax);
    CDC_fileSyntax getDocumentInputFileSyntax(QString doctag, int ifIndex);

    QStandardItemModel * getProjectStructure();

    QStringList getDocumentInputFilesList(QString doctag);
    QString     getDocumentName(QString doctag);

    // Getters for project structure
    QString getProjectName() { return project.name; }
    QStringList getDocumentTagList();

private:
    // Attribues
    QProcess * process;
    QString basePath;
    QStandardItemModel * structure;

    QString workFolderPath;

    configurationFileParser * fp;
    documentWorker  * docw;
    abstractBuilder * builder;

    /// \brief Struct that contains the current project's params. Only one prj at a time.
    struct {
        QString tag;
        QString name;
        QList<documentWorker *> documents;
        CDC_buildEngine buildEngine;
    } project;

    // Methods

    /**
     * @brief Configures all documents already in the project strucutre.
     * If one (or more) document fails to parse, the function will carry on parsing all the
     * other docs, but will return false nontheless.
     * @return Whether parse went ok. Return false if at least one document failed to parse.
     */
    bool configureAllDocuments();

    /**
     * @brief Returns the pointer to the project's document with the specified tag.
     * Internal function that is used as general document getter for all other indirect
     * document manipultions.
     * @param tag Tag of the desired document.
     * @return Pointer to the documentWorker. Returns empty documentWorker if no tag is found
     */
    documentWorker* getDocumentbyTag(QString tag);

signals:

public slots:

};

#endif // PROJECTWORKER_H
