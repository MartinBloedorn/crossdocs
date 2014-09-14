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

#ifndef DOXYGENBUILDER_H
#define DOXYGENBUILDER_H

#include "abstractbuilder.h"

/**
 * @brief The doxygenBuilder class uses an external \p doxygen parser to generate docs.
 *
 * This class takes a already supplied \b Doxyfile, than looks for a Doxyfile in the project folder,
 * and ultimately creates one if not available, via \p doxygen \p -g .
 * It then copies that file to the hidden project folder, where an edited version of the Doxyfile is
 * created. This edited version contains links to the edited and translated Input Files (with resolved
 * CDC links and resources). The doxygen parser is than invoqued on this edited Doxyfile.
 *
 */
class doxygenBuilder : public abstractBuilder
{
public:
    doxygenBuilder(QObject *parent = 0);

    bool buildDocument(documentWorker *doc, CDC_status *retStatus);

    void setDoxygenPath(QString path) { doxygenPath = path; }

signals:

public slots:

private:
    documentWorker * document;
    QProcess * process;
    QDir * dir;

    QString doxygenPath;      /// Path to doxygen executable (`which doxygen`)
    QString originalDoxyPath; /// Path to the original doxyfile
    QString cdcDoxyPath;      /// Path to the CDC-created doxyfile
};

#endif // DOXYGENBUILDER_H
