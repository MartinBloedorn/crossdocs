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
  * @file    main.cpp
  * @author  Martin Vincent Bloedorn
  * @version V0.1.0
  * @date    17-August-2014
  * @brief   CrossDocs GUI entry point.
**/

#include <iostream>

#include <QDebug>

//#include "projectworker.h"
#include "cdcmainwindow.h"
//#include "configurationfileparser.h"
#include "inputfileparser.h"

cdcMainWindow * mainWindow;

/// \brief Wrapper for cdcMainWindow::messageHandler , to use with qInstallMessageHandler .
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray ba = msg.toLocal8Bit();
    mainWindow->messageHandler(type, ba.constData(), false);
}

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("CrossDocs GUI");

    mainWindow = new cdcMainWindow();

    qInstallMessageHandler(messageHandler);

    mainWindow->show();
    qDebug() << "CrossDocs GUI started sucessfully!";
    mainWindow->openProject("/Users/martin/Workspaces/qt/crossdocs_gui/cdc_test/testconf.cdc");

    //if(argc > 1)
        //mainWindow->openProject(argv[1]);

    //inputFileParser * ifparser = new inputFileParser();
    //ifparser->setSyntax(CDC_fileSyntax::doxygen);
    //ifparser->parseInputFile(new QFile("/Users/martin/Workspaces/qt/crossdocs_gui/cdc_test/doctestpage.cdp"));

    //projectWorker pw;
    //pw.configureProject("/Users/martin/Workspaces/qt/crossdocs_gui/cdc_test/testconf.cdc");

    return app.exec();
    //return 0;
}
