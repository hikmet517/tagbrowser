#include <QDebug>
#include <QVBoxLayout>

#include "MainWindow.hpp"
#include "TMSU.hpp"
#include "TagEdit.hpp"
#include "TagWidget.hpp"


TagWidget::TagWidget(const QStringList& tags, const QStringList& allTags, QWidget *parent)
    : QWidget(parent), mLayout(new QVBoxLayout)
{
    mAllTags = allTags;
    // clearLayout();
    QStringList tagsSorted(tags);
    tagsSorted.sort(Qt::CaseInsensitive);
    for(const auto& tag : tagsSorted) {
        TagEdit *widget = new TagEdit(tag, this);
        connect(widget, &TagEdit::addTagClicked, this, &TagWidget::addTagClicked);
        connect(widget, &TagEdit::removeTagClicked, this, &TagWidget::removeTagClicked);
        mLayout->addWidget(widget);
        mWidgets.append(widget);
    }
    TagEdit *widget = new TagEdit(allTags, this);
    connect(widget, &TagEdit::addTagClicked, this, &TagWidget::addTagClicked);
    connect(widget, &TagEdit::removeTagClicked, this, &TagWidget::removeTagClicked);
    mWidgets.append(widget);

    mLayout->addWidget(widget);
    mLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    setLayout(mLayout);
}


QSize
TagWidget::sizeHint() const
{
    return size();
}

void
TagWidget::lock()
{
    mWidgets[mWidgets.size()-1]->setDisabled(true);
}

// void
// TagWidget::clearLayout()
// {
//     QLayoutItem *child;
//     while ((child = mLayout->takeAt(0)) != nullptr) {
//         delete child->widget(); // delete the widget
//         delete child;   // delete the layout item
//     }
// }


void
TagWidget::setFocus()
{
    qDebug() << "TagWidget::setFocus()";
    QWidget::setFocus();
    mWidgets[mWidgets.size()-1]->setFocus();
}


TagWidget::~TagWidget()
{
    qDebug() << "TagWidget::~TagWidget()";
    for(auto widget : mWidgets)
        delete widget;
    delete mLayout;
}
