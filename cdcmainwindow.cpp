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
  * @file    mainwindow.cpp
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    17-August-2014
  * @brief   CrossDocs GUI main window.
**/

#include "cdcmainwindow.h"

// Names for the syntaxes that will appear on the menu
const QString syntaxnameDoxygen   = "Doxygen";
const QString syntaxnameMarkdown  = "Markdown";
const QString syntaxnameNone      = "None";

/**************************************** CONSTRUCTOR *******************************************/
cdcMainWindow::cdcMainWindow(QWidget *parent) :
    QMainWindow(parent),
    currentDocumentTag(""),
    currentDocumentInputFileIndex(-1),
    analysisThreshold(0),
    windowTitle(QString("CrossDocs GUI"))
{
    pw = new projectWorker();

    this->setAttribute(Qt::WA_QuitOnClose);

    createActions();
    createMenus();
    createToolbars();
    createStatusBar();
    createWidgets();

    //guiSettings = new QSettings(QSettings::)... no patience for that now

    highlighter = new cdcHighlighter(plainTextEditor->document());
    highlighter->setSyntax(CDC_fileSyntax::doxygen);

    setWindowTitle(windowTitle);
    setUnifiedTitleAndToolBarOnMac(true);
}

cdcMainWindow::~cdcMainWindow() {
    delete pw;
    delete highlighter;
    delete webView;
}

/**************************************** SLOTS *************************************************/

void cdcMainWindow::toggleFullscreen(bool fs) {
    if(fs) this->showFullScreen();
    else   this->showNormal();
}

void cdcMainWindow::menuActionAbout() {
    QMessageBox::about(this, "About CrossDocs GUI", "Just for fun.");
}

void cdcMainWindow::menuSyntaxTriggered(QAction *selectedSyntax) {
    qDebug() << "Selected syntax " << selectedSyntax->text() << " for current file.";
    selectedSyntax->setChecked(true);
    CDC_fileSyntax newsyntax = CDC_fileSyntax::none;

    if(selectedSyntax->text().compare(syntaxnameDoxygen)      == 0) newsyntax = CDC_fileSyntax::doxygen;
    else if(selectedSyntax->text().compare(syntaxnameMarkdown)== 0) newsyntax = CDC_fileSyntax::markdown;
    else if(selectedSyntax->text().compare(syntaxnameNone)    == 0) newsyntax = CDC_fileSyntax::none;

    pw->setDocumentInputFileSyntax(currentDocumentTag, currentDocumentInputFileIndex, newsyntax);
}

void cdcMainWindow::requestBuild() {
    qDebug() << QString(__FUNCTION__) << " Request build";

    QString filename("/Users/martin/Git/provant-software/io-board/stm32f4/app/remote-controlled-flight/doc/html/index.html");
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
        return;
    }

    QTextStream out(&file);
    QString output = out.readAll();

    // display contents
    plainTextEditor->setPlainText(output);
    webView->setHtml(output, QUrl::fromLocalFile(filename));
}

void cdcMainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        // read from file
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                file.errorString());
            return;
        }

        QTextStream out(&file);
        QString output = out.readAll();

        // display contents
        plainTextEditor->setPlainText(output);
        webView->setHtml(output, QUrl::fromLocalFile(fileName));
    }
}

void cdcMainWindow::openProject(QString fileName) {
    if (fileName.isEmpty()) {
        QFileDialog fd;
        fd.setNameFilter(tr("CrossDocs Project (*.cdc)"));
        fileName = fd.getOpenFileName(this);
        fd.close();
    }
    if (!fileName.isEmpty()) {
        currentProjectPath = fileName;
        loadProject();
    }
}

void cdcMainWindow::loadProject() {
    pw->configureProject(currentProjectPath);

    currentDocumentInputFileIndex = -1;
    currentDocumentTag = "";

    treeProject->setModel(pw->getProjectStructure());
    if(treeProject->model()->rowCount() > 0) {
        treeProject->selectionModel()->select(treeProject->indexAt(QPoint(0,0)), QItemSelectionModel::Select);
        projectTreeItemSelected(treeProject->model()->index(0,0));
    }
}

void cdcMainWindow::createNew() {

}

void cdcMainWindow::textEditorChanged() {
    analysisThreshold++;
    if(analysisThreshold > 10) {
        analysisThreshold = 0;
        updateProjectView();
    }
}

/**
 * @brief This function is only a helper for the cdcMainWindow::analyseProject() method.
 * It implements recursion over the child elements of itemHandle, expanding them if they were found
 * in the list of previously expanded items ( expandedProjectTreeItems ).
 * Given that the Structural model of the document IS bounded, this function will always return.
 */
void recursiveItemExpansion(QStandardItem * itemHandle, QStringList taglist, QTreeView * treeView) {
    for (int i = 0; i < itemHandle->rowCount(); ++i) {
        if(taglist.lastIndexOf(itemHandle->child(i)->data(CDC_docStructuralElementRole::Tag).toString()) != -1)
            treeView->expand(itemHandle->child(i)->index());
        if(itemHandle->child(i)->hasChildren())
            recursiveItemExpansion(itemHandle->child(i), taglist, treeView);
    }
}

void cdcMainWindow::updateProjectView() {
    pw->setDocumentInputFileContents(currentDocumentTag, currentDocumentInputFileIndex, plainTextEditor->toPlainText());

    treeProject->setModel(pw->getProjectStructure());

    QStandardItem * itemHandle;
    QString currdoc;
    QString itemtag;
    QStringList currtags;
    for (int i = 0; i < treeProject->model()->rowCount(); ++i) { // Iterate over all docs
        itemHandle = dynamic_cast<QStandardItemModel *>(treeProject->model())->item(i);
        itemtag    = itemHandle->data(CDC_docStructuralElementRole::Tag).toString();
        currdoc    = itemHandle->data(CDC_docStructuralElementRole::Doc).toString();
        for (int j = 0; j < expandedProjectTreeItems.length(); ++j) { // Go through the list of items that were expanded
            if(expandedProjectTreeItems[j].doc.compare(currdoc) == 0) {
                currtags = expandedProjectTreeItems[j].tags;
                if(currtags.lastIndexOf(itemtag) != -1) // Expand document if needed
                    treeProject->expand(itemHandle->index());
                recursiveItemExpansion(itemHandle, currtags, treeProject); // Check 'n expand doc's elements
            }
        }
    }
}

void cdcMainWindow::projectTreeItemExpanded(QModelIndex index) {
    QString tag = treeProject->model()->data(index, CDC_docStructuralElementRole::Tag).toString();
    QString doc = treeProject->model()->data(index, CDC_docStructuralElementRole::Doc).toString();
    for(int i = 0; i < expandedProjectTreeItems.length(); ++i) {
       if(expandedProjectTreeItems[i].doc.compare(doc) == 0) {
           if(expandedProjectTreeItems[i].tags.lastIndexOf(tag) == -1) {
               expandedProjectTreeItems[i].tags.append(tag);
               //qDebug() << "Appended " << tag << " to document " << doc;
           }
           return;
       }
    }
    treeProjectDocExpTags texp;
    texp.doc = doc; texp.tags.append(tag);
    expandedProjectTreeItems.append(texp);
    //qDebug() << "Appended " << tag << " to NEWLY created document entry, " << doc;
}

void cdcMainWindow::projectTreeItemCollapsed(QModelIndex index) {
    QString tag = treeProject->model()->data(index, CDC_docStructuralElementRole::Tag).toString();
    QString doc = treeProject->model()->data(index, CDC_docStructuralElementRole::Doc).toString();
    for(int i = 0; i < expandedProjectTreeItems.length(); ++i) {
       if(expandedProjectTreeItems[i].doc.compare(doc) == 0) {
           expandedProjectTreeItems[i].tags.removeAll(tag);
           qDebug() << "Removed " << tag << " in document " << doc;
           return;
       }
    }
}

void cdcMainWindow::projectTreeItemSelected(QModelIndex index) {
    QString selectedTag = treeProject->model()->data(index, CDC_docStructuralElementRole::Tag).toString();
    QString selectedDoc = treeProject->model()->data(index, CDC_docStructuralElementRole::Doc).toString();

    int line          = treeProject->model()->data(index, CDC_docStructuralElementRole::Line).toInt();
    int selectedIndex = treeProject->model()->data(index, CDC_docStructuralElementRole::Index).toInt();

    CDC_docStructuralElementType type = static_cast<CDC_docStructuralElementType>
            (treeProject->model()->data(index, CDC_docStructuralElementRole::Type).toInt());

    // Store the contents of current text being edited. Though you may not be changing files, saving the contents
    // here makes the logic of the switching soooo much more readable, at (theoretically) almost no cost (assign a variable)
    pw->setDocumentInputFileContents(currentDocumentTag, currentDocumentInputFileIndex, plainTextEditor->toPlainText());

    if(type == CDC_docStructuralElementType::document) {
        if(selectedTag.compare(currentDocumentTag) != 0) { // Selected a different doc!
            listFilesWidget->clear();
            QStringList iflist = pw->getDocumentInputFilesList(selectedTag);
            if(iflist.length() > 0) {                      // This doc at least one input file!
                for (int i = 0; i < iflist.length(); ++i)
                    listFilesWidget->addItem(iflist[i]);
                currentDocumentInputFileIndex = 0;
                listFilesWidget->horizontalScrollBar()->setValue(listFilesWidget->horizontalScrollBar()->maximum());
            }
            else                                           // This doc has no input files
                currentDocumentInputFileIndex = -1;
            currentDocumentTag = selectedTag;
        }
        if (currentDocumentInputFileIndex != -1) {         // If input files are available for that doc...
            currentDocumentInputFileIndex = 0;
            listFilesWidget->setCurrentRow(0);
            plainTextEditor->setEnabled(true);
            plainTextEditor->setText(pw->getDocumentInputFileContents(selectedTag, 0));
            updateSyntaxMenu(pw->getDocumentInputFileSyntax(selectedTag,0));
        }
        else {
            plainTextEditor->setText(QString(""));
            plainTextEditor->setEnabled(false);
            return;
        }
    }
    else {                                                 // Selected some section, subsection...
        if(selectedDoc.compare(currentDocumentTag) != 0) {
            listFilesWidget->clear();
            QStringList iflist = pw->getDocumentInputFilesList(selectedDoc);
            if(iflist.length() > 0) {                      // This doc at least one input file!
                for (int i = 0; i < iflist.length(); ++i)
                    listFilesWidget->addItem(iflist[i]);
                currentDocumentInputFileIndex = 0;
                listFilesWidget->horizontalScrollBar()->setValue(listFilesWidget->horizontalScrollBar()->maximum());
            }
            else                                           // This doc has no input files
                currentDocumentInputFileIndex = -1;        // Should never happen, still...
            currentDocumentTag = selectedDoc;
        }
        if(currentDocumentInputFileIndex != -1) {
            currentDocumentInputFileIndex = selectedIndex;
            listFilesWidget->setCurrentRow(selectedIndex);
            plainTextEditor->setEnabled(true);
            plainTextEditor->setText(pw->getDocumentInputFileContents(selectedDoc, selectedIndex));
            updateSyntaxMenu(pw->getDocumentInputFileSyntax(selectedDoc,selectedIndex));
        }
        else {
            plainTextEditor->setText(QString(""));
            plainTextEditor->setEnabled(false);
            return;
        }

        plainTextEditor->setEnabled(true);
        plainTextEditor->setFocus();
        QTextCursor cursor = plainTextEditor->textCursor();  // Now move the cursor to the desired line
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line-1);
        plainTextEditor->setTextCursor(cursor);
    }
}

void cdcMainWindow::listFilesWidgetSelected() {
    int index = listFilesWidget->currentRow();
    if(index != currentDocumentInputFileIndex) {
        pw->setDocumentInputFileContents(currentDocumentTag, currentDocumentInputFileIndex, plainTextEditor->toPlainText());
        currentDocumentInputFileIndex = index;
        plainTextEditor->setText(pw->getDocumentInputFileContents(currentDocumentTag, index));
    }
}

void cdcMainWindow::messageHandler(QtMsgType type, const char *msg, bool isDialog)
{
    if(isDialog)
    {
        switch(type) {
            case QtDebugMsg:   QMessageBox::warning(this, tr("Message"),  QString(msg), QMessageBox::Ok); break;
            case QtWarningMsg: QMessageBox::warning(this, tr("Warning"),  QString(msg), QMessageBox::Ok); break;
            case QtCriticalMsg:QMessageBox::warning(this, tr("Critical!"),QString(msg), QMessageBox::Ok); break;
            case QtFatalMsg:   QMessageBox::warning(this, tr("FATAL"),    QString(msg), QMessageBox::Ok); break;
        }
    }
    else
    {
        switch (type) {
            case QtDebugMsg:
                logArea->setTextColor(Qt::black);
                logArea->append(QString(QDateTime::currentDateTime().toString("hh.mm.ss.zzz") + ": " + msg));
                break;
            case QtWarningMsg:
                logArea->setTextColor(QColor(255,150,0));
                logArea->append(QString(QDateTime::currentDateTime().toString("hh.mm.ss.zzz") + ": " + msg));
                break;
            case QtCriticalMsg:
                logArea->setTextColor(Qt::red);
                logArea->append(QString(QDateTime::currentDateTime().toString("hh.mm.ss.zzz") + ": " + msg));
                break;
            case QtFatalMsg:
                logArea->setTextColor(Qt::red);
                logArea->append(QString(QDateTime::currentDateTime().toString("hh.mm.ss.zzz") + " FATAL: " + msg));
            break;
        }
    }
    logArea->moveCursor(QTextCursor::End);
}

// PRIVATE ------------------------------------------------------------------------
void cdcMainWindow::updateSyntaxMenu(CDC_fileSyntax syntax) {
    QString syntaxname;
    switch (syntax) {
    case CDC_fileSyntax::doxygen:
        syntaxname = syntaxnameDoxygen; break;
    case CDC_fileSyntax::markdown:
        syntaxname = syntaxnameMarkdown; break;
    default:
        syntaxname = syntaxnameNone; break;
    }
    for (int i = 0; i < actionsSyntaxList->actions().length(); ++i) {
        if(actionsSyntaxList->actions()[i]->text().compare(syntaxname) == 0)
            actionsSyntaxList->actions()[i]->setChecked(true);
    }
}


/**************************************** GUI SETUP *********************************************/
void cdcMainWindow::createActions()
{
    actionExit = new QAction(QIcon(":/icons/gnomeexit.png"), tr("&Close"), this);
    actionExit->setShortcuts(QKeySequence::Quit);
    actionExit->setStatusTip(tr("Quit CrossDocs GUI"));
    connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(this, SIGNAL(destroyed()), this, SLOT(close()));

    actionBuild = new QAction(QIcon(":/icons/run.png"), tr("&Build"), this);
    actionBuild->setShortcuts(QKeySequence::Bold);
    actionBuild->setStatusTip(tr("Build current document"));
    connect(actionBuild, SIGNAL(triggered()), this, SLOT(requestBuild()));

    actionNew = new QAction(QIcon(":/icons/new_star.png"), tr("&New..."), this);
    actionNew->setShortcuts(QKeySequence::New);
    actionNew->setStatusTip(tr("Create new document/project"));
    connect(actionNew, SIGNAL(triggered()), this, SLOT(createNew()));

    actionOpen = new QAction(QIcon(":/icons/open.png"), tr("&Open"), this);
    actionOpen->setShortcuts(QKeySequence::Open);
    actionOpen->setStatusTip(tr("Open a file"));
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));

    actionOpenProject = new QAction(QIcon(":/icons/open.png"), tr("Open &Project"), this);
    actionOpenProject->setShortcuts(QKeySequence::Open);
    actionOpenProject->setStatusTip(tr("Open a project"));
    connect(actionOpenProject, SIGNAL(triggered()), this, SLOT(openProject()));

    actionPreferences = new QAction(QIcon(":/icons/gnomepreferences.png"), tr("&Preferences"), this);
    actionPreferences->setStatusTip(tr("CrossDocs GUI preferences"));
    actionPreferences->setShortcut(QKeySequence::Preferences);
    //connect(actionPreferences, SIGNAL(triggered()), this, SLOT(launchPreferencesPanel()));

    actionAbout = new QAction(tr("&Info"), this);
    actionAbout->setStatusTip(tr("Information on CrossDocs GUI"));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(menuActionAbout()));

    actionAboutQt = new QAction(tr("About Qt"), this);
    actionAboutQt->setStatusTip(tr("About Qt"));
    connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    actionToggleFullscreen = new QAction(QIcon(":/icons/gnomefullscreen.png"), tr("Full&screen"), this);
    actionToggleFullscreen->setStatusTip(tr("Toggle Fullscreen"));
    actionToggleFullscreen->setCheckable(true);
    actionToggleFullscreen->setChecked(false);
    connect(actionToggleFullscreen, SIGNAL(toggled(bool)), this, SLOT(toggleFullscreen(bool)));
}

void cdcMainWindow::createMenus() {
    menuFile = menuBar()->addMenu(tr("&File"));
    menuFile->addAction(actionNew);
    menuFile->addAction(actionOpenProject);
    menuFile->addAction(actionExit);

    menuEdit = menuBar()->addMenu(tr("&Tools"));
    menuEdit->addAction(actionBuild);
    menuEdit->addAction(actionPreferences);

    actionsSyntaxList = new QActionGroup(this);
    actionsSyntaxList->setExclusive(true);
    actionsSyntaxList->addAction(syntaxnameDoxygen);
    actionsSyntaxList->addAction(syntaxnameMarkdown);
    actionsSyntaxList->addAction(syntaxnameNone);
    for (int i = 0; i < actionsSyntaxList->actions().length(); ++i)
        actionsSyntaxList->actions()[i]->setCheckable(true);

    QMenu * submenuSyntax = new QMenu(tr("&Set Syntax"));
    submenuSyntax->addActions(actionsSyntaxList->actions());
    menuEdit->addMenu(submenuSyntax);

    menuView = menuBar()->addMenu(tr("&View"));
    menuView->addAction(actionToggleFullscreen);

    menuBar()->addSeparator();

    menuHelp = menuBar()->addMenu(tr("&Help"));
    menuHelp->addAction(actionAbout);
    menuHelp->addAction(actionAboutQt);

    connect(actionsSyntaxList, SIGNAL(triggered(QAction*)), this, SLOT(menuSyntaxTriggered(QAction*)));
}

void cdcMainWindow::createToolbars()
{
    toolbarFile = addToolBar(tr("Toolbar"));
    toolbarFile->addAction(actionNew);
    toolbarFile->addAction(actionOpenProject);
    toolbarFile->addAction(actionBuild);
    toolbarFile->addAction(actionPreferences);
    toolbarFile->addAction(actionToggleFullscreen);
    toolbarFile->addAction(actionExit);
    menuView->addAction(toolbarFile->toggleViewAction());
}

void cdcMainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void cdcMainWindow::createWidgets()
{
    plainTextEditor     = new CodeEditor(this);
    logArea             = new QTextEdit(this);
    treeProject         = new QTreeView(this);
    listFilesWidget     = new QListWidget(this);
    webView             = new QWebView(this);

    QFont * font      = new QFont("Courier New");
    QFontMetrics * fm = new QFontMetrics(*font);
    plainTextEditor->setFont(*font);
    plainTextEditor->resize(fm->width(".")*160, plainTextEditor->height()); //160 chars width
    plainTextEditor->setLineWrapMode(QPlainTextEdit::NoWrap);

    logArea->setTextInteractionFlags(Qt::TextSelectableByMouse);
    logArea->setFocusPolicy(Qt::NoFocus);
    logArea->setFont(QFont("Courier"));

    listFilesWidget->setAlternatingRowColors(true);

    // --------------------------

    QDockWidget *dockFiles     = new QDockWidget(tr("Files"),    this);
    QDockWidget *dockStructure = new QDockWidget(tr("Index"), this);

    QSplitter   *vCentralSplitter = new QSplitter(this);
    QSplitter   *hCentralSplitter = new QSplitter(this);
    QGroupBox   *hCentralPanel    = new QGroupBox(this);
    QHBoxLayout *hCentralLayout   = new QHBoxLayout(this);
    QSplitter   *vSideSplitter    = new QSplitter(this);
    QGroupBox   *hSidePanel       = new QGroupBox(this);
    QVBoxLayout *hSideLayout      = new QVBoxLayout(this);

    QString string = "<html><body><h1>Welcome to CrossDocs GUI!</h1>"
                     " <p>Type in your text on the left pane and click"
                     " build to generate your documentation!</p>"
                     " </body></html>";
    webView->setHtml(string);

    dockStructure->setWidget(treeProject);
    dockFiles->setWidget(listFilesWidget);

    hSideLayout->addWidget(dockStructure);
    hSideLayout->addWidget(dockFiles);
    hSidePanel->setLayout(hSideLayout);

    vCentralSplitter->addWidget(plainTextEditor);
    vCentralSplitter->addWidget(webView);
    hCentralLayout->addWidget(vCentralSplitter);
    hCentralPanel->setLayout(hCentralLayout);

    hCentralSplitter->setOrientation(Qt::Vertical);
    hCentralSplitter->addWidget(hCentralPanel);
    hCentralSplitter->addWidget(logArea);

    vSideSplitter->addWidget(hSidePanel);
    vSideSplitter->addWidget(hCentralSplitter);

    setCentralWidget(vSideSplitter);

    menuView->addAction(dockFiles->toggleViewAction());
    menuView->addAction(dockStructure->toggleViewAction());

    connect(treeProject, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(projectTreeItemSelected(QModelIndex)));
    connect(treeProject, SIGNAL(expanded(QModelIndex)), this, SLOT(projectTreeItemExpanded(QModelIndex)));
    connect(treeProject, SIGNAL(collapsed(QModelIndex)), this, SLOT(projectTreeItemCollapsed(QModelIndex)));
    connect(listFilesWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(listFilesWidgetSelected()));
    connect(plainTextEditor, SIGNAL(textChanged()), this, SLOT(textEditorChanged()));

    connect(new QShortcut(QKeySequence::Undo, this), SIGNAL(activated()), plainTextEditor, SLOT(undo()));
    connect(new QShortcut(QKeySequence::Redo, this), SIGNAL(activated()), plainTextEditor, SLOT(redo()));
}
