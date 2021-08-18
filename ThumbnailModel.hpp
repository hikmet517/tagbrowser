#pragma once
#include <QWidget>
#include <QAbstractListModel>
#include <QMimeDatabase>

#include <KFileItem>

#include "FileData.hpp"
#include "TagWidget.hpp"


class ThumbnailModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ThumbnailModel(const QString &dir, QObject *parent = nullptr);
    ThumbnailModel(const QStringList &dirs, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void getFilesFromDir(const QString& dir);
    void startPreviewJob();

    void handleThumbSuccess(const KFileItem& item, const QPixmap& preview);
    void handleThumbFail(const KFileItem& item);

    void getTagsFromDB();
    QStringList getSelectedTags();
    QStringList getSelectedPaths();
    bool hasSelected();
    QStringList getAllTags();

    // fix here
    QList<FileData> mData;
    QSet<int> mSelected;
    QString mDir;
    QString mRootPath;
    QMimeDatabase mMimeDB;
    QString mDBPath;
    QStringList mAllTags;

private:
};
