#include <QApplication>
#include <QDebug>
#include <QLoggingCategory>
#include <QCommandLineParser>

#include <iostream>

#include "MainWindow.hpp"
#include "TagEdit.hpp"


int main(int argc, char *argv[])
{
#ifndef NDEBUG
    // to enable qDebug()
    QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
#endif

    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addPositionalArgument("folder", "Folder to generate thumbnails for.");
    parser.process(app);

    MainWindow *mw = nullptr;
    if(parser.positionalArguments().size() == 0) {
        mw = new MainWindow;
    }
    else {
        QString dir = parser.positionalArguments().at(0);
        mw = new MainWindow(dir);
    }
    mw->resize(1200, 600);
    mw->setWindowTitle("Tag Browser");
    mw->show();
    return app.exec();
}
