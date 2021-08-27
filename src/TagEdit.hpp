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
    Q_OBJECT
public:
    TagEdit(const QStringList& completionList, QWidget *parent = nullptr);
    TagEdit(const QString& tag, QWidget *parent = nullptr);
    void handleClicked();
    QString getText();
    void setFocus();
    ~TagEdit();
signals:
    void addTagClicked(const QString& tag);
    void removeTagClicked(const QString& tag);
protected:
    void keyPressEvent(QKeyEvent* event) override;
private:
    QHBoxLayout *mLayout;
    QLineEdit *mLineEdit;
    QCompleter *mCompleter;
    QPushButton *mButton;
    bool mReadOnly;
};
