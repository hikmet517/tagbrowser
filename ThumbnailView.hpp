#pragma once
#include <QWidget>
#include <QListView>
#include <QDebug>

class ThumbnailView : public QListView
{
public:
    ThumbnailView(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event);
};
