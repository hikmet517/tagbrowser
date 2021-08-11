#pragma once
#include <QWidget>

class QVBoxLayout;


class TagWidget : public QWidget
{
Q_OBJECT
public:
    TagWidget(const QStringList& tags, const QStringList& allTags, QWidget *parent=nullptr);
    ~TagWidget();
    // void clearLayout();
    void setFocus();
signals:
    void addTagClicked(const QString& tag);
    void removeTagClicked(const QString& tag);
private:
    QVBoxLayout *mLayout;
    QStringList mAllTags;
};
