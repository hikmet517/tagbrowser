#pragma once
#include <QWidget>

class QVBoxLayout;


class TagWidget : public QWidget
{
public:
    TagWidget(const QStringList& tags, const QStringList& allTags, QWidget *parent=nullptr);
    ~TagWidget();
    void addTag(const QString& tag);
    void removeTag(const QString& tag);
    void clearLayout();
    void setFocus();
private:
    QVBoxLayout *mLayout;
    QStringList mAllTags;
};
