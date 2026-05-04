#pragma once
#include <QDialog>
#include <QComboBox>
#include <QVector>
#include "models.h"

class ReturnDialog : public QDialog {
    Q_OBJECT
public:
    ReturnDialog(const QVector<BorrowRecord*> &active,
                 const QVector<User>          &users,
                 const QVector<Book>          &books,
                 QWidget *parent = nullptr);
    int selectedBorrowId() const;
private:
    QComboBox    *m_combo;
    QVector<int>  m_borrowIds;
};
