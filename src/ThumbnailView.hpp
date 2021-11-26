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
    // int offsetX(){return horizontalOffset();};
    // int offsetY(){return verticalOffset();};
protected:
    void keyPressEvent(QKeyEvent *event) override;
    // void paintEvent(QPaintEvent *e) override;
signals:
    void returnPressed(const QModelIndex &index);
    void openDirectoryTriggered(const QModelIndex &index);
private:
    QAction *mOpenFileAct;
    QAction *mOpenDirAct;
};
