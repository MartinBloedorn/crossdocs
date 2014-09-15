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
  * @file    abstractbuilder.h
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    02-September-2014
  * @brief   CrossDocs build engine manager/master.
**/

#ifndef BUILDWORKER_H
#define BUILDWORKER_H

#include <QObject>
#include <QProcess>

#include "cdcdefs.h"
#include "documentworker.h"

class abstractBuilder : public QObject
{
    Q_OBJECT
public:
    virtual ~abstractBuilder();

    virtual bool buildDocument(documentWorker * doc, CDC_status * retStatus = NULL) = 0;

    // Inheritable getters and setters
    void setWorkFolder(QString path) { workFolderPath = path; }
    QString getWorkFolder() { return workFolderPath; }

    void setProjectName(QString name) { projectName = name; }

    void setDocument(documentWorker * d) { doc = d; }
    documentWorker * document() { return doc; }

signals:
    void buildFinished(QString doctag, QString outfile);

public slots:

private:
    QString workFolderPath;
    QString projectName;

    documentWorker * doc;
};

#endif // BUILDWORKER_H
