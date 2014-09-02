/**
  ******************************************************************************
  *  _____                  ______
  * /  __ \                 |  _  \
  * | /  \/_ __ ___  ___ ___| | | |___   ___ ___
  * | |   | '__/ _ \/ __/ __| | | / _ \ / __/ __|
  * | \__/\ | | |_| \__ \__ \ |/ / |_| | (__\__ \
  *  \____/_|  \___/|___/___/___/ \___/ \___|___/
  *
  ******************************************************************************
  *
  * @file    configurationfileparser.cpp
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    03-August-2014
  * @brief   Definitions for *.cdc file parsing.
**/

#include "configurationfileparser.h"

/* Local Types --------------------------------------------------------------*/

enum class CDC_cState {idle, at_header, at_argument, fail};

/* Constructor & Destructor -------------------------------------------------*/

configurationFileParser::configurationFileParser() {}

configurationFileParser::~configurationFileParser() {}

/* Public methods -----------------------------------------------------------*/

bool configurationFileParser::parseFile(QString fpath, CDC_status * retStatus) {
    // Preamble --------------------
    bool retval = true;
    bool skip = false;
    int currLine = 0;
    QString line;
    QStringList lsplit;
    CDC_cState state = CDC_cState::idle;

    // Clear previous contents, that might be related to other file.
    confFileSections.clear();

    // Body ------------------------

    // Open file
    file = new QFile(fpath);
    if(!file->open(QIODevice::ReadOnly)) {
        qDebug() << QString(__FUNCTION__) << " Could not open " << fpath;
        retval = false;
        if(retStatus != NULL)
            *retStatus = CDC_status::ioError;
    }

    // Text stream
    fileIn = new QTextStream(file);

    /// Finite state machine
    while(!fileIn->atEnd()) {
        currLine++;
        line = fileIn->readLine();
        lsplit = line.split(" ", QString::SkipEmptyParts);

        // Check state transition, according to current state
        if(state == CDC_cState::idle && !skip) {
            state = CDC_cState::fail; // defaults to 'fail' if no transition below happens

            if(lsplit.length() == 0) //empty line, stay on 'idle'
                state = CDC_cState::idle;
            if(lsplit.length() >= 1 && lsplit[0][0] == CDC_CONF_COMMENT) { //comment, stay on 'idle'
                state = CDC_cState::idle;
                //qDebug() << QString(__FUNCTION__) << "found comment on line " << currLine;
            }
            if(lsplit.length() == 1 && lsplit[0][lsplit[0].length()-1] == CDC_CONF_COMMAND)
                state = CDC_cState::at_header;

            skip = true;
        }

        if(state == CDC_cState::at_header && !skip) {
            state = CDC_cState::fail; // defaults to 'fail' if no transition below happens

            if(lsplit.length() == 0) //empty line, stay on 'idle'
                state = CDC_cState::idle;
            if(lsplit.length() >= 1 && lsplit[0][0] == CDC_CONF_COMMENT) //comment, stay on 'idle'
                state = CDC_cState::idle;
            if(lsplit.length() == 1 && lsplit[0][lsplit[0].length()-1] == CDC_CONF_COMMAND)
                state = CDC_cState::at_header;
            if(lsplit.length() >= 1 && lsplit[0][lsplit[0].length()-1] != CDC_CONF_COMMAND)
                state = CDC_cState::at_argument;

            skip = true;
        }

        if(state == CDC_cState::at_argument && !skip) {
            state = CDC_cState::fail; // defaults to 'fail' if no transition below happens

            if(lsplit.length() == 0) //empty line, stay on 'idle'
                state = CDC_cState::idle;
            if(lsplit.length() >= 1 && lsplit[0][0] == CDC_CONF_COMMENT) //comment, stay on 'idle'
                state = CDC_cState::idle;
            if(lsplit.length() == 1 && lsplit[0][lsplit[0].length()-1] == CDC_CONF_COMMAND)
                state = CDC_cState::at_header;
            if(lsplit.length() >= 1 && lsplit[0][lsplit[0].length()-1] != CDC_CONF_COMMAND)
                state = CDC_cState::at_argument;

            skip = true;
        }

        // Run actions of each state:
        skip = false;
        if(state == CDC_cState::idle) {}     // no action at 'idle'
        if(state == CDC_cState::at_header) { // create entry, append header
            CDC_confSection sec;
            sec.header.append(lsplit[0].remove(CDC_CONF_COMMAND));
            confFileSections.append(sec);
        }
        if(state == CDC_cState::at_argument) { // append content to last entry's list
            confFileSections.last().contentsList.append(line);
        }
        if(state == CDC_cState::fail) {
            qDebug() << QString(__FUNCTION__) << "got a syntax error on line " << currLine << " of file" << fpath;
            retval = false;
            if(retStatus != NULL)
                *retStatus = CDC_status::syntaxError;

            return retval;
        }

    }

    // End -------------------------
    file->close();
    if(retval)
        qDebug() << QString(__FUNCTION__) << "sucessfully parsed " << fpath;
    return retval;
}

QStringList configurationFileParser::getFileSections() {
    QStringList retList;
    for(int i=0; i < confFileSections.length(); i++)
        retList.append(confFileSections[i].header);
    return retList;
}

QStringList configurationFileParser::getSectionContents(QString secName) {
    QStringList sections = getFileSections();
    int idx = sections.lastIndexOf(secName);
    if(idx != -1)
        return confFileSections[idx].contentsList;
    else { // Return empty list
        QStringList retList;
        return retList;
    }
}

bool configurationFileParser::saveToConfigurationFile(QString filename, CDC_confList sections) {

    if (!filename.isEmpty()) {
        QFile file(filename);

        if (!file.open(QIODevice::WriteOnly))
            return false;

        QString contents;
        for (int i = 0; i < sections.length(); ++i) {
            contents += sections[i].header;
            contents += ":\n";
            for (int j = 0; j < sections[i].contentsList.length(); ++j) {
                contents += sections[i].contentsList[j];
                contents += "\n";
            }
        }

        QTextStream in(&file);
        in << contents;
        file.close();
        return true;
    }
    else
        return false;
}
