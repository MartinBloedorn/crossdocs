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
  * @file    documentworker.cpp
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    18-August-2014
  * @brief   CDC document functions: management, build engine, etc.
**/

#include "documentworker.h"

// Sections for which the parser will look for
const QString docsecTag            = "document";
const QString docsecName           = "name";
const QString docsecBuild          = "build_engine";
const QString docsecInputFiles     = "input_files";
const QString docsecOutdir         = "output_directory";
const CDC_fileSyntax defaultSyntax = CDC_fileSyntax::doxygen;


/**************************************** CONSTRUCTOR *******************************************/
documentWorker::documentWorker(QObject *parent) :
    QObject(parent),
    buildEngine(CDC_buildEngine::none),
    parseOk(false)
{
    fp        = new configurationFileParser();
    infp      = new inputFileParser();
    structure = new QStandardItemModel();
}

documentWorker::~documentWorker() {
    delete fp;
    delete infp;
    cddFile->close();
    for (int i = 0; i < inputFiles.length(); ++i)
        inputFiles[i].file->close();
}

/**************************************** SLOTS *************************************************/

/**************************************** METHODS ***********************************************/
bool documentWorker::configureDocument(QString docConfPath, CDC_status *retStatus) {
    if(!docConfPath.isEmpty())
        cddFilePath = docConfPath;

    cddFile = new QFile(cddFilePath);
    // Clean everything that might be from other parse
    inputFiles.clear();
    parseOk = false;

    QDir dir(cddFilePath);
    basePath = QString(cddFilePath);
    basePath.chop(dir.dirName().length());

    // If cdd file does not exist, create file with that tag.
    if(!cddFile->exists()) {
        qDebug() << QString(__FUNCTION__) << "Creating: " << cddFilePath;
        CDC_confSection doc;
        doc.header = "document";
        doc.contentsList.append(QString(tag));
        CDC_confList list;
        list.append(doc);

        if(!fp->saveToConfigurationFile(cddFilePath, list)) {
            qWarning() << QString(__FUNCTION__) << "Failed to create " << tag;
            return false;
        }
        else
            return true; //Document was created, no point in parsing it again
    }
    // Cdd file exists already, parse it
    if(!fp->parseFile(cddFilePath)) {
        qWarning() << QString(__FUNCTION__) << "Failed to parse " << cddFilePath;
        return false;
    }
    // Get document tag (obligatory section)
    QStringList tempTag = fp->getSectionContents(docsecTag);
    if(tempTag.isEmpty()) {
        qWarning() << QString(__FUNCTION__) << "No " << docsecTag << " section in "
                   << cddFilePath << ". Aborting!";
        return false;
    }
    tag = tempTag[0];

    // Get output directory for document (optional)
    tempTag = fp->getSectionContents(docsecOutdir);
    if(tempTag.isEmpty()) {
        qWarning() << QString(__FUNCTION__) << "No " << docsecOutdir << " section in " << cddFilePath;
        outputPath = basePath;
    }
    else
        outputPath = tempTag[0];

    // Get build engine for document (optional)

    // Get input files
    tempTag = fp->getSectionContents(docsecInputFiles);
    if(tempTag.isEmpty())
        qWarning() << QString(__FUNCTION__) << "No input files for " << cddFilePath;
    else {
        // check if the input documents exist and stash them
        for (int i = 0; i < tempTag.length(); ++i) {
            dir.setPath(tempTag[i]);
            QFile * ifile;

            if(dir.isAbsolute())
                ifile = new QFile(tempTag[i]);
            else
                ifile = new QFile(basePath + tempTag[i]);

            if(!ifile->exists())
                qWarning() << QString(__FUNCTION__) << "In " << tag
                           << ": input file doesnt exist: "<< tempTag[i] << " Will be ignored!";
            else {
                qDebug() << QString(__FUNCTION__) << "Found input file"<< tempTag[i];
                // All ok! Appending new input file to document's list
                CDC_inputFile newInputFile;
                QFileInfo finfo(*ifile);
                ifile->setFileName(finfo.canonicalFilePath());
                newInputFile.file = ifile;
                newInputFile.modified = false;
                newInputFile.contents = QString("");
                newInputFile.buildEngine = buildEngine;
                newInputFile.syntax = defaultSyntax;
                inputFiles.append(newInputFile);
            }
        }
    }

    // get name. If non-existing, set name = tag
    tempTag = fp->getSectionContents(docsecName);
    if(tempTag.isEmpty()) {
        qWarning() << QString(__FUNCTION__) << "No " << docsecName << " section in " << cddFilePath;
        name = tag;
    }
    else
        name = tempTag[0];

    parseOk = true;
    return true;
}

QString documentWorker::getInputFileContents(int index, CDC_status * retStatus) {
    if(retStatus != NULL) *retStatus = CDC_status::ok;

    if(!(index < inputFiles.length() && index >= 0)) {
        qWarning() << QString(__FUNCTION__) << "Invalid index";
        if(retStatus != NULL) *retStatus = CDC_status::paramError;
        return "";
    }
    if(inputFiles[index].contents.isEmpty()) {
        if(!inputFiles[index].file->isOpen())
            if(!inputFiles[index].file->open(QIODevice::ReadWrite)) {
                qWarning() << QString(__FUNCTION__) << "Unable to load file " << inputFiles[index].file->fileName();
                if(retStatus != NULL) *retStatus = CDC_status::ioError;
                return "";
            }
        QTextStream txstream(inputFiles[index].file);
        inputFiles[index].contents = txstream.readAll();
        inputFiles[index].file->close();
    }
    return inputFiles[index].contents;
}

void documentWorker::setInputFileContents(int index, QString content, CDC_status * retStatus) {
    if(retStatus != NULL) *retStatus = CDC_status::ok;
    if(!(index < inputFiles.length() && index >= 0)) {
        qWarning() << QString(__FUNCTION__) << "Invalid index";
        if(retStatus != NULL) *retStatus = CDC_status::paramError;
        return;
    }
    if(inputFiles[index].contents.compare(content) != 0) {
        inputFiles[index].contents = content;
        inputFiles[index].modified = true;
    }
}

bool documentWorker::saveInputFile(int index, CDC_status * retStatus) {
    if(retStatus != NULL) *retStatus = CDC_status::ok;
    if(!(index < inputFiles.length() && index >= 0)) {
        qWarning() << QString(__FUNCTION__) << "Invalid index";
        *retStatus = CDC_status::paramError;
        return false;
    }
    if(inputFiles[index].modified) { // don't bother saving file if content wasn't changed
        if(!inputFiles[index].file->open(QIODevice::WriteOnly)) {
            qWarning() << QString(__FUNCTION__) << "Unble to save to file " << inputFiles[index].file->fileName();
            if(retStatus != NULL) *retStatus = CDC_status::ioError;
            return false;
        }
        QTextStream in(inputFiles[index].file);
        in << inputFiles[index].contents;
        inputFiles[index].file->close();
        inputFiles[index].modified = false;
    }
    return true;
}

bool documentWorker::saveAllInputFiles(CDC_status *retStatus) {
    for(int i = 0; i < inputFiles.length(); ++i)
        if(!saveInputFile(i, retStatus))
            return false;
    return true;
}

QString documentWorker::getCddFileContents() {
    if(!cddFile->open(QIODevice::ReadOnly))
        return "";
    QString retcont = cddFile->readAll();
    cddFile->close();
    return retcont;
}

bool documentWorker::saveCddFileContents(QString contents, CDC_status *retStatus) {
    return true;
}

void documentWorker::setInputFileSyntax(int index, CDC_fileSyntax syntax) {
    if(!(index < inputFiles.length() && index >= 0)) {
        qWarning() << QString(__FUNCTION__) << "Invalid index";
        return;
    }
    inputFiles[index].syntax = syntax;
}

CDC_fileSyntax documentWorker::getInputFileSyntax(int index) {
    if(!(index < inputFiles.length() && index >= 0)) {
        qWarning() << QString(__FUNCTION__) << "Invalid index";
        return CDC_fileSyntax::none;
    }
    return inputFiles[index].syntax;
}

QStringList documentWorker::getInputFilesList() {
    QStringList retList;
    for (int i = 0; i < inputFiles.length(); ++i)
        retList.append(inputFiles[i].file->fileName());
    return retList;
}

bool documentWorker::isModified(int index) {
    if(!(index < inputFiles.length() && index >= 0)) {
        qWarning() << QString(__FUNCTION__) << "Invalid index";
        return false;
    }
    return inputFiles[index].modified;
}

QStandardItemModel * documentWorker::getDocumentStructure() {
    structure->clear();

    // Append root item that refers to the document's self
    QStandardItem * rootItem = new QStandardItem(name);
    rootItem->setToolTip("Document tag: " + tag);
    rootItem->setData(QVariant(tag), CDC_docStructuralElementRole::Tag );
    rootItem->setData(QVariant(tag), CDC_docStructuralElementRole::Doc ); // Is his own parent
    rootItem->setData(QVariant(-1),  CDC_docStructuralElementRole::Line);
    rootItem->setData(QVariant(static_cast<int>(CDC_docStructuralElementType::document)),
                     CDC_docStructuralElementRole::Type);
    QFont ifont = rootItem->font(); ifont.setBold(true);
    rootItem->setFont(ifont);
    rootItem->setEditable(false);

    infp->setParentDocumentTag(tag);
    for (int i = 0; i < inputFiles.length(); ++i) {
        infp->setSyntax(inputFiles[i].syntax);
        infp->setIndexOfCurrentFile(i);
        if(infp->parseInputFile(getInputFileContents(i))) {
            // This transfers ownership of the underlying model elemnts, so that they
            // don't get deleted by its original parent
            if(infp->getStructure()->columnCount() > 0) {
                if(!(rootItem->columnCount() > 0))
                    rootItem->appendColumn(infp->getStructure()->takeColumn(0));
                else
                    rootItem->appendColumn(rootItem->takeColumn(0) << infp->getStructure()->takeColumn(0));
            }
        }
    }

    structure->appendRow(rootItem);
    return structure;
}

int documentWorker::getLineOfStructuralElement(QString elemtag) {
    int retval = -1;
    for(int i = 0; i < structureList.length(); ++i) {
        if(structureList[i].tag.compare(elemtag) == 0)
            retval = structureList[i].line;
    }
    return retval;
}

int documentWorker::getIndexOfStructuralElement(QString elemtag) {
    int retval = -1;
    for(int i = 0; i < structureList.length(); ++i) {
        if(structureList[i].tag.compare(elemtag) == 0)
            retval = structureList[i].index;
    }
    return retval;
}

