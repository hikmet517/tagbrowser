#include "ThumbnailView.hpp"
#include "ThumbnailModel.hpp"
#include <QDebug>
#include <QScrollBar>
#include <QKeyEvent>
#include <QMenu>
#include <QStyle>


ThumbnailView::ThumbnailView(QWidget *parent) : QListView(parent)
{
    qDebug() << "ThumbnailView::ThumbnailView()";
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setStyleSheet("QListView::item { width: 256px; }");
    setUniformItemSizes(true);
    setSpacing(2);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setVerticalScrollMode(ScrollPerPixel);
    setHorizontalScrollMode(ScrollPerPixel);
    verticalScrollBar()->setSingleStep(40);
    setSelectionRectVisible(true);

    mOpenFileAct = new QAction(tr("Open file"), this);
    mOpenFileAct->setIcon(QIcon::fromTheme("document-open"));
    connect(mOpenFileAct, &QAction::triggered, this, &ThumbnailView::handleOpenFile);

    mOpenDirAct = new QAction(tr("Open containing folder"), this);
    mOpenDirAct->setIcon(QIcon::fromTheme("folder-open"));
    connect(mOpenDirAct, &QAction::triggered, this, &ThumbnailView::handleOpenDirectory);

    addAction(mOpenFileAct);
    addAction(mOpenDirAct);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}


void
ThumbnailView::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if(key == Qt::Key_Return || key == Qt::Key_Enter)
        emit returnPressed(selectionModel()->currentIndex());
    QAbstractItemView::keyPressEvent(event);
}


void
ThumbnailView::handleOpenFile()
{
    emit returnPressed(selectionModel()->currentIndex());
}


void
ThumbnailView::handleOpenDirectory()
{
    emit openDirectoryTriggered(selectionModel()->currentIndex());
}
