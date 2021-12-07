#pragma once
#include <QWidget>
#include <QAbstractListModel>
#include <QAbstractItemModel>
#include <QMimeDatabase>

#include "FileData.hpp"
#include "TagWidget.hpp"
#include "ThumbnailJob.hpp"


class ThumbnailModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ThumbnailModel(QObject *parent = nullptr);
    ~ThumbnailModel();
    void loadData(const QString &dir);

    // getters
    void getTagsFromDB();
    QStringList getSelectedTags();
    QStringList getSelectedPaths();
    QStringList getAllTags();
    QString getDBPath() const {return mDBPath;};
    QString getRootPath() const {return mRootPath;};
    QStringList getAllTags() const {return mAllTags;};
    int getDataCount() const {return mData.count();};
    void addSelected(int i) {mSelected.insert(i);};
    void removeSelected(int i) {mSelected.remove(i);};
    void clearSelected() {mSelected.clear();};
    bool hasSelected() {return !mSelected.isEmpty();};
    int getSelectedSize() const {return mSelected.size();};
    QString getFilePath(int i) const {return mData[i].url.path();};

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private slots:
    void handleThumbnail(const QString &filepath, const QPixmap &pm);

private:
    QList<FileData> getFilesFromDir(const QString& dir);
    void startPreviewJob();

    QThread mJob;
    QList<FileData> mData;
    QSet<int> mSelected;
    QString mDir;
    QString mRootPath;
    QMimeDatabase mMimeDB;
    QString mDBPath;
    QStringList mAllTags;

signals:
    void prepareThumbnail(QString filepath) const;
};
