#include "TagEdit.hpp"
#include "TagWidget.hpp"
#include "TMSU.hpp"

#include <QIcon>
#include <QDebug>
#include <QKeyEvent>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QCompleter>
#include <qwidget.h>


// editable
TagEdit::TagEdit(const QStringList& completionList, QWidget *parent) :
    QWidget(parent), mLayout(nullptr), mLineEdit(nullptr), mCompleter(nullptr), mButton(nullptr)
{
    mLayout = new QHBoxLayout(this);
    mLineEdit = new QLineEdit(this);
    mCompleter = new QCompleter(completionList, this);
    mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    mCompleter->setFilterMode(Qt::MatchContains);
    mLineEdit->setCompleter(mCompleter);
    mButton = new QPushButton(QIcon::fromTheme("list-add"), "", this);

    mLayout->addWidget(mLineEdit);
    mLayout->addWidget(mButton);
    mReadOnly = false;
    connect(mButton, &QPushButton::clicked, this, &TagEdit::handleClicked);
}


// read-only
TagEdit::TagEdit(const QString& tag, QWidget *parent) :
    QWidget(parent), mLayout(nullptr), mLineEdit(nullptr), mCompleter(nullptr), mButton(nullptr)
{
    mLayout = new QHBoxLayout(this);
    mLineEdit = new QLineEdit(tag, this);
    mLineEdit->setReadOnly(true);
    mButton = new QPushButton(QIcon::fromTheme("list-remove"), "", this);
    mLayout->addWidget(mLineEdit);
    mLayout->addWidget(mButton);
    mReadOnly = true;
    connect(mButton, &QPushButton::clicked, this, &TagEdit::handleClicked);
}


void
TagEdit::setFocus()
{
    qDebug() << "TagEdit::setFocus()";
    QWidget::setFocus();
    mLineEdit->setFocus();
}


void
TagEdit::handleClicked()
{
    QString tag = mLineEdit->text();
    if(mReadOnly) {
        emit removeTagClicked(tag);
    }
    else {
        emit addTagClicked(tag);
    }
}


QString
TagEdit::getText()
{
    return mLineEdit->text().trimmed();
}


void
TagEdit::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if(key == Qt::Key_Enter or key == Qt::Key_Return)
        mButton->animateClick();
    else
        QWidget::keyPressEvent(event);
}


TagEdit::~TagEdit()
{
    delete mLayout;
    delete mLineEdit;
    if (mCompleter)
        delete mCompleter;
    delete mButton;
}
