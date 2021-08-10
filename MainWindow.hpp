#pragma once
#include <QWidget>
#include <QMainWindow>

class ThumbnailModel;
class ThumbnailView;
class TagWidget;


class MainWindow : public QMainWindow
{
public:
    MainWindow();
    MainWindow(const QString& dir);
    ~MainWindow();
    void addTag(const QString& tag);
    void removeTag(const QString& tag);
    void refreshTagWidget();

    // dnd (https://doc.qt.io/qt-5/dnd.html)
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void createMenus();
    void openDirectory();
    void startModelView(const QString& dir);

    QString mLastDir;

    ThumbnailModel *mModel;
    ThumbnailView *mView;
    QDockWidget *mDock;
    TagWidget *mTagWidget;

    QAction *mOpenAct;
    QAction *mExitAct;
    QAction *mClearAct;
    QAction *mSelectAct;
    QWidget *mEmpty;
    QToolBar *mToolBar;
};
