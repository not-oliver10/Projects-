#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QProgressBar>
#include <QVector>
#include "models.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddUser();
    void onAddBook();
    void onBorrowBook();
    void onReturnBook();

private:
    // Data
    QVector<User>         m_users;
    QVector<Book>         m_books;
    QVector<BorrowRecord> m_borrows;
    int m_nextUserId   = 1;
    int m_nextBookId   = 1;
    int m_nextBorrowId = 1;

    // Stat labels
    QLabel *m_lblTotalUsers;
    QLabel *m_lblTotalBooks;
    QLabel *m_lblBorrowed;
    QLabel *m_lblAvailable;

    // Tables
    QTableWidget *m_tblUsers;
    QTableWidget *m_tblBooks;
    QTableWidget *m_tblBorrowed;
    QWidget      *m_popularWidget;
    QVBoxLayout  *m_popularLayout;

    void setupUI();
    void seedData();
    void saveData();
    void loadData();
    void refreshAll();
    void refreshStats();
    void refreshUsers();
    void refreshBooks();
    void refreshBorrowed();
    void refreshPopular();

    QWidget *makeStatCard(const QString &label, QLabel *&valueLabel);
    QWidget *makeTopBar();
    QWidget *makeStatsBar();
    QWidget *makeButtonBar();
    QTabWidget *makeTabs();
    QTableWidget *makeTable(const QStringList &headers);
    void styleTable(QTableWidget *t);
};
