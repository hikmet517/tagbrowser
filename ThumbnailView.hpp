#pragma once
#include <QWidget>
#include <QListView>
#include <QDebug>

class ThumbnailView : public QListView
{
Q_OBJECT
public:
    ThumbnailView(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event) override;
    void handleOpenFile();
    void handleOpenDirectory();
signals:
    void returnPressed(const QModelIndex &index);
    void openDirectoryTriggered(const QModelIndex &index);
private:
    QAction *mOpenFileAct;
    QAction *mOpenDirAct;
};
