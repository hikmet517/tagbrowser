#pragma once
#include <QWidget>
#include <QListView>
#include <QDebug>

class ThumbnailView : public QListView
{
Q_OBJECT
public:
    ThumbnailView(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event);
signals:
    void returnPressed(const QModelIndex &index);
};
