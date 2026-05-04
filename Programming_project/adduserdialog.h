#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QString>

class AddUserDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddUserDialog(QWidget *parent = nullptr);
    QString name()  const;
    QString email() const;
private:
    QLineEdit *m_nameEdit;
    QLineEdit *m_emailEdit;
};
