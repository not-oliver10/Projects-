#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QString>

class AddBookDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddBookDialog(QWidget *parent = nullptr);
    QString title()  const;
    QString author() const;
    int     stock()  const;
private:
    QLineEdit *m_titleEdit;
    QLineEdit *m_authorEdit;
    QSpinBox  *m_stockSpin;
};
