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
  * @file    configurationfileparser.h
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    03-August-2014
  * @brief   Definitions for *.cdc file parsing.
  *
  * \todo Add suport for inline comments.
**/

#ifndef CONFIGURATIONFILEPARSER_H
#define CONFIGURATIONFILEPARSER_H

/* Includes -----------------------------------------------------------------*/
// General includes
#include <iostream>

// Qt includes
#include <QDir>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>

// Qt Debug
#include <QtDebug>

// CDC includes
#include "cdcdefs.h"

/* Types --------------------------------------------------------------------*/

/* Definitions --------------------------------------------------------------*/

class configurationFileParser
{
public:
    configurationFileParser();
    ~configurationFileParser();

    /** \brief Parses configuration file in \em fpath.
     *
     *  To implement the read operation of the configuration file in a flexible
     *  fashion, a very simple finite state machine is implemented.
     *  Four states are possible:
     *  - \b idle: Starting state; corresponds to reading an empty line or comment;
     *  - \b at-header: Found a line with a single string (not whitespace-separated) that ends with ':'
     *  - \b at-argument: After finding at least one header (command), found a line with a single string.
     *  - \b fail: Something went wrong. Function returns.
     *
     *  \param fpath The full path of the file to be parsed.
     *  \return Whether the file was correctly parsed or not.
     **/
    bool parseFile(QString fpath, CDC_status * retStatus = NULL);

    /// \brief Overloads previous definition of parseFile to deal with \em QDir type of inputs.
    bool parseFile(QDir fpath, CDC_status * retStatus = NULL) { return parseFile(fpath.absolutePath(),  retStatus); }

    /** \brief Returns the list with all the commands (strings ending with ':') found.
     *
     **/
    QStringList getFileSections();

    /** \brief Returns the with the contents of a specific section.
     *
     *  Returns an empty list if the section does not exist or has no contents.
     **/
    QStringList getSectionContents(QString secName);

    /** \brief Saves \em sections to \em filename according to CDC's syntax.
     *
     **/
    bool saveToConfigurationFile(QString filename, CDC_confList sections);

private:
    // Attributes
    /// \brief Current file being handled.
    QFile * file;
    /// \brief Text stream of the current file handle.
    QTextStream * fileIn;
    /// \brief List of type CDC_confSection containing the sections of the read document.
    QList<CDC_confSection> confFileSections;

};

#endif // CONFIGURATIONFILEPARSER_H
