#pragma once
#include <QWidget>
#include <QAbstractListModel>
#include <QAbstractItemModel>
#include <QMimeDatabase>
#include <QHash>

#include "FileData.hpp"
#include "TagWidget.hpp"
#include "ThumbnailJob.hpp"


class ThumbnailModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ThumbnailModel(const QString& dbpath, int sortBy=0, int sortStyle=0, QObject *parent = nullptr);
    ~ThumbnailModel();

    void resetToAll();
    void getTagsForData();

    void filterByRegex(const QString &text);
    void filterByFiles(const QSet<QString> &text);
    void sortFiles(int byName, int ascending);

    // overrides for model
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    QStringList getSelectedTags();
    QStringList getSelectedPaths();

    QString getDBPath() const {return mDBPath;};
    QString getRootPath() const {return mRootPath;};
    QStringList getAllTags() const {return mAllTags;};
    int getDataCount() const {return mData.count();};
    int getFullDataCount() const {return mFullData.count();};
    void addSelected(int i) {mSelected.insert(i);};
    void removeSelected(int i) {mSelected.remove(i);};
    void clearSelected() {mSelected.clear();};
    bool hasSelected() {return !mSelected.isEmpty();};
    int getSelectedSize() const {return mSelected.size();};
    QString getFilePath(int i) const {return mData[i].url.path();};

private:
    void createFileData(const QStringList& files);
    void clearData();
    void sort(QList<FileData>& list, int byName, int ascending);
    QStringList getAllFiles();
    void startPreviewJob();

    QList<FileData> mData;
    QList<FileData> mFullData;
    QString mDBPath;
    QString mRootPath;
    QStringList mAllTags;

    QSet<int> mSelected;
    QMimeDatabase mMimeDB;
    QThread mJob;
    int mProcessedCount = 0;

private slots:
    void handleThumbnail(const QString &filepath, const QPixmap &pm);

signals:
    void prepareThumbnail(int beg, int end);
};
