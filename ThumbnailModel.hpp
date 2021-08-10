#pragma once
#include <QWidget>
#include <QAbstractListModel>
#include <QMimeDatabase>
#include <QItemSelection>

#include <KFileItem>

#include "FileData.hpp"
#include "TagWidget.hpp"


class ThumbnailModel : public QAbstractListModel
{
public:
    ThumbnailModel(const QString &dir, QObject *parent = nullptr);
    ThumbnailModel(const QStringList &dirs, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void getFilesFromDir(const QString& dir);
    void startPreviewJob();

    void handleThumbSuccess(const KFileItem& item, const QPixmap& preview);
    void handleThumbFail(const KFileItem& item);
    void handleSelection(const QItemSelection &selected, const QItemSelection &deselected);
    void handleDoubleClick(const QModelIndex &index);

    void getTagsFromDB();
    QStringList getSelectedTags();
    QStringList getSelectedPaths();
    bool hasSelected();
    QStringList getAllTags();
private:
    QString mDir;
    QList<FileData> mData;
    QMimeDatabase mMimeDB;
    QString mDBPath;
    QStringList mAllTags;
    QSet<int> mSelected;
};
