#pragma once

#include <QWidget>
#include <QSet>
#include <QSortFilterProxyModel>


class FilterTagProxyModel : public QSortFilterProxyModel
{
public:
    FilterTagProxyModel(QObject *parent = nullptr);
    QSet<QString> mFilteredData;
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    // bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};
