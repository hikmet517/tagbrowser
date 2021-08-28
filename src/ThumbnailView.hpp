#pragma once
#include <QWidget>
#include <QListView>
#include <QDebug>

class ThumbnailView : public QListView
{
Q_OBJECT
public:
    ThumbnailView(QWidget *parent = nullptr);
    void handleOpenFile();
    void handleOpenDirectory();
protected:
    void keyPressEvent(QKeyEvent *event) override;
signals:
    void returnPressed(const QModelIndex &index);
    void openDirectoryTriggered(const QModelIndex &index);
private:
    QAction *mOpenFileAct;
    QAction *mOpenDirAct;
};
