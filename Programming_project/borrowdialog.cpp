#include "borrowdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QMessageBox>

BorrowDialog::BorrowDialog(const QVector<User> &users,
                           const QVector<Book> &books,
                           QWidget *parent) : QDialog(parent) {
    setWindowTitle("Borrow a Book");
    setFixedWidth(380);
    setStyleSheet("QDialog { background:#FFFFFF; font-family:'Segoe UI',Arial; }"
                  "QComboBox { border:1px solid #D3D1C7; border-radius:6px; padding:7px 10px;"
                  " font-size:13px; background:#FFFFFF; color:#2C2C2A; }"
                  "QComboBox:focus { border-color:#888780; }"
                  "QComboBox::drop-down { border:none; width:24px; }"
                  "QLabel { font-size:12px; color:#5F5E5A; font-weight:600; }"
                  "QPushButton { border-radius:6px; font-size:13px; padding:8px 20px; }");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 20);
    layout->setSpacing(16);

    auto *title = new QLabel("Borrow a Book");
    title->setStyleSheet("font-size:16px; font-weight:700; color:#2C2C2A;");
    layout->addWidget(title);

    auto *form = new QFormLayout;
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft);

    m_userCombo = new QComboBox;
    for (auto &u : users) {
        m_userCombo->addItem(u.name);
        m_userIds.append(u.id);
    }

    m_bookCombo = new QComboBox;
    for (auto &b : books) {
        if (b.available > 0) {
            m_bookCombo->addItem(QString("%1  (%2 left)").arg(b.title).arg(b.available));
            m_bookIds.append(b.id);
        }
    }

    form->addRow("User", m_userCombo);
    form->addRow("Book", m_bookCombo);
    layout->addLayout(form);

    auto *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btns, &QDialogButtonBox::accepted, this, [this]{
        if (m_bookCombo->count() == 0) {
            QMessageBox::warning(this, "No Books", "No books available to borrow.");
            return;
        }
        accept();
    });
    connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(btns);
}

int BorrowDialog::selectedUserId() const {
    int i = m_userCombo->currentIndex();
    return (i >= 0 && i < m_userIds.size()) ? m_userIds[i] : -1;
}

int BorrowDialog::selectedBookId() const {
    int i = m_bookCombo->currentIndex();
    return (i >= 0 && i < m_bookIds.size()) ? m_bookIds[i] : -1;
}
