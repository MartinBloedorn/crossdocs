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
  * @file    buildworker.h
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    02-September-2014
  * @brief   CrossDocs build engine manager/master.
**/

#ifndef BUILDWORKER_H
#define BUILDWORKER_H

#include <QObject>

#include "cdcdefs.h"
#include "documentworker.h"

class buildWorker : public QObject
{
    Q_OBJECT
public:
    buildWorker(QObject *parent = 0);
    ~buildWorker();

    bool buildDocument(documentWorker * doc, CDC_status * retStatus = NULL);

signals:

public slots:

private:
    documentWorker * document;
};

#endif // BUILDWORKER_H
