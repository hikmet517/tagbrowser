#pragma once
#include <QSortFilterProxyModel>
#include <QSet>
#include <QString>


class FilterTagProxyModel : public QSortFilterProxyModel
{
public:
    FilterTagProxyModel(QObject *parent = nullptr);
    QSet<QString> mFilteredData;
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    // bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};
