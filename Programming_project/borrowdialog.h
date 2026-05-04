#pragma once
#include <QDialog>
#include <QComboBox>
#include <QVector>
#include "models.h"

class BorrowDialog : public QDialog {
    Q_OBJECT
public:
    BorrowDialog(const QVector<User> &users,
                 const QVector<Book> &books,
                 QWidget *parent = nullptr);
    int selectedUserId() const;
    int selectedBookId() const;
private:
    QComboBox *m_userCombo;
    QComboBox *m_bookCombo;
    QVector<int> m_userIds;
    QVector<int> m_bookIds;
};
