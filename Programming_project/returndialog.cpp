#include "returndialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>

ReturnDialog::ReturnDialog(const QVector<BorrowRecord*> &active,
                           const QVector<User>          &users,
                           const QVector<Book>          &books,
                           QWidget *parent) : QDialog(parent) {
    setWindowTitle("Return a Book");
    setFixedWidth(400);
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

    auto *title = new QLabel("Return a Book");
    title->setStyleSheet("font-size:16px; font-weight:700; color:#2C2C2A;");
    layout->addWidget(title);

    auto *form = new QFormLayout;
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft);

    m_combo = new QComboBox;
    for (auto *br : active) {
        QString userName = "Unknown", bookTitle = "Unknown";
        for (auto &u : users) if (u.id == br->userId) { userName  = u.name;  break; }
        for (auto &b : books) if (b.id == br->bookId) { bookTitle = b.title; break; }
        m_combo->addItem(QString("%1  —  %2  (%3)")
                         .arg(userName, bookTitle,
                              br->borrowDate.toString("dd MMM yyyy")));
        m_borrowIds.append(br->id);
    }

    form->addRow("Select record", m_combo);
    layout->addLayout(form);

    auto *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btns, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(btns);
}

int ReturnDialog::selectedBorrowId() const {
    int i = m_combo->currentIndex();
    return (i >= 0 && i < m_borrowIds.size()) ? m_borrowIds[i] : -1;
}
