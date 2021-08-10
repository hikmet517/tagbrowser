#pragma once
#include <QWidget>
#include <QString>
#include <QStringList>

class QHBoxLayout;
class QLineEdit;
class QPushButton;
class QCompleter;


class TagEdit : public QWidget
{
public:
    TagEdit(const QStringList& completionList, QWidget *parent = nullptr);
    TagEdit(const QString& tag, QWidget *parent = nullptr);
    void handleClicked();
    QString getText();
    void setFocus();
    ~TagEdit();
protected:
    void keyPressEvent(QKeyEvent* event) override;
private:
    QHBoxLayout *mLayout;
    QLineEdit *mLineEdit;
    QCompleter *mCompleter;
    QPushButton *mButton;
    bool mReadOnly;
};
