#include "SortProxyModel.hpp"
#include "FileData.hpp"

#include <QDebug>
#include <QDateTime>

#include <qglobal.h>
#include <random>

bool
SortProxyModel::lessThan(const QModelIndex &lefti, const QModelIndex &righti) const
{
    if(sortColumn() != 0)
        qDebug() << "sortColumn() NOT 0";

    // by name
    if(mSortBy == 0) {
        const QString& left = sourceModel()->data(lefti, Qt::ToolTipRole).toString();
        const QString& right = sourceModel()->data(righti, Qt::ToolTipRole).toString();
        if (mSortStyle == 0) {
            return left < right;
        }
        else {
            return right < left;
        }
    }
    // by date
    else if(mSortBy == 1) {
        const QDateTime& left = sourceModel()->data(lefti, Qt::UserRole).value<FileData>().modified;
        const QDateTime& right = sourceModel()->data(righti, Qt::UserRole).value<FileData>().modified;

        if (mSortStyle == 0) {
            return left > right;
        }
        else {
            return left < right;
        }
    }
    else if(mSortBy == 2) {
        qint64 left = sourceModel()->data(lefti, Qt::UserRole).value<FileData>().size;
        qint64 right = sourceModel()->data(righti, Qt::UserRole).value<FileData>().size;

        if (mSortStyle == 0) {
            return left < right;
        }
        else {
            return left > right;
        }
    }

    // else if(mSortBy == 3) {
    //     std::random_device rd;
    //     std::mt19937 gen(rd());
    //     std::uniform_int_distribution<> dis(0, 1);
    //     return dis(gen) == 0;
    // }
    return true;
};
