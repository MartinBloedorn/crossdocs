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
  * @file    cdcdefs.h
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    15-August-2014
  * @brief   Classes, namespaces and types for CrossDocs.
**/

#ifndef CDCDEFS_H
#define CDCDEFS_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QVariant>

/* Types --------------------------------------------------------------------*/

/** \brief Enumeration for possible return status on CDC's methods.
 *
 *  \todo Document each status (they are, as of this version, not yet fully
 *  defined).
 **/
enum class CDC_status {
    ok,             /// Operation went ok
    ioError,        /// Operation failed due to IO error (file-related)
    syntaxError,    /// Operation failed due to syntax error in a file
    paramError      /// Invalid parameter passed to method
};

/** \brief Structure that contains the header and the contents on each section of
 *  a configuration file.
 *
 **/
typedef struct {
    QString header;
    QStringList contentsList;
} CDC_confSection;

/** \brief A list of configuration sections.
 *
 *  Represents data obtained from a conf file, or data that will be saved to a file.
 **/
typedef QList<CDC_confSection> CDC_confList;

/// \brief For furhter versions - will allow different doc build setups
enum class CDC_buildEngine {
    doxygen,
    markdown,
    custom,
    none        /// When no build engine is specified, the conf. next hierarchically available option
};

/// \brief Defines which syntatical analysis will be performed on a input file.
enum class CDC_fileSyntax {
    doxygen,
    latex,
    html,
    markdown,
    none
};

enum class CDC_docStructuralElementType {
    document,
    section,
    subsection,
    subsubsection,
    paragraph
};

/// \brief Struct that defines one document's parameters. Each document generates independent
typedef struct {
    QString tag;                /// Doc tag. Can be used for relative search within a tree if cddFile is empty.
    QString name;               /// Presentable name.
    QString cddFilePath;        /// Absolute path for document's cdd configuration file.
    QList<QFile *> inputFiles;  /// Doc's input files parsed from it's cdd file.
} CDC_document;

/// Retains data on the structural sections found in a document's content
typedef struct {
    QString tag;    /// Tag of the structural element (problems will happen if not unique!)
    QString name;   /// Pretty name of the structural element
    int line;       /// Line number where the structural element was found
    int index;      /// Index (column) wherer the structural element
    CDC_docStructuralElementType type; /// Identifies the type of the element (section? paragraph? ...)
} CDC_docStructuralElement;

/// Role definitions used to store Structural Element's data in QVariant containers on a QStandardModel structure
enum CDC_docStructuralElementRole {
    Tag     = Qt::UserRole+1,   /// Structural element's tag
    Line    = Qt::UserRole+2,   /// Line where structural element was found
    Type    = Qt::UserRole+3,   /// Type of structural element (must be converted to int with static_cast<int>())
    Doc     = Qt::UserRole+4,   /// Parent document's tag
    Index   = Qt::UserRole+5    /// Index of the input file in the parent document's IFlist
};

/* Definitions --------------------------------------------------------------*/
#define CDC_CONF_COMMENT '#'
#define CDC_CONF_COMMAND ':'

#endif // CDCDEFS_H
