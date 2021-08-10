#include <QWidget>
#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include <QListView>
#include <QMessageBox>
#include <QAction>
#include <QIcon>
#include <QDrag>
#include <QMimeData>
#include <QDropEvent>
#include <QListView>
#include <QDockWidget>
#include <QToolBar>
#include <QFileInfo>

#include <iostream>

#include "MainWindow.hpp"
#include "ThumbnailModel.hpp"
#include "ThumbnailView.hpp"
#include "TMSU.hpp"


MainWindow::MainWindow()
    : QMainWindow(), mModel(nullptr), mView(nullptr), mDock(nullptr), mTagWidget(nullptr)
{
    qDebug() << "MainWindow::MainWindow()";
    createMenus();
}


MainWindow::MainWindow(const QString &dir)
    : QMainWindow(), mModel(nullptr), mView(nullptr), mDock(nullptr), mTagWidget(nullptr)
{
    qDebug() << "MainWindow::MainWindow(QString)";
    createMenus();
    startModelView(dir);
}


void
MainWindow::createMenus()
{
    // setup toolbar
    mToolBar = new QToolBar(this);
    mToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mToolBar->setFloatable(false);
    mToolBar->setWindowTitle("Main Toolbar");

    mOpenAct = new QAction(tr("&Open..."), this);
    mOpenAct->setShortcuts(QKeySequence::Open);
    mOpenAct->setIcon(QIcon::fromTheme("document-open"));
    connect(mOpenAct, &QAction::triggered, this, &MainWindow::openDirectory);
    mToolBar->addAction(mOpenAct);

    mSelectAct = new QAction(tr("Select &All"), this);
    mSelectAct->setShortcuts({tr("Ctrl+A")});
    mSelectAct->setIcon(QIcon::fromTheme("edit-select-all"));
    mSelectAct->setDisabled(true);
    mToolBar->addAction(mSelectAct);

    mClearAct = new QAction(tr("&Clear Selection"), this);
    mClearAct->setShortcuts({tr("Ctrl+Shift+A")});
    mClearAct->setIcon(QIcon::fromTheme("edit-select-none"));
    mClearAct->setDisabled(true);
    mToolBar->addAction(mClearAct);

    mEmpty = new QWidget();
    mEmpty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    mToolBar->addWidget(mEmpty);

    mExitAct = new QAction(tr("&Quit"), this);
    mExitAct->setShortcuts(QKeySequence::Quit);
    mExitAct->setIcon(QIcon::fromTheme("application-exit"));
    connect(mExitAct, &QAction::triggered, this, &QWidget::close);
    mToolBar->addAction(mExitAct);

    addToolBar(mToolBar);

    // setup view
    mView = new ThumbnailView(this);
    mView->setWindowTitle("Files");
    setCentralWidget(mView);

    // setup dock
    mDock = new QDockWidget("Tags", this);
    mDock->setFeatures(QDockWidget::DockWidgetMovable);
    mDock->setMinimumWidth(200);
    addDockWidget(Qt::RightDockWidgetArea, mDock);

    setAcceptDrops(true);
}


void
MainWindow::openDirectory()
{
    QString defaultDir = mLastDir.isEmpty() ? "" : mLastDir;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    defaultDir,
                                                    QFileDialog::ShowDirsOnly);
    startModelView(dir);
}


void
MainWindow::startModelView(const QString& dir)
{
    if(!dir.isEmpty() && QDir(dir).exists()){
        mLastDir = dir;
        if(mModel) delete mModel;

        connect(mClearAct, &QAction::triggered, mView, &QAbstractItemView::clearSelection);
        mClearAct->setEnabled(true);
        connect(mSelectAct, &QAction::triggered, mView, &QAbstractItemView::selectAll);
        mSelectAct->setEnabled(true);

        mModel = new ThumbnailModel(dir, this);
        mView->setModel(mModel);
        refreshTagWidget();
    }
}


void
MainWindow::addTag(const QString& tag)
{
    qDebug() << "MainWindow::addTag()";
    int res = TMSU::addTag(tag, mModel->getSelectedPaths());
    if(res == 0) {
        mModel->getTagsFromDB();
        refreshTagWidget();
        mTagWidget->setFocus();
    }
    else {
        QMessageBox msg = QMessageBox(QMessageBox::Information,
                                      "Tag cannot be added",
                                      "Process returned " + QString::number(res));
        msg.exec();
    }
}


void
MainWindow::removeTag(const QString& tag)
{
    qDebug() << "MainWindow::remove()";
    for(const auto& file : mModel->getSelectedPaths()) {
        int res = TMSU::removeTag(tag, file);
        if(res == 0) {
            mModel->getTagsFromDB();
            refreshTagWidget();
        }
        else {
            QMessageBox msg = QMessageBox(QMessageBox::Information,
                                          "Tag cannot be added",
                                          "Process returned " + QString::number(res));
            msg.exec();
            break;
        }
    }
}


void
MainWindow::refreshTagWidget()
{
    qDebug() << "MainWindow::refreshTagWidget()";
    if(mTagWidget) {
        delete mTagWidget;
        mTagWidget = nullptr;
    }

    if(mModel->hasSelected()) {
        mTagWidget = new TagWidget(mModel->getSelectedTags(), mModel->getAllTags(), mDock);
        mDock->setWidget(mTagWidget);
    }
}


// https://doc.qt.io/qt-5/dnd.html
void
MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    qDebug() << "MainWindow::dragMoveEvent()";
    if (event->mimeData()->hasFormat("text/plain"))
        event->acceptProposedAction();
}


void
MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "MainWindow::dragEnterEvent()";
    if (event->mimeData()->hasFormat("text/plain"))
        event->acceptProposedAction();
}


void
MainWindow::dropEvent(QDropEvent* event)
{
    qDebug() << "MainWindow::dropEvent()";
    const QMimeData* mimeData = event->mimeData();

    // check for our needed mime type, here a file or a list of files
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();

        // extract the local paths of the files
        for (int i = 0; i < urlList.size(); i++) {
            const QString& path = urlList.at(i).path();
            QFileInfo fi(path);
            if(fi.isDir() && fi.exists()) {
                startModelView(path);
                break;
            }
        }
    }
    event->acceptProposedAction();
}


MainWindow::~MainWindow()
{
    if(mModel)
        delete mModel;
    if(mView)
        delete mView;
    if(mDock)
        delete mDock;
    if(mTagWidget)
        delete mTagWidget;

    menuBar()->clear();

    delete mOpenAct;
    delete mExitAct;
    delete mClearAct;
    delete mSelectAct;
    delete mEmpty;
    delete mToolBar;
}
