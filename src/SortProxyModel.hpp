#pragma once
#include <QSortFilterProxyModel>
#include <random>
#include <chrono>

class SortProxyModel : public QSortFilterProxyModel {
public:
    void setSortParams(int by, int style) {
        mSortBy = by;
        mSortStyle = style;
    };

    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
private:
    int mSortBy;
    int mSortStyle;
};
