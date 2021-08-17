#pragma once
#include <QWidget>

class QVBoxLayout;
class TagEdit;


class TagWidget : public QWidget
{
Q_OBJECT
public:
    TagWidget(const QStringList& tags, const QStringList& allTags, QWidget *parent=nullptr);
    ~TagWidget();
    QSize sizeHint() const;
    void lock();
    // void clearLayout();
    void setFocus();
signals:
    void addTagClicked(const QString& tag);
    void removeTagClicked(const QString& tag);
private:
    QList<TagEdit*> mWidgets;
    QVBoxLayout *mLayout;
    QStringList mAllTags;
};
