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
#include <QSortFilterProxyModel>
#include <QProcess>

#include <iostream>
#include <qsortfilterproxymodel.h>

#include "MainWindow.hpp"
#include "ThumbnailModel.hpp"
#include "ThumbnailView.hpp"
#include "FilterWidget.hpp"
#include "TMSU.hpp"


MainWindow::MainWindow()
    : QMainWindow(), mModel(nullptr), mView(nullptr), mDock(nullptr), mTagWidget(nullptr), mFilterPathProxyModel(nullptr)
{
    qDebug() << "MainWindow::MainWindow()";
    createMenus();
}


MainWindow::MainWindow(const QString &dir)
    : QMainWindow(), mModel(nullptr), mView(nullptr), mDock(nullptr), mTagWidget(nullptr), mFilterPathProxyModel(nullptr)
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
    connect(mSelectAct, &QAction::triggered, mView, &QAbstractItemView::selectAll);
    mToolBar->addAction(mSelectAct);

    mClearAct = new QAction(tr("&Clear Selection"), this);
    mClearAct->setShortcuts({tr("Ctrl+Shift+A")});
    mClearAct->setIcon(QIcon::fromTheme("edit-select-none"));
    mClearAct->setDisabled(true);
    connect(mClearAct, &QAction::triggered, mView, &QAbstractItemView::clearSelection);
    mToolBar->addAction(mClearAct);

    mFilterPathWidget = new FilterWidget(this);
    mFilterPathWidget->setPlaceholderText("Filter by path");
    connect(mFilterPathWidget, &FilterWidget::filterChanged, this, &MainWindow::pathFilterChanged);
    mToolBar->addWidget(mFilterPathWidget);

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
    if(!dir.isEmpty() && QDir(dir).exists()) {
        mLastDir = dir;
        if(mModel) delete mModel;

        mClearAct->setEnabled(true);
        mSelectAct->setEnabled(true);

        mModel = new ThumbnailModel(dir, this);

        if(mFilterPathProxyModel) delete mFilterPathProxyModel;
        mFilterPathProxyModel = new QSortFilterProxyModel(this);
        mFilterPathProxyModel->setSourceModel(mModel);
        mFilterPathProxyModel->setFilterRole(Qt::ToolTipRole);
        mView->setModel(mFilterPathProxyModel);


        connect(mView->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &MainWindow::handleSelection);
        connect(mView, &ThumbnailView::doubleClicked,
                this, &MainWindow::handleDoubleClick);

        refreshTagWidget();
    }
}


void
MainWindow::handleSelection(const QItemSelection &selected, const QItemSelection &deselected)
{
    qDebug() << "MainWindow::handleSelection()";
    for(const auto& index : selected.indexes()) {
        mModel->mSelected.insert(mFilterPathProxyModel->mapToSource(index).row());
    }
    for(const auto& index : deselected.indexes()) {
        mModel->mSelected.remove(mFilterPathProxyModel->mapToSource(index).row());
    }
    refreshTagWidget();
}


void
MainWindow::handleDoubleClick(const QModelIndex &index)
{
    qDebug() << "MainWindow::handleDoubleClick()";
    // for windows start (cmd) or Invoke-Item (powershell)
    QProcess p(this);
    QStringList args;
    args << mModel->mData[mFilterPathProxyModel->mapToSource(index).row()].url.path();
    p.setProgram("xdg-open");
    p.setArguments(args);
    qint64 pid;
    p.startDetached(&pid);
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
        connect(mTagWidget, &TagWidget::addTagClicked, this, &MainWindow::addTag);
        connect(mTagWidget, &TagWidget::removeTagClicked, this, &MainWindow::removeTag);

        mDock->setWidget(mTagWidget);
    }
}


void
MainWindow::pathFilterChanged()
{
    mFilterPathProxyModel->setFilterFixedString(mFilterPathWidget->text());
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
    if(mDock)
        delete mDock;
    if(mTagWidget)
        delete mTagWidget;
    if(mFilterPathProxyModel)
        delete mFilterPathProxyModel;

    menuBar()->clear();

    delete mToolBar;
    delete mOpenAct;
    delete mSelectAct;
    delete mClearAct;
    delete mFilterPathWidget;
    delete mEmpty;
    delete mExitAct;
    delete mView;
}
