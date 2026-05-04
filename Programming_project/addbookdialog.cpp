#include "addbookdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QMessageBox>

AddBookDialog::AddBookDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Add New Book");
    setFixedWidth(360);
    setStyleSheet("QDialog { background:#FFFFFF; font-family:'Segoe UI',Arial; }"
                  "QLineEdit, QSpinBox { border:1px solid #D3D1C7; border-radius:6px;"
                  " padding:7px 10px; font-size:13px; background:#FFFFFF; color:#2C2C2A; }"
                  "QLineEdit:focus, QSpinBox:focus { border-color:#888780; }"
                  "QLabel { font-size:12px; color:#5F5E5A; font-weight:600; }"
                  "QPushButton { border-radius:6px; font-size:13px; padding:8px 20px; }"
                  "QSpinBox::up-button, QSpinBox::down-button { width:20px; }");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 20);
    layout->setSpacing(16);

    auto *title = new QLabel("Add New Book");
    title->setStyleSheet("font-size:16px; font-weight:700; color:#2C2C2A;");
    layout->addWidget(title);

    auto *form = new QFormLayout;
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft);

    m_titleEdit  = new QLineEdit; m_titleEdit->setPlaceholderText("Book title");
    m_authorEdit = new QLineEdit; m_authorEdit->setPlaceholderText("Author name");
    m_stockSpin  = new QSpinBox;
    m_stockSpin->setRange(1, 999);
    m_stockSpin->setValue(1);

    form->addRow("Title",  m_titleEdit);
    form->addRow("Author", m_authorEdit);
    form->addRow("Stock",  m_stockSpin);
    layout->addLayout(form);

    auto *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btns, &QDialogButtonBox::accepted, this, [this]{
        if (m_titleEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Validation", "Please enter a title.");
            return;
        }
        accept();
    });
    connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(btns);
}

QString AddBookDialog::title()  const { return m_titleEdit->text().trimmed(); }
QString AddBookDialog::author() const { return m_authorEdit->text().trimmed(); }
int     AddBookDialog::stock()  const { return m_stockSpin->value(); }
