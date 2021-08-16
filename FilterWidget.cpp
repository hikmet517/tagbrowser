#include <QCompleter>
#include "FilterWidget.hpp"


FilterWidget::FilterWidget(const QString& placeholder, QWidget *parent) : QLineEdit(parent), mCompleter(nullptr)
{
    setPlaceholderText(placeholder);
}


FilterWidget::FilterWidget(const QString& placeholder, const QStringList& completions, QWidget *parent) : QLineEdit(parent), mCompleter(nullptr)
{
    setPlaceholderText(placeholder);
    setCompletions(completions);
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


void
FilterWidget::focusOutEvent(QFocusEvent* event)
{
    (void)event;
    emit returnPressed();
    QLineEdit::focusOutEvent(event);
}


FilterWidget::~FilterWidget()
{
    if(mCompleter)
        delete mCompleter;
}
