#include <QDebug>
#include <QVBoxLayout>

#include "TagWidget.hpp"
#include "TagEdit.hpp"
#include "MainWindow.hpp"
#include "TMSU.hpp"


TagWidget::TagWidget(const QStringList& tags, const QStringList& allTags, QWidget *parent)
    : QWidget(parent), mLayout(new QVBoxLayout)
{
    mAllTags = allTags;
    clearLayout();
    QStringList tagsSorted(tags);
    tagsSorted.sort(Qt::CaseInsensitive);
    for(const auto& tag : tagsSorted) {
        TagEdit *widget = new TagEdit(tag, this);
        mLayout->addWidget(widget);
    }
    TagEdit *widget = new TagEdit(allTags, this);
    mLayout->addWidget(widget);
    mLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setLayout(mLayout);
}


void
TagWidget::clearLayout()
{
    QLayoutItem *child;
    while ((child = mLayout->takeAt(0)) != nullptr) {
        delete child->widget(); // delete the widget
        delete child;   // delete the layout item
    }
}


void
TagWidget::setFocus()
{
    qDebug() << "TagWidget::setFocus()";
    static_cast<TagEdit*>(mLayout->itemAt(mLayout->count()-1)->widget())->setFocus();
}


void
TagWidget::addTag(const QString& tag)
{
    qDebug() << "TagWidget::addTag()";
    MainWindow *mw = static_cast<MainWindow*>(parent()->parent());
    mw->addTag(tag);
}


void
TagWidget::removeTag(const QString &tag)
{
    qDebug() << "TagWidget::removeTag()";
    MainWindow *mw = static_cast<MainWindow*>(parent()->parent());
    mw->removeTag(tag);
}


TagWidget::~TagWidget()
{
    delete mLayout;
}
