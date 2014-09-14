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
  * @file    doxygenbuilder.h
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    13-September-2014
  * @brief   CrossDocs build engine manager/master.
**/

#include "doxygenbuilder.h"

/**************************************** CONSTRUCTOR *******************************************/
doxygenBuilder::doxygenBuilder(QObject *parent) :
    doxygenPath("/usr/local/bin/doxygen")
{
    dir     = new QDir();
    process = new QProcess();
}


/**************************************** SLOTS *************************************************/

/**************************************** METHODS ***********************************************/

bool doxygenBuilder::buildDocument(documentWorker *doc, CDC_status *retStatus) {
    // Check for sanity...
    if(!doc->isOk()) {
        qDebug() << QString(__FUNCTION__) << "Invalid Document " << doc->getName();
        return false;
    }
    document = doc;
    if(!document->saveAllInputFiles(retStatus)) {
        qDebug() << QString(__FUNCTION__) << "Unable to save all Input Files to filesystem! Aborting!";
        return false;
    }

    cdcDoxyPath = getWorkFolder().append(document->getTag() + "_Doxyfile");

    // Creating fresh Doxyfile inside the Work folder
    QStringList args;
    args << "-s" << "-g" << cdcDoxyPath;
    qDebug() << "Generating " << args.last();
    process->start(doxygenPath, args, QIODevice::ReadOnly);
    process->waitForFinished();

    // Append the interesting stuff
    QFile outfile(getWorkFolder() + document->getTag() + ".dox");
    if(!outfile.open(QIODevice::ReadWrite | QIODevice::Truncate))
        return false;

    QTextStream stream(&outfile);
    stream << "/*!\n";

    QStringList iflist = document->getInputFilesList();
    for (int i = 0; i < iflist.length(); ++i)
        stream << document->getInputFileContents(i);

    stream << "\n**/\n";
    outfile.close();

    // Edit doxyfile to add the Input Files of the document
    QFile doxyfile(cdcDoxyPath);
    if(!doxyfile.open(QIODevice::ReadWrite))
        return false;
    QString doxyfileContents = doxyfile.readAll();

    // Other configuration fields
    doxyfileContents.append("\n# CrossDocs appended items:");
    doxyfileContents.append("\nINPUT += " + outfile.fileName());
    doxyfileContents.append("\nOUTPUT_DIRECTORY = "  + getWorkFolder() + document->getTag());
    doxyfileContents.append("\nGENERATE_TREEVIEW = YES\n");
    doxyfileContents.append("\nPROJECT_NAME = \"" + document->getName() + "\"\n");

    QTextStream in(&doxyfile);
    in << doxyfileContents;
    doxyfile.close();

    args.clear();
    args << cdcDoxyPath;

    process->start(doxygenPath, args, QIODevice::ReadOnly);
    process->waitForFinished();

    QString stdOut   = process->readAllStandardOutput();  //Reads standard output
    QString stdError = process->readAllStandardError();   //Reads standard error

    qDebug() << stdOut;

    //emit buildFinished(document->getTag(), getWorkFolder() + "/" + document->getTag() + "/hmtl/index.html");

    return true;
}
