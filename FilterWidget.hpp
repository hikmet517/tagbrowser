#pragma once
#include <QLineEdit>
#include <QWidget>


class FilterWidget : public QLineEdit
{
    Q_OBJECT
public:
    FilterWidget(const QString& placeholder, QWidget *parent = nullptr);
    FilterWidget(const QString& placeholder, const QStringList& completions, QWidget *parent = nullptr);
    ~FilterWidget();
    void setCompletions(const QStringList& completions);
    void focusOutEvent(QFocusEvent* event);
private:
    QCompleter *mCompleter;
};
