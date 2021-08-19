#include <QCompleter>
#include <QDebug>

#include "FilterWidget.hpp"


FilterWidget::FilterWidget(const QString& placeholder, QWidget *parent) : QLineEdit(parent), mCompleter(nullptr)
{
    setPlaceholderText(placeholder);
    // connect(this, &QLineEdit::textChanged, this, &FilterWidget::handleTextChange, Qt::QueuedConnection);
}


FilterWidget::FilterWidget(const QString& placeholder, const QStringList& completions, QWidget *parent) : QLineEdit(parent), mCompleter(nullptr)
{
    setPlaceholderText(placeholder);
    setCompletions(completions);
    // connect(this, &QLineEdit::textChanged, this, &FilterWidget::handleTextChange, Qt::QueuedConnection);
}


void
FilterWidget::setCompletions(const QStringList& completions)
{
    if(mCompleter)
        delete mCompleter;
    mCompleter = new QCompleter(completions, this);
    mCompleter->setFilterMode(Qt::MatchContains);
    mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    setCompleter(mCompleter);
}


// void
// FilterWidget::focusOutEvent(QFocusEvent* event)
// {
//     (void)event;
//     emit returnPressed();
//     QLineEdit::focusOutEvent(event);
// }


// void
// FilterWidget::handleTextChange(const QString &text)
// {
//     int len = text.size();
//     if(len > 0 && text.at(len-1) == " ") {
//         emit QLineEdit::textChanged("");
//     }
//     else {
//         int i = text.lastIndexOf(" ");
//         if(i != -1)
//             emit QLineEdit::textChanged(text.mid(i+1));
//     }
// }

FilterWidget::~FilterWidget()
{
    qDebug() << "FilterWidget::~FilterWidget()";
    if(mCompleter)
        delete mCompleter;
}
