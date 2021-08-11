#include <QApplication>
#include <QDir>
#include <QCommandLineParser>
#include <QDebug>
#include <QLoggingCategory>
#include <QScreen>

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
        mw = new MainWindow(QDir(dir).absolutePath());
    }

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect rect = screen->availableGeometry();
    mw->resize(rect.width()*3/4, rect.height()*3/4);
    mw->setWindowTitle("Tag Browser");
    mw->show();
    return app.exec();
}
