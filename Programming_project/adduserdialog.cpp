#include "adduserdialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QMessageBox>

AddUserDialog::AddUserDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Add New User");
    setFixedWidth(360);
    setStyleSheet("QDialog { background:#FFFFFF; font-family:'Segoe UI',Arial; }"
                  "QLineEdit { border:1px solid #D3D1C7; border-radius:6px; padding:7px 10px;"
                  " font-size:13px; background:#FFFFFF; color:#2C2C2A; }"
                  "QLineEdit:focus { border-color:#888780; }"
                  "QLabel { font-size:12px; color:#5F5E5A; font-weight:600; }"
                  "QPushButton { border-radius:6px; font-size:13px; padding:8px 20px; }"
                  "QPushButton[text='OK'] { background:#2C2C2A; color:#FFFFFF; border:none; }"
                  "QPushButton[text='Cancel'] { background:#FFFFFF; color:#2C2C2A;"
                  " border:1px solid #D3D1C7; }");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 20);
    layout->setSpacing(16);

    auto *title = new QLabel("Add New User");
    title->setStyleSheet("font-size:16px; font-weight:700; color:#2C2C2A;");
    layout->addWidget(title);

    auto *form = new QFormLayout;
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft);

    m_nameEdit  = new QLineEdit; m_nameEdit->setPlaceholderText("e.g. Layla Ibrahim");
    m_emailEdit = new QLineEdit; m_emailEdit->setPlaceholderText("layla@email.com");

    form->addRow("Full Name", m_nameEdit);
    form->addRow("Email",     m_emailEdit);
    layout->addLayout(form);

    auto *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btns, &QDialogButtonBox::accepted, this, [this]{
        if (m_nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Validation", "Please enter a name.");
            return;
        }
        accept();
    });
    connect(btns, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(btns);
}

QString AddUserDialog::name()  const { return m_nameEdit->text().trimmed(); }
QString AddUserDialog::email() const { return m_emailEdit->text().trimmed(); }
