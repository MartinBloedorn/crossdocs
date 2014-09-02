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
  * @date    17-August-2014
  * @brief   CDC project functions: management, build engine, etc.
**/

#include "projectworker.h"

// Sections for which the parser will look for
const QString confsecProjectTag  = "project";
const QString confsecDocuments   = "documents";
const QString confsecProjectName = "name";
const QString confsecBuildEngine = "build_engine";

/**************************************** CONSTRUCTOR *******************************************/
projectWorker::projectWorker(QObject *parent) :
    QObject(parent)
{
    process   = new QProcess;
    fp        = new configurationFileParser;
    structure = new QStandardItemModel();
}

projectWorker::~projectWorker() {
    for (int i = 0; i < project.documents.length(); ++i)
        delete project.documents[i];
    delete fp;
    delete process;
}

/**************************************** METHODS ***********************************************/

bool projectWorker::configureProject(QString prjconffile, CDC_status *retStatus) {
    if(fp->parseFile(prjconffile, retStatus)) {
        QDir dir(prjconffile);
        basePath = QString(prjconffile);
        basePath.chop(dir.dirName().length());
        //File parsed, now check for sections

        //Clear any old existing project defs
        project.documents.clear();
        project.name.clear();
        project.tag.clear();

        QStringList tempList;

        // Project tag -------------------------------------------
        tempList = fp->getSectionContents(confsecProjectTag);
        if(tempList.isEmpty()) {
            qWarning() << QString(__FUNCTION__) << "Undefined section "
                       << confsecProjectName << " aborting.";
            return false;
        }
        project.tag = tempList[0];
        project.name = tempList[0];

        // Project name (optional) --------------------------------
        tempList = fp->getSectionContents(confsecProjectName);
        if(tempList.isEmpty())
            qDebug() << QString(__FUNCTION__) << "Undefined section " << confsecProjectName;
        else
            project.name = tempList[0];

        // Build engine -------------------------------------------
        tempList = fp->getSectionContents(confsecBuildEngine);
        if(tempList.isEmpty()) {
            qWarning() << QString(__FUNCTION__) << "Undefined section "
                       << confsecBuildEngine << " Your documents should have it!";
            project.buildEngine = CDC_buildEngine::none;
        } else {
            if(tempList[0].compare(QString("doxygen")))
                project.buildEngine = CDC_buildEngine::doxygen;
            else if(tempList[0].compare(QString("custom")))
                project.buildEngine = CDC_buildEngine::custom;
        }

        // Documents -----------------------------------------------
        tempList = fp->getSectionContents(confsecDocuments);
        if(tempList.isEmpty()) {
            qWarning() << QString(__FUNCTION__) << "Undefined section "
                       << confsecDocuments << " aborting.";
            return false;
        }
        // Apend listed docs to project's definitions
        for(int i = 0; i < tempList.length(); ++i) {
           documentWorker * pd = new documentWorker();
           pd->setTag(tempList[i]); // Until the file is parsed, it receives a temp tag
           pd->setConfFilePath(basePath + tempList[i]);
           pd->setBuildEngine(project.buildEngine);
           project.documents.append(pd);
        }
        // Parse documents that were found. Has to be the last operation, since
        // fp will be cleansed internally!
        configureAllDocuments();

        // Verify any repeated document tag
        QStringList taglist;
        for(int i = 0; i < project.documents.length(); ++i)
            taglist.append(project.documents[i]->getTag());
        for(int i = 0; i < project.documents.length(); ++i)
            if(taglist.lastIndexOf(project.documents[i]->getTag()) != i) {
                qWarning() << QString(__FUNCTION__) << "Repeated document tag: "
                           << project.documents[i]->getTag() << " Aborting.";
                return false;
            }

        return true;
    }
    else
        return false;
}

QStandardItemModel * projectWorker::getProjectStructure() {
    structure->clear();
    // Append the strcuture of each document in the project
    for (int i = 0; i < project.documents.length(); ++i)
        if(project.documents[i]->getDocumentStructure()->columnCount() > 0)
            structure->invisibleRootItem()->appendRow(project.documents[i]->getDocumentStructure()->takeColumn(0));
    structure->setHorizontalHeaderLabels(QStringList() << project.name);
    return structure;
}

bool projectWorker::build(QString prjconffile, CDC_status *retStatus) {
    QString cmd("/usr/local/bin/doxygen");
    QStringList args;

    args << "-g";

    process->start(cmd, args, QIODevice::ReadOnly);
    process->waitForFinished();

    QString StdOut   = process->readAllStandardOutput();  //Reads standard output
    QString StdError = process->readAllStandardError();   //Reads standard error

    std::cout << StdOut.toStdString() << std::endl;

    return true;
}

QString projectWorker::getDocumentInputFileContents(QString doctag, int ifIndex, CDC_status *retStatus) {
    return getDocumentbyTag(doctag)->getInputFileContents(ifIndex, retStatus);
}

void projectWorker::setDocumentInputFileContents(QString doctag, int ifIndex, QString content, CDC_status *retStatus) {
    //check for multiple references for the same input file
    QStringList iflist = getDocumentbyTag(doctag)->getInputFilesList();
    if(iflist.length() > ifIndex && iflist.length() > 0) {
        QString ifname = iflist[ifIndex];
        int index;
        for (int i = 0; i < project.documents.length(); ++i) {
            index = project.documents[i]->getInputFilesList().lastIndexOf(ifname);
            if(index != -1)
                project.documents[i]->setInputFileContents(index, content, retStatus);
        }
    }
}

void projectWorker::setDocumentInputFileSyntax(QString doctag, int ifIndex, CDC_fileSyntax syntax) {
    getDocumentbyTag(doctag)->setInputFileSyntax(ifIndex, syntax);
}

CDC_fileSyntax projectWorker::getDocumentInputFileSyntax(QString doctag, int ifIndex) {
    return getDocumentbyTag(doctag)->getInputFileSyntax(ifIndex);
}

QStringList projectWorker::getDocumentInputFilesList(QString doctag) {
    return getDocumentbyTag(doctag)->getInputFilesList();
}

QString projectWorker::getDocumentName(QString doctag) {
    return getDocumentbyTag(doctag)->getName();
}

QStringList projectWorker::getDocumentTagList() {
    QStringList retlist;
    for (int i = 0; i < project.documents.length(); ++i) {
        retlist.append(project.documents[i]->getTag());
    }
    return retlist;
}


// PRIVATE ------------------------------------------------------------------------
bool projectWorker::configureAllDocuments() {
    bool retval = true;
    for(int i = 0; i < project.documents.length(); ++i)
        if(!project.documents[i]->configureDocument())
            retval = false;
    return retval;
}

documentWorker* projectWorker::getDocumentbyTag(QString tag) {
    for (int i = 0; i < project.documents.length(); ++i)
        if(project.documents[i]->getTag().compare(tag) == 0)
           return project.documents[i];

    return new documentWorker; // returns empty documentWorker if no tag is found
}

