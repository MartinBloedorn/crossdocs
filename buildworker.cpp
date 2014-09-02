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
  * @file    buildworker.cpp
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    02-September-2014
  * @brief   CrossDocs build engine manager/master.
**/

#include "buildworker.h"

/**************************************** CONSTRUCTOR *******************************************/
buildWorker::buildWorker(QObject *parent) :
    QObject(parent)
{

}

buildWorker::~buildWorker() {

}

/**************************************** SLOTS *************************************************/

/**************************************** METHODS ***********************************************/

bool buildWorker::buildDocument(documentWorker *doc, CDC_status *retStatus) {
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
}
