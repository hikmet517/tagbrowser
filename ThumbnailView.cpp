#include "ThumbnailView.hpp"
#include "ThumbnailModel.hpp"
#include <QDebug>
#include <QScrollBar>
#include <QKeyEvent>


ThumbnailView::ThumbnailView(QWidget *parent) : QListView(parent)
{
    qDebug() << "ThumbnailView::ThumbnailView()";
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setUniformItemSizes(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setVerticalScrollMode(ScrollPerPixel);
    setHorizontalScrollMode(ScrollPerPixel);
    verticalScrollBar()->setSingleStep(40);
}

void
ThumbnailView::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if(key == Qt::Key_Return || key == Qt::Key_Enter)
        emit returnPressed(selectionModel()->currentIndex());
    QAbstractItemView::keyPressEvent(event);
}
