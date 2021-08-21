#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QLoggingCategory>

#include <iostream>

#include "MainWindow.hpp"
#include "TagEdit.hpp"


int main(int argc, char *argv[])
{
    // to enable qDebug()
// #ifndef NDEBUG
    QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
// #endif

    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("tagbrowser");
    QCoreApplication::setOrganizationName("TagBrowser");

    QCommandLineParser parser;
    parser.addPositionalArgument("folder", "Folder to generate thumbnails for.");
    parser.process(app);

    MainWindow mw;
    if(parser.positionalArguments().size() != 0) {
        QString dir = parser.positionalArguments().at(0);
        mw.startModelView(QDir(dir).canonicalPath());
    }

    mw.show();
    return app.exec();
}
