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
  * @file    mainwindow.h
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    17-August-2014
  * @brief   CrossDocs GUI main window.
  *
  * \todo \b DONE! Fix the logic with the treeView and the fileListWidget (Works, but make it neater!)
  * \todo Add suport to save and restore last window configuration (width, height, etc)
  * \todo \b DONE! Add the syntax highlighter
  * \todo Make the syntax highlighter trigger the window update everytime he runs on a text block. Only update
  *       the tree if a SE keyword match occurs.
  * \todo Add suport to open the last opened projects
  * \todo IMPORTANT: when cdd parsing fail, the tree shows up with a blank element. Create some 'isOk' in documentWorker
  * \todo VERY IMPORTANT: When a IF isnt modified, it should not be reparsed for its structure!
**/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <iostream>

#include <QtWidgets>
#include <QtWebKitWidgets>
#include <QStandardItemModel>

#include "projectworker.h"

#include "cdchighlighter.h"
#include "cdccodeeditor.h"

class cdcMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    cdcMainWindow(QWidget *parent = 0);
    ~cdcMainWindow();

signals:


public slots: 
    void messageHandler(QtMsgType type, const char *msg, bool isDialog);
    void openProject(QString fileName = "");
    void updateView(QString doctag, QString outfile);

private slots:
    void menuActionAbout();
    void menuSyntaxTriggered(QAction * selectedSyntax);
    void toggleFullscreen(bool);
    void requestBuild();
    void open();
    void loadProject();
    void createNew();
    void saveCurrentFile();
    void textEditorChanged();
    void updateProjectView();

    /**
     * @brief Implements the behavior when an object on the structure tree is selected.
     * If a document is selected, its input files are loaded in the listFilesWidget, and the
     * first of them is loaded in the plainTextEditor. If no files are available, the plainTextEditor
     * is greyed out (disabled). If some structural element of a document is selected, the correct document
     * and its input files are loaded, the input file of the desired section is loaded into the plainTextEditor
     * and the textCursor is moved to the desired line.
     * \todo Make the movement of the textCursor robust to line wrapping and formating in the plainTextEditor
     * @param index The index from the selected item. Gets passed from the calling signal.
     */
    void projectTreeItemSelected(QModelIndex  index);

    void projectTreeItemExpanded(QModelIndex  index);
    void projectTreeItemCollapsed(QModelIndex index);

    void listFilesWidgetSelected();//QTreeWidgetItem* item);

private:
    void createActions();
    void createMenus();
    void createToolbars();
    void createStatusBar();
    void createWidgets();

    void updateSyntaxMenu(CDC_fileSyntax syntax);

    projectWorker  * pw;

    cdcHighlighter * highlighter;

    QString currentProjectPath;
    QString currentDocumentTag;
    int     currentDocumentInputFileIndex;

    /// Holds the tags of the expanded SEs of a document in the treeProject view
    typedef struct {
        QString     doc;
        QStringList tags;
    } treeProjectDocExpTags;
    QList<treeProjectDocExpTags> expandedProjectTreeItems;

    int     analysisThreshold;

/***    GUI ELEMENTS   *******************************************/
    CodeEditor  *plainTextEditor;
    QSettings   *guiSettings;
    QString     windowTitle;
    QToolBar    *toolbarFile;
    QTextEdit   *logArea;
    QListWidget *listFilesWidget;
    QWebView    *webView;
    QTreeView   *treeProject;

    QMenu       *menuFile;
    QMenu       *menuEdit;
    QMenu       *menuHelp;
    QMenu       *menuView;

    QAction     *actionExit;
    QAction     *actionOpen;
    QAction     *actionOpenProject;
    QAction     *actionSave;
    QAction     *actionNew;
    QAction     *actionBuild;
    QAction     *actionTest;
    QAction     *actionAbout;
    QAction     *actionAboutQt;
    QAction     *actionToggleFullscreen;
    QAction     *actionPreferences;
    QActionGroup*actionsSyntaxList;
};

#endif // MAINWINDOW_H
