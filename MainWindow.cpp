#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDockWidget>
#include <QDrag>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QProcess>
#include <QScreen>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QStatusBar>
#include <QToolBar>
#include <QWidget>

#include <iostream>

#include "FilterTagProxyModel.hpp"
#include "FilterWidget.hpp"
#include "MainWindow.hpp"
#include "TMSU.hpp"
#include "ThumbnailModel.hpp"
#include "ThumbnailView.hpp"


MainWindow::MainWindow()
    : QMainWindow(), mSettings(nullptr), mView(nullptr), mModel(nullptr), mDock(nullptr), mTagWidget(nullptr),
      mFilterPathProxyModel(nullptr), mFilterTagProxyModel(nullptr)
{
    qDebug() << "MainWindow::MainWindow()";
    mSettings = new QSettings(this);
    setWindowTitle(QCoreApplication::applicationName());
    setupBasics();
    readSettings();
}


void
MainWindow::setupBasics()
{
    // setup actions
    mOpenAct = new QAction(tr("&Open..."), this);
    mOpenAct->setShortcuts(QKeySequence::Open);
    mOpenAct->setToolTip(QString(tr("Open Directory (%1)")).arg(QKeySequence(QKeySequence::Open).toString()));
    mOpenAct->setIcon(QIcon::fromTheme("document-open"));
    connect(mOpenAct, &QAction::triggered, this, &MainWindow::openDirectory);

    mSelectAct = new QAction(tr("Select &All"), this);
    mSelectAct->setShortcut(tr("Alt+A"));
    mSelectAct->setToolTip(tr("Select All (Alt+A)"));
    mSelectAct->setIcon(QIcon::fromTheme("edit-select-all"));
    mSelectAct->setDisabled(true);

    mClearAct = new QAction(tr("&Clear Selection"), this);
    mClearAct->setShortcut(tr("Alt+Shift+A"));
    mClearAct->setToolTip(tr("Clear Selection (Alt+Shift+A)"));
    mClearAct->setIcon(QIcon::fromTheme("edit-select-none"));
    mClearAct->setDisabled(true);

    mFilterPathWidget = new FilterWidget(tr("Filter by path using regex"),
                                         {".*.(mp4|avi|mkv|mov|wmv|webm)$",
                                          ".*.(jpg|jpeg|png|bmp|gif|webp)"}, this);
    connect(mFilterPathWidget, &FilterWidget::returnPressed, this, &MainWindow::pathFilterChanged);
    mFilterPathWidget->setDisabled(true);

    mFilterTagWidget = new FilterWidget(tr("Filter by tag using boolean logic"), this);
    connect(mFilterTagWidget, &FilterWidget::returnPressed, this, &MainWindow::tagFilterChanged);
    mFilterTagWidget->setDisabled(true);

    mFiller = new QWidget(this);
    mFiller->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    mExitAct = new QAction(tr("&Quit"), this);
    mExitAct->setShortcuts(QKeySequence::Quit);
    mExitAct->setIcon(QIcon::fromTheme("application-exit"));
    connect(mExitAct, &QAction::triggered, this, &QWidget::close);

    mAboutAct = new QAction(tr("&About"), this);
    mAboutAct->setStatusTip(tr("Show the application's About box"));
    connect(mAboutAct, &QAction::triggered, this, &MainWindow::about);

    mAboutQtAct = new QAction(tr("About &Qt"), this);
    mAboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(mAboutQtAct, &QAction::triggered, qApp, &QApplication::aboutQt);

    mHideMenuAct = new QAction(tr("Toggle Menu Bar"), this);
    mHideMenuAct->setToolTip(tr("Show/hide Menu Bar (Ctrl+M)"));
    mHideMenuAct->setShortcut(tr("Ctrl+M"));
    mHideMenuAct->setIcon(QIcon::fromTheme("show-menu"));
    connect(mHideMenuAct, &QAction::triggered, this, &MainWindow::toggleMenuHide);


    // setup toolbar
    mToolBar = new QToolBar(this);
    mToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mToolBar->setFloatable(false);
    mToolBar->setWindowTitle(tr("Main Toolbar"));
    mToolBar->addAction(mOpenAct);
    mToolBar->addAction(mSelectAct);
    mToolBar->addAction(mClearAct);
    mToolBar->addAction(mHideMenuAct);
    mToolBar->addWidget(mFilterPathWidget);
    mToolBar->addWidget(mFilterTagWidget);
    mToolBar->addWidget(mFiller);
    mToolBar->addAction(mExitAct);
    addToolBar(mToolBar);

    // setup menubar
    QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(mOpenAct);
    fileMenu->addSeparator();
    fileMenu->addAction(mExitAct);
    QMenu * editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(mSelectAct);
    editMenu->addAction(mClearAct);
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(mAboutAct);
    helpMenu->addAction(mAboutQtAct);

    setAcceptDrops(true);
}


// entry point
void
MainWindow::openDirectory()
{
    qDebug() << "MainWindow::openDirectory()";
    QString defaultDir = mLastPath.isEmpty() ? "" : mLastPath;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    defaultDir,
                                                    QFileDialog::ShowDirsOnly);
    startModelView(dir);
}


void
MainWindow::startModelView(const QString& dir)
{
    qDebug() << "MainWindow::startModelView()" << dir;

    // setup view
    if(mView == nullptr) {
        mView = new ThumbnailView(this);
        mView->setWindowTitle(tr("Files"));
        setCentralWidget(mView);
    }

    // setup dock
    if(mDock == nullptr) {
        mDock = new QDockWidget(tr("Tags"), this);
        mDock->setFeatures(QDockWidget::DockWidgetMovable);
        addDockWidget(Qt::RightDockWidgetArea, mDock);
    }

    if(mView && mModel) {
        mView->selectionModel()->clear();
        mView->scrollToTop();
    }

    QDir tempDir = QDir(QDir(dir).absolutePath());
    QString path = tempDir.canonicalPath();
    if(!path.isEmpty() && tempDir.exists()) {
        mLastPath = path;
        setWindowTitle(QCoreApplication::applicationName() + " — " + tempDir.dirName());

        mSelectAct->setEnabled(true);
        connect(mSelectAct, &QAction::triggered, mView, &QAbstractItemView::selectAll, Qt::UniqueConnection);
        mClearAct->setEnabled(true);
        connect(mClearAct, &QAction::triggered, mView, &QAbstractItemView::clearSelection, Qt::UniqueConnection);

        if(mModel == nullptr) {
            mModel = new ThumbnailModel(this);
        }
        mModel->loadData(path);

        mFilterTagWidget->setEnabled(true);
        mFilterTagWidget->setCompletions(QStringList() << mModel->mAllTags << "%UNTAGGED%");
        mFilterTagWidget->clear();
        mFilterPathWidget->setEnabled(true);
        mFilterPathWidget->clear();

        if(mFilterPathProxyModel == nullptr) {
            mFilterPathProxyModel = new QSortFilterProxyModel(this);
            mFilterPathProxyModel->setFilterRole(Qt::ToolTipRole);
            mFilterPathProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
            mFilterPathProxyModel->setSourceModel(mModel);
        }
        else {
            mFilterPathProxyModel->setFilterRegExp("");
            mFilterPathProxyModel->invalidate();
        }

        if(mFilterTagProxyModel == nullptr) {
            mFilterTagProxyModel = new FilterTagProxyModel(this);
            mFilterTagProxyModel->setSourceModel(mFilterPathProxyModel);
            mView->setModel(mFilterTagProxyModel);
        }
        else {
            mFilterTagProxyModel->setFilterFixedString("");
            mFilterTagProxyModel->invalidate();
        }

        connect(mView->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &MainWindow::handleSelection, Qt::UniqueConnection);
        connect(mView, &ThumbnailView::doubleClicked,
                this, &MainWindow::openFile, Qt::UniqueConnection);
        connect(mView, &ThumbnailView::returnPressed,
                this, &MainWindow::openFile, Qt::UniqueConnection);
        connect(mView, &ThumbnailView::openDirectoryTriggered,
                this, &MainWindow::openContainingFolder, Qt::UniqueConnection);

        statusBar()->showMessage(tr("%1 files").arg(mView->model()->rowCount()));
        refreshTagWidget();
    }
}


void
MainWindow::handleSelection(const QItemSelection &selected, const QItemSelection &deselected)
{
    qDebug() << "MainWindow::handleSelection()";
    for(const auto& index : selected.indexes()) {
        mModel->mSelected.insert(mFilterPathProxyModel->mapToSource(mFilterTagProxyModel->mapToSource(index)).row());
    }
    for(const auto& index : deselected.indexes()) {
        mModel->mSelected.remove(mFilterPathProxyModel->mapToSource(mFilterTagProxyModel->mapToSource(index)).row());
    }
    int selectedSize = mModel->mSelected.size();
    if(selectedSize != 0)
        statusBar()->showMessage(tr("%1 files selected").arg(selectedSize));
    else
        statusBar()->showMessage(tr("%1 files").arg(mView->model()->rowCount()));
    refreshTagWidget();
}


void
MainWindow::openFile(const QModelIndex &index)
{
    qDebug() << "MainWindow::openFile()";
    // for windows start (cmd) or Invoke-Item (powershell)
    int i = mFilterPathProxyModel->mapToSource(mFilterTagProxyModel->mapToSource(index)).row();
    QProcess::startDetached("xdg-open", QStringList() << mModel->mData[i].url.path());
}


void
MainWindow::openContainingFolder(const QModelIndex &index)
{
    qDebug() << "MainWindow::openFile()";
    int i = mFilterPathProxyModel->mapToSource(mFilterTagProxyModel->mapToSource(index)).row();
    QString dir = QFileInfo(mModel->mData[i].url.path()).dir().path();
    QProcess::startDetached("xdg-open", QStringList() << dir);
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
        mFilterTagWidget->setCompletions(QStringList() << mModel->mAllTags << "%UNTAGGED%");
    }
    else {
        statusBar()->showMessage(tr("Tag cannot be added, Process returned %1").arg(res));
    }
}


void
MainWindow::removeTag(const QString& tag)
{
    qDebug() << "MainWindow::remove()";
    int res = TMSU::removeTag(tag, mModel->getSelectedPaths());
    if(res == 0) {
        mModel->getTagsFromDB();
        refreshTagWidget();
        mFilterTagWidget->setCompletions(QStringList() << mModel->mAllTags << "%UNTAGGED%");
    }
    else {
        statusBar()->showMessage(tr("Tag cannot be removed, Process returned %1").arg(res));
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

    mTagWidget = new TagWidget(mModel->getSelectedTags(), mModel->getAllTags());
    connect(mTagWidget, &TagWidget::addTagClicked, this, &MainWindow::addTag);
    connect(mTagWidget, &TagWidget::removeTagClicked, this, &MainWindow::removeTag);
    if(!mModel->hasSelected()) {
        mTagWidget->lock();
    }
    mTagWidget->resize(mDockSize);
    mDock->setWidget(mTagWidget); // parent of mTagWidget is mDock now
}


void
MainWindow::pathFilterChanged()
{
    qDebug() << "MainWindow::pathFilterChanged()";
    mModel->mSelected.clear();
    mView->clearSelection();
    mView->scrollToTop();
    mFilterPathProxyModel->setFilterRegExp(mFilterPathWidget->text());
    statusBar()->showMessage(tr("%1 files").arg(mView->model()->rowCount()));
}

void
MainWindow::tagFilterChanged()
{
    qDebug() << "MainWindow::tagFilterChanged()";
    mModel->mSelected.clear();
    mView->clearSelection();
    mView->scrollToTop();
    QString query = mFilterTagWidget->text();
    mFilterTagProxyModel->mFilteredData.clear();

    QStringList output;
    int res = 0;
    if(query.toUpper() == "%UNTAGGED%")
        res = TMSU::untagged(mModel->mRootPath, output);
    else if(!query.isEmpty())
        res = TMSU::query(query, mModel->mRootPath, output);

    mFilterTagProxyModel->mFilteredData = QSet<QString>(output.begin(), output.end());

    if(res != 0)
        statusBar()->showMessage(tr("Query Failed, Process returned %1").arg(res));
    else {
        mFilterTagProxyModel->setFilterFixedString(query);
        statusBar()->showMessage(tr("%1 files").arg(mView->model()->rowCount()));
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


// entry point
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


void
MainWindow::readSettings()
{
    qDebug() << "MainWindow::readSettings()";

    mSettings->beginGroup("General");
    mLastPath = mSettings->value("lastPath", "").toString();
    menuBar()->setHidden(mSettings->value("menuHidden", false).toBool());
    mSettings->endGroup();

    mSettings->beginGroup("MainWindow");
    QRect rect = QGuiApplication::primaryScreen()->availableGeometry();
    resize(mSettings->value("size", QSize(rect.width()*3/4, rect.height()*3/4)).toSize());
    move(mSettings->value("pos", QPoint(200, 200)).toPoint());
    mSettings->endGroup();

    mSettings->beginGroup("Dock");
    mDockSize = mSettings->value("size").toSize();
    mSettings->endGroup();
}


void
MainWindow::writeSettings()
{
    qDebug() << "MainWindow::writeSettings()";

    mSettings->beginGroup("General");
    mSettings->setValue("lastPath", mLastPath);
    mSettings->setValue("menuHidden", menuBar()->isHidden());
    mSettings->endGroup();

    mSettings->beginGroup("MainWindow");
    mSettings->setValue("size", size());
    mSettings->setValue("pos", pos());
    mSettings->endGroup();

    if(mDock) {
        mSettings->beginGroup("Dock");
        mSettings->setValue("size", mDock->size());
        mSettings->endGroup();
    }
}


void
MainWindow::about()
{
    QMessageBox::about(this, tr("About Tag Browser"), tr("View and edit TMSU tags"));
}


void
MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(mSelectAct);
    menu.addAction(mClearAct);
    menu.exec(event->globalPos());
}


void
MainWindow::toggleMenuHide()
{
    QMenuBar *mbar = menuBar();
    if(mbar->isHidden())
        mbar->setHidden(false);
    else
        mbar->hide();
}


MainWindow::~MainWindow()
{
    qDebug() << "MainWindow::~MainWindow()";
    writeSettings();
}
