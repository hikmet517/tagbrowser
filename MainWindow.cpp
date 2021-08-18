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
    : QMainWindow(), mModel(nullptr), mView(nullptr), mDock(nullptr),
      mTagWidget(nullptr), mFilterPathProxyModel(nullptr), mFilterTagProxyModel(nullptr)
{
    qDebug() << "MainWindow::MainWindow()";
    setupWidgets();
}


// entry point
MainWindow::MainWindow(const QString &dir)
    : QMainWindow(), mModel(nullptr), mView(nullptr), mDock(nullptr),
      mTagWidget(nullptr), mFilterPathProxyModel(nullptr), mFilterTagProxyModel(nullptr)
{
    qDebug() << "MainWindow::MainWindow(QString)";
    setupWidgets();
    startModelView(dir);
}


void
MainWindow::setupWidgets()
{
    qDebug() << "MainWindow::setupWidgets()";
    mSettings = new QSettings(this);
    setWindowTitle(QCoreApplication::applicationName());

    mOpenAct = new QAction(tr("&Open..."), this);
    mOpenAct->setShortcuts(QKeySequence::Open);
    mOpenAct->setToolTip(QString("Open Directory (%1)").arg(QKeySequence(QKeySequence::Open).toString()));
    mOpenAct->setIcon(QIcon::fromTheme("document-open"));
    connect(mOpenAct, &QAction::triggered, this, &MainWindow::openDirectory);

    mSelectAct = new QAction(tr("Select &All"), this);
    mSelectAct->setShortcut(tr("Alt+A"));
    mSelectAct->setToolTip("Select All (Alt+A)");
    mSelectAct->setIcon(QIcon::fromTheme("edit-select-all"));
    // mSelectAct->setDisabled(true);

    mClearAct = new QAction(tr("&Clear Selection"), this);
    mClearAct->setShortcut(tr("Alt+Shift+A"));
    mClearAct->setToolTip("Clear Selection (Alt+Shift+A)");
    mClearAct->setIcon(QIcon::fromTheme("edit-select-none"));
    // mClearAct->setDisabled(true);

    mFilterPathWidget = new FilterWidget("Filter by path using regex",
                                         {".*.(mp4|avi|mkv|mov|wmv|webm)$",
                                          ".*.(jpg|jpeg|png|bmp|gif|webp)"}, this);
    connect(mFilterPathWidget, &FilterWidget::returnPressed, this, &MainWindow::pathFilterChanged);
    // mFilterPathWidget->setDisabled(true);

    mFilterTagWidget = new FilterWidget("Filter by tag using boolean logic", this);
    connect(mFilterTagWidget, &FilterWidget::returnPressed, this, &MainWindow::tagFilterChanged);
    // mFilterTagWidget->setDisabled(true);

    mEmpty = new QWidget(this);
    mEmpty->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

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
    connect(mAboutQtAct, &QAction::triggered, this, &MainWindow::aboutQt);

    mHideMenuAct = new QAction("Toggle Menu Bar", this);
    mHideMenuAct->setShortcut(tr("Ctrl+M"));
    mHideMenuAct->setIcon(QIcon::fromTheme("show-menu"));
    connect(mHideMenuAct, &QAction::triggered, this, &MainWindow::toggleMenuHide);


    // setup toolbar
    mToolBar = new QToolBar(this);
    mToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mToolBar->setFloatable(false);
    mToolBar->setWindowTitle("Main Toolbar");
    mToolBar->addAction(mOpenAct);
    mToolBar->addAction(mSelectAct);
    mToolBar->addAction(mClearAct);
    mToolBar->addAction(mHideMenuAct);
    mToolBar->addWidget(mFilterPathWidget);
    mToolBar->addWidget(mFilterTagWidget);
    mToolBar->addWidget(mEmpty);
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


    // setup view
    mView = new ThumbnailView(this);
    mView->setWindowTitle("Files");
    setCentralWidget(mView);

    // setup dock
    mDock = new QDockWidget("Tags", this);
    mDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::RightDockWidgetArea, mDock);

    setAcceptDrops(true);
    readSettings();
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
    if(mView && mModel) {
        mView->selectionModel()->clear();
        mView->scrollToTop();
        mView->repaint();
    }

    QString path = QDir(dir).canonicalPath();
    if(!path.isEmpty() && QDir(path).exists()) {
        mLastPath = path;

        mSelectAct->setEnabled(true);
        connect(mSelectAct, &QAction::triggered, mView, &QAbstractItemView::selectAll, Qt::UniqueConnection);
        mClearAct->setEnabled(true);
        connect(mClearAct, &QAction::triggered, mView, &QAbstractItemView::clearSelection, Qt::UniqueConnection);

        if(mModel) delete mModel;
        mModel = new ThumbnailModel(path, this);

        mFilterTagWidget->setEnabled(true);
        mFilterTagWidget->setCompletions(QStringList() << mModel->mAllTags << "%UNTAGGED%");
        mFilterTagWidget->clear();
        mFilterPathWidget->setEnabled(true);
        mFilterPathWidget->clear();

        if(mFilterPathProxyModel) delete mFilterPathProxyModel;
        mFilterPathProxyModel = new QSortFilterProxyModel(this);
        mFilterPathProxyModel->setFilterRole(Qt::ToolTipRole);
        mFilterPathProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        mFilterPathProxyModel->setSourceModel(mModel);

        if(mFilterTagProxyModel) delete mFilterTagProxyModel;
        mFilterTagProxyModel = new FilterTagProxyModel(this);
        mFilterTagProxyModel->setSourceModel(mFilterPathProxyModel);
        mView->setModel(mFilterTagProxyModel);

        connect(mView->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &MainWindow::handleSelection, Qt::UniqueConnection);
        connect(mView, &ThumbnailView::doubleClicked,
                this, &MainWindow::openFile);
        connect(mView, &ThumbnailView::returnPressed,
                this, &MainWindow::openFile, Qt::UniqueConnection);

        statusBar()->showMessage(QString::number(mModel->mData.size()) + " files");
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
        statusBar()->showMessage(QString::number(selectedSize) + " files selected");
    else
        statusBar()->showMessage(QString::number(mModel->mData.size()) + " files");
    refreshTagWidget();
}


void
MainWindow::openFile(const QModelIndex &index)
{
    qDebug() << "MainWindow::openFile()";
    // for windows start (cmd) or Invoke-Item (powershell)
    QProcess p(this);
    int i = mFilterPathProxyModel->mapToSource(mFilterTagProxyModel->mapToSource(index)).row();
    p.setProgram("xdg-open");
    p.setArguments(QStringList() << mModel->mData[i].url.path());
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
        mFilterTagWidget->setCompletions(QStringList() << mModel->mAllTags << "%UNTAGGED%");
    }
    else {
        statusBar()->showMessage("Tag cannot be added, Process returned " + QString::number(res));
        // QMessageBox msg = QMessageBox(QMessageBox::Information,
        //                               "Tag cannot be added",
        //                               "Process returned " + QString::number(res));
        // msg.exec();
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
        statusBar()->showMessage("Tag cannot be removed, Process returned " + QString::number(res));
        // QMessageBox msg = QMessageBox(QMessageBox::Information,
        //                               "Tag cannot be removed",
        //                               "Process returned " + QString::number(res));
        // msg.exec();
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
    if(query.toUpper() == "%UNTAGGED%") {
        res = TMSU::untagged(mModel->mRootPath, output);
    }
    else if(!query.isEmpty()) {
        res = TMSU::query(QStringList() << "files" << query, mModel->mRootPath, output);
    }
    mFilterTagProxyModel->mFilteredData = QSet<QString>(output.begin(), output.end());

    if(res != 0) {
        statusBar()->showMessage("Query Failed, Process returned " + QString::number(res));
        // QMessageBox msg = QMessageBox(QMessageBox::Information,
        //                               "Query error",
        //                               "Process returned " + QString::number(res));
        // msg.exec();
        // return;
    }
    else
        mFilterTagProxyModel->setFilterFixedString(query);
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
                mFilterPathWidget->clear();
                mFilterTagWidget->clear();
                startModelView(path);
                break;
            }
        }
    }
    event->acceptProposedAction();
}


void MainWindow::readSettings()
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


void MainWindow::writeSettings()
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

    mSettings->beginGroup("Dock");
    mSettings->setValue("size", mDock->size());
    mSettings->endGroup();
}


void
MainWindow::about()
{
    // infoLabel->setText(tr("Invoked <b>Help|About</b>"));
    QMessageBox::about(this, tr("About Menu"),
            tr("The <b>Menu</b> example shows how to create "
               "menu-bar menus and context menus."));
}


void
MainWindow::aboutQt()
{
    // infoLabel->setText(tr("Invoked <b>Help|About Qt</b>"));
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
    delete mSettings;

    if(mModel) delete mModel;
    delete mView;
    delete mDock;               // also clears mTagWidget
    if(mFilterPathProxyModel) delete mFilterPathProxyModel;
    if(mFilterTagProxyModel) delete mFilterTagProxyModel;

    delete mOpenAct;
    delete mSelectAct;
    delete mClearAct;
    delete mEmpty;
    delete mExitAct;
    delete mFilterPathWidget;
    delete mFilterTagWidget;
    delete mToolBar;
}
