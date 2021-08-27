#pragma once
#include <QDebug>
#include <QWidget>
#include <QMainWindow>
#include <QItemSelection>


class QSortFilterProxyModel;
class ThumbnailModel;
class ThumbnailView;
class TagWidget;
class FilterWidget;
class FilterTagProxyModel;
class QCompleter;
class QSettings;

class MainWindow : public QMainWindow
{
public:
    MainWindow();
    ~MainWindow();
    void startModelView(const QString& dir);

private slots:
    void addTag(const QString& tag);
    void removeTag(const QString& tag);

    void pathFilterChanged();
    void tagFilterChanged();

    void handleSelection(const QItemSelection &selected, const QItemSelection &deselected);
    void openFile(const QModelIndex &index);
    void openContainingFolder(const QModelIndex &index);

    void about();
    void toggleMenuHide();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

    // dnd (https://doc.qt.io/qt-5/dnd.html)
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void setupBasics();
    void openDirectory();

    void refreshTagWidget();
    void readSettings();
    void writeSettings();


    QString mLastPath;
    QSize mDockSize;

    QSettings *mSettings;

    ThumbnailView *mView;
    ThumbnailModel *mModel;
    QDockWidget *mDock;
    TagWidget *mTagWidget;

    QAction *mOpenAct;
    QAction *mSelectAct;
    QAction *mClearAct;
    QWidget *mFiller;
    QAction *mExitAct;
    QAction *mAboutAct;
    QAction *mAboutQtAct;
    QAction *mHideMenuAct;
    QToolBar *mToolBar;

    FilterWidget *mFilterPathWidget;
    FilterWidget *mFilterTagWidget;
    QSortFilterProxyModel *mFilterPathProxyModel;
    FilterTagProxyModel *mFilterTagProxyModel;
};
