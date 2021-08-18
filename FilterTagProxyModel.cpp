#include <QDebug>

#include "FilterTagProxyModel.hpp"


FilterTagProxyModel::FilterTagProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}


bool
FilterTagProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(filterRegExp().isEmpty())
        return true;
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QString data = sourceModel()->data(index, Qt::ToolTipRole).toString();
    return mFilteredData.contains(data);
}

// bool
// FilterTagProxyModel::lessThan(const QModelIndex &left,
//                                const QModelIndex &right) const
// {
//     QVariant leftData = sourceModel()->data(left);
//     QVariant rightData = sourceModel()->data(right);

// }
