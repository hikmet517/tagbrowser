#include <QApplication>
#include <QDir>
#include <QCommandLineParser>
#include <QDebug>
#include <QLoggingCategory>

#include <iostream>

#include "MainWindow.hpp"
#include "TagEdit.hpp"


int main(int argc, char *argv[])
{
    // to enable qDebug()
#ifndef NDEBUG
    QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
#endif

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("tagbrowser");
    QCoreApplication::setOrganizationName("TagBrowser");

    QCommandLineParser parser;
    parser.addPositionalArgument("folder", "Folder to generate thumbnails for.");
    parser.process(app);

    MainWindow *mw = nullptr;
    if(parser.positionalArguments().size() == 0) {
        mw = new MainWindow;
    }
    else {
        QString dir = parser.positionalArguments().at(0);
        mw = new MainWindow(QDir(dir).canonicalPath());
    }

    mw->show();
    int ret = app.exec();
    delete mw;
    return ret;
}
