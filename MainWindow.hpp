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
    MainWindow(const QString& dir);
    ~MainWindow();

    void refreshTagWidget();
    void readSettings();
    void writeSettings();

    // dnd (https://doc.qt.io/qt-5/dnd.html)
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent* event) override;


private slots:
    void addTag(const QString& tag);
    void removeTag(const QString& tag);

    void pathFilterChanged();
    void tagFilterChanged();

    void handleSelection(const QItemSelection &selected, const QItemSelection &deselected);
    void openFile(const QModelIndex &index);

    void about();
    void aboutQt();

    void toggleMenuHide();

private:
    void setupWidgets();
    void openDirectory();
    void startModelView(const QString& dir);

    QString mLastPath;
    QSize mDockSize;

    QSettings *mSettings;

    ThumbnailModel *mModel;
    ThumbnailView *mView;
    QDockWidget *mDock;
    TagWidget *mTagWidget;

    QAction *mOpenAct;
    QAction *mSelectAct;
    QAction *mClearAct;
    QWidget *mEmpty;
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
