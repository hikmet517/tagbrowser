#include "ThumbnailView.hpp"
#include "ThumbnailModel.hpp"
#include <QDebug>
#include <QScrollBar>


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
ThumbnailView::setModel(QAbstractItemModel *model)
{
    qDebug() << "ThumbnailView::setModel()";
    QListView::setModel(model);
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged,
            (ThumbnailModel*)this->model(), &ThumbnailModel::handleSelection);
    connect(this, &ThumbnailView::doubleClicked,
            (ThumbnailModel*)this->model(), &ThumbnailModel::handleDoubleClick);
}
