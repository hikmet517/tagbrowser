#pragma once
#include <QLineEdit>
#include <QWidget>


class FilterWidget : public QLineEdit
{
    Q_OBJECT
public:
    FilterWidget(QWidget *parent = nullptr);
// signals:
//     void filterChanged();
};
