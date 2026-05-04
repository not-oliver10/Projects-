#include "mainwindow.h"
#include "addbookdialog.h"
#include "adduserdialog.h"
#include "borrowdialog.h"
#include "returndialog.h"
#include <QHeaderView>
#include <QScrollArea>
#include <QFrame>
#include <QFont>
#include <QDate>
#include <algorithm>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>

// ─── Helpers ──────────────────────────────────────────────────────────────────

static QLabel *badge(const QString &text, const QString &color, const QString &bg) {
    auto *l = new QLabel(text);
    l->setAlignment(Qt::AlignCenter);
    l->setFixedHeight(22);
    l->setStyleSheet(QString(
        "QLabel { background:%1; color:%2; border-radius:11px;"
        " padding:0 10px; font-size:11px; font-weight:600; }").arg(bg, color));
    return l;
}

// ─── Constructor ──────────────────────────────────────────────────────────────

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Library Management System");
    setMinimumSize(1000, 680);

    setStyleSheet(R"(
        QMainWindow, QWidget { background: #F8F7F4; font-family: 'Segoe UI', Arial; }
        QTabWidget::pane { border: 1px solid #E2E0D8; border-radius: 8px;
                           background: #FFFFFF; margin-top: -1px; }
        QTabBar::tab { background: #EDECEA; color: #888780; border: 1px solid #E2E0D8;
                       border-bottom: none; padding: 8px 18px; border-radius: 6px 6px 0 0;
                       font-size: 13px; margin-right: 2px; }
        QTabBar::tab:selected { background: #FFFFFF; color: #2C2C2A; font-weight: 600; }
        QTabBar::tab:hover:!selected { background: #E8E6E2; }
        QTableWidget { background: #FFFFFF; border: none; gridline-color: #EDECEA;
                       font-size: 13px; selection-background-color: #EAF3DE; color: #2C2C2A; }
        QTableWidget::item { padding: 6px 12px; border-bottom: 1px solid #F1EFE8; }
        QTableWidget::item:selected { color: #2C2C2A; }
        QHeaderView::section { background: #F8F7F4; color: #888780; font-size: 12px;
                               font-weight: 600; padding: 8px 12px; border: none;
                               border-bottom: 1px solid #E2E0D8; }
        QPushButton { border-radius: 6px; font-size: 13px; padding: 8px 18px;
                      font-family: 'Segoe UI', Arial; }
        QScrollBar:vertical { width: 6px; background: transparent; }
        QScrollBar::handle:vertical { background: #D3D1C7; border-radius: 3px; }
    )");

    setupUI();
    loadData();
    refreshAll();
}

MainWindow::~MainWindow() {
    saveData();
}

// ─── UI Setup ─────────────────────────────────────────────────────────────────

void MainWindow::setupUI() {
    auto *central = new QWidget;
    setCentralWidget(central);
    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(24, 20, 24, 20);
    root->setSpacing(16);

    root->addWidget(makeTopBar());
    root->addWidget(makeStatsBar());
    root->addWidget(makeButtonBar());
    root->addWidget(makeTabs(), 1);
}

QWidget *MainWindow::makeTopBar() {
    auto *w = new QWidget;
    auto *h = new QHBoxLayout(w);
    h->setContentsMargins(0,0,0,0);

    auto *title = new QLabel("Library Management System");
    QFont f = title->font();
    f.setPointSize(18); f.setWeight(QFont::DemiBold);
    title->setFont(f);
    title->setStyleSheet("color: #2C2C2A;");

    auto *sub = new QLabel(QDate::currentDate().toString("dddd, d MMMM yyyy"));
    sub->setStyleSheet("color: #888780; font-size: 13px;");

    h->addWidget(title);
    h->addStretch();
    h->addWidget(sub);
    return w;
}

QWidget *MainWindow::makeStatCard(const QString &label, QLabel *&valueLabel) {
    auto *card = new QFrame;
    card->setStyleSheet("QFrame { background:#FFFFFF; border:1px solid #E2E0D8;"
                        " border-radius:10px; }");
    auto *v = new QVBoxLayout(card);
    v->setContentsMargins(18, 14, 18, 14);
    v->setSpacing(4);

    auto *lbl = new QLabel(label);
    lbl->setStyleSheet("color:#888780; font-size:12px; font-weight:600;");

    valueLabel = new QLabel("0");
    QFont f = valueLabel->font();
    f.setPointSize(26); f.setWeight(QFont::DemiBold);
    valueLabel->setFont(f);
    valueLabel->setStyleSheet("color:#2C2C2A;");

    v->addWidget(lbl);
    v->addWidget(valueLabel);
    return card;
}

QWidget *MainWindow::makeStatsBar() {
    auto *w = new QWidget;
    auto *h = new QHBoxLayout(w);
    h->setContentsMargins(0,0,0,0);
    h->setSpacing(12);
    h->addWidget(makeStatCard("Total Users",    m_lblTotalUsers));
    h->addWidget(makeStatCard("Total Books",    m_lblTotalBooks));
    h->addWidget(makeStatCard("Borrowed",       m_lblBorrowed));
    h->addWidget(makeStatCard("Available",      m_lblAvailable));
    return w;
}

QWidget *MainWindow::makeButtonBar() {
    auto *w = new QWidget;
    auto *h = new QHBoxLayout(w);
    h->setContentsMargins(0,0,0,0);
    h->setSpacing(10);

    auto makeBtn = [](const QString &text, const QString &style) {
        auto *b = new QPushButton(text);
        b->setStyleSheet(style);
        b->setCursor(Qt::PointingHandCursor);
        b->setFixedHeight(38);
        return b;
    };

    QString primaryStyle = "QPushButton { background:#2C2C2A; color:#FFFFFF; border:none; }"
                           "QPushButton:hover { background:#444441; }";
    QString borrowStyle  = "QPushButton { background:#0F6E56; color:#FFFFFF; border:none; }"
                           "QPushButton:hover { background:#1D9E75; }";
    QString returnStyle  = "QPushButton { background:#FFFFFF; color:#A32D2D;"
                           " border:1px solid #F7C1C1; }"
                           "QPushButton:hover { background:#FCEBEB; }";

    auto *btnAddUser  = makeBtn("+ Add User",   primaryStyle);
    auto *btnAddBook  = makeBtn("+ Add Book",   primaryStyle);
    auto *btnBorrow   = makeBtn("Borrow Book",  borrowStyle);
    auto *btnReturn   = makeBtn("Return Book",  returnStyle);

    connect(btnAddUser, &QPushButton::clicked, this, &MainWindow::onAddUser);
    connect(btnAddBook, &QPushButton::clicked, this, &MainWindow::onAddBook);
    connect(btnBorrow,  &QPushButton::clicked, this, &MainWindow::onBorrowBook);
    connect(btnReturn,  &QPushButton::clicked, this, &MainWindow::onReturnBook);

    h->addWidget(btnAddUser);
    h->addWidget(btnAddBook);
    h->addWidget(btnBorrow);
    h->addWidget(btnReturn);
    h->addStretch();
    return w;
}

QTableWidget *MainWindow::makeTable(const QStringList &headers) {
    auto *t = new QTableWidget(0, headers.size());
    t->setHorizontalHeaderLabels(headers);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setAlternatingRowColors(false);
    t->verticalHeader()->setVisible(false);
    t->horizontalHeader()->setStretchLastSection(true);
    t->setShowGrid(false);
    t->setFocusPolicy(Qt::NoFocus);
    t->setFrameShape(QFrame::NoFrame);
    t->verticalHeader()->setDefaultSectionSize(42);
    return t;
}

QTabWidget *MainWindow::makeTabs() {
    auto *tabs = new QTabWidget;

    // ── Users tab
    m_tblUsers = makeTable({"Name", "Email", "Books Borrowed", "Status"});
    m_tblUsers->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tblUsers->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_tblUsers->setColumnWidth(2, 130);
    m_tblUsers->setColumnWidth(3, 100);
    tabs->addTab(m_tblUsers, "Users");

    // ── Books tab
    m_tblBooks = makeTable({"Title", "Author", "Total Stock", "Available", "Times Borrowed"});
    m_tblBooks->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tblBooks->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_tblBooks->setColumnWidth(2, 110);
    m_tblBooks->setColumnWidth(3, 100);
    m_tblBooks->setColumnWidth(4, 130);
    tabs->addTab(m_tblBooks, "Books");

    // ── Borrowed tab
    m_tblBorrowed = makeTable({"User", "Book", "Borrow Date", "Status"});
    m_tblBorrowed->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tblBorrowed->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_tblBorrowed->setColumnWidth(2, 120);
    m_tblBorrowed->setColumnWidth(3, 100);
    tabs->addTab(m_tblBorrowed, "Borrowed Books");

    // ── Most Borrowed tab
    auto *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: #FFFFFF; }");
    m_popularWidget = new QWidget;
    m_popularWidget->setStyleSheet("background: #FFFFFF;");
    m_popularLayout = new QVBoxLayout(m_popularWidget);
    m_popularLayout->setContentsMargins(20, 16, 20, 16);
    m_popularLayout->setSpacing(0);
    m_popularLayout->addStretch();
    scrollArea->setWidget(m_popularWidget);
    tabs->addTab(scrollArea, "Most Borrowed");

    return tabs;
}

// ─── JSON persistence ─────────────────────────────────────────────────────────

static QString dataFilePath() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/library_data.json";
}

void MainWindow::saveData() {
    QJsonArray usersArr;
    for (auto &u : m_users) {
        QJsonObject o;
        o["id"]    = u.id;
        o["name"]  = u.name;
        o["email"] = u.email;
        usersArr.append(o);
    }

    QJsonArray booksArr;
    for (auto &b : m_books) {
        QJsonObject o;
        o["id"]          = b.id;
        o["title"]       = b.title;
        o["author"]      = b.author;
        o["totalStock"]  = b.totalStock;
        o["available"]   = b.available;
        o["borrowCount"] = b.borrowCount;
        booksArr.append(o);
    }

    QJsonArray borrowsArr;
    for (auto &br : m_borrows) {
        QJsonObject o;
        o["id"]         = br.id;
        o["userId"]     = br.userId;
        o["bookId"]     = br.bookId;
        o["borrowDate"] = br.borrowDate.toString(Qt::ISODate);
        o["returned"]   = br.returned;
        borrowsArr.append(o);
    }

    QJsonObject root;
    root["users"]         = usersArr;
    root["books"]         = booksArr;
    root["borrows"]       = borrowsArr;
    root["nextUserId"]    = m_nextUserId;
    root["nextBookId"]    = m_nextBookId;
    root["nextBorrowId"]  = m_nextBorrowId;

    QFile file(dataFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        file.close();
    }
}

void MainWindow::loadData() {
    QFile file(dataFilePath());

    // If no file exists yet, load sample seed data for first run
    if (!file.exists()) {
        seedData();
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        seedData();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (doc.isNull() || !doc.isObject()) {
        seedData();
        return;
    }

    QJsonObject root = doc.object();

    m_nextUserId   = root["nextUserId"].toInt(1);
    m_nextBookId   = root["nextBookId"].toInt(1);
    m_nextBorrowId = root["nextBorrowId"].toInt(1);

    m_users.clear();
    for (auto v : root["users"].toArray()) {
        QJsonObject o = v.toObject();
        m_users.append({ o["id"].toInt(), o["name"].toString(), o["email"].toString() });
    }

    m_books.clear();
    for (auto v : root["books"].toArray()) {
        QJsonObject o = v.toObject();
        m_books.append({ o["id"].toInt(), o["title"].toString(), o["author"].toString(),
                         o["totalStock"].toInt(), o["available"].toInt(), o["borrowCount"].toInt() });
    }

    m_borrows.clear();
    for (auto v : root["borrows"].toArray()) {
        QJsonObject o = v.toObject();
        m_borrows.append({ o["id"].toInt(), o["userId"].toInt(), o["bookId"].toInt(),
                           QDate::fromString(o["borrowDate"].toString(), Qt::ISODate),
                           o["returned"].toBool() });
    }
}

// ─── Seed data (first run only) ───────────────────────────────────────────────

void MainWindow::seedData() {
    m_users.append({m_nextUserId++, "Ahmed Hassan",  "ahmed@email.com"});
    m_users.append({m_nextUserId++, "Sara Khalil",   "sara@email.com"});
    m_users.append({m_nextUserId++, "Mohamed Ali",   "mo@email.com"});

    m_books.append({m_nextBookId++, "Clean Code",                "Robert C. Martin",  3, 3, 0});
    m_books.append({m_nextBookId++, "The Pragmatic Programmer",  "Hunt & Thomas",     2, 2, 0});
    m_books.append({m_nextBookId++, "Design Patterns",           "Gang of Four",      4, 4, 0});
    m_books.append({m_nextBookId++, "Introduction to Algorithms","CLRS",              2, 2, 0});

    // Pre-borrow two books
    m_books[0].available--;  m_books[0].borrowCount++;
    m_books[1].available--;  m_books[1].borrowCount++;
    m_borrows.append({m_nextBorrowId++, 1, 1, QDate(2026,4,20), false});
    m_borrows.append({m_nextBorrowId++, 2, 2, QDate(2026,4,22), false});
}

// ─── Refresh ──────────────────────────────────────────────────────────────────

void MainWindow::refreshAll() {
    refreshStats();
    refreshUsers();
    refreshBooks();
    refreshBorrowed();
    refreshPopular();
}

void MainWindow::refreshStats() {
    int borrowed  = 0, available = 0;
    for (auto &b : m_borrows)  if (!b.returned) ++borrowed;
    for (auto &b : m_books)    available += b.available;

    m_lblTotalUsers->setText(QString::number(m_users.size()));
    m_lblTotalBooks->setText(QString::number(m_books.size()));
    m_lblBorrowed->setText(QString::number(borrowed));
    m_lblAvailable->setText(QString::number(available));
}

void MainWindow::refreshUsers() {
    m_tblUsers->setRowCount(0);
    for (auto &u : m_users) {
        int activeBorrows = 0;
        for (auto &b : m_borrows)
            if (b.userId == u.id && !b.returned) ++activeBorrows;

        int row = m_tblUsers->rowCount();
        m_tblUsers->insertRow(row);
        m_tblUsers->setItem(row, 0, new QTableWidgetItem(u.name));
        m_tblUsers->setItem(row, 1, new QTableWidgetItem(u.email));
        m_tblUsers->setItem(row, 2, new QTableWidgetItem(QString::number(activeBorrows)));

        auto *statusBadge = badge("Active", "#0F6E56", "#E1F5EE");
        m_tblUsers->setCellWidget(row, 3, statusBadge);
    }
}

void MainWindow::refreshBooks() {
    m_tblBooks->setRowCount(0);
    for (auto &b : m_books) {
        int row = m_tblBooks->rowCount();
        m_tblBooks->insertRow(row);
        m_tblBooks->setItem(row, 0, new QTableWidgetItem(b.title));
        m_tblBooks->setItem(row, 1, new QTableWidgetItem(b.author));
        m_tblBooks->setItem(row, 2, new QTableWidgetItem(QString::number(b.totalStock)));
        m_tblBooks->setItem(row, 4, new QTableWidgetItem(QString::number(b.borrowCount)));

        QString color, bg;
        if      (b.available == 0)          { color = "#A32D2D"; bg = "#FCEBEB"; }
        else if (b.available < b.totalStock) { color = "#633806"; bg = "#FAEEDA"; }
        else                                { color = "#0F6E56"; bg = "#E1F5EE"; }
        auto *avBadge = badge(QString::number(b.available) + " / " +
                              QString::number(b.totalStock), color, bg);
        m_tblBooks->setCellWidget(row, 3, avBadge);
    }
}

void MainWindow::refreshBorrowed() {
    m_tblBorrowed->setRowCount(0);
    for (auto &br : m_borrows) {
        QString userName = "Unknown", bookTitle = "Unknown";
        for (auto &u : m_users)  if (u.id == br.userId)  { userName  = u.name;    break; }
        for (auto &b : m_books)  if (b.id == br.bookId)  { bookTitle = b.title;   break; }

        int row = m_tblBorrowed->rowCount();
        m_tblBorrowed->insertRow(row);
        m_tblBorrowed->setItem(row, 0, new QTableWidgetItem(userName));
        m_tblBorrowed->setItem(row, 1, new QTableWidgetItem(bookTitle));
        m_tblBorrowed->setItem(row, 2, new QTableWidgetItem(br.borrowDate.toString("dd MMM yyyy")));

        auto *sb = br.returned
            ? badge("Returned", "#0F6E56", "#E1F5EE")
            : badge("Active",   "#185FA5", "#E6F1FB");
        m_tblBorrowed->setCellWidget(row, 3, sb);
    }
}

void MainWindow::refreshPopular() {
    // Clear existing items except the trailing stretch
    while (m_popularLayout->count() > 1)
        delete m_popularLayout->takeAt(0)->widget();

    QVector<Book> sorted = m_books;
    std::sort(sorted.begin(), sorted.end(),
              [](const Book &a, const Book &b){ return a.borrowCount > b.borrowCount; });

    int maxCount = sorted.isEmpty() ? 1 : std::max(sorted[0].borrowCount, 1);

    if (sorted.isEmpty() || sorted[0].borrowCount == 0) {
        auto *empty = new QLabel("No borrow history yet. Start lending books to see stats here.");
        empty->setStyleSheet("color:#888780; font-size:13px;");
        empty->setAlignment(Qt::AlignCenter);
        m_popularLayout->insertWidget(0, empty);
        return;
    }

    // Column header
    auto *header = new QWidget;
    auto *hh = new QHBoxLayout(header);
    hh->setContentsMargins(0, 0, 0, 8);
    auto makeHdr = [](const QString &t) {
        auto *l = new QLabel(t);
        l->setStyleSheet("color:#888780; font-size:11px; font-weight:600;");
        return l;
    };
    hh->addWidget(makeHdr("#"), 0);
    hh->addWidget(makeHdr("Book"), 2);
    hh->addWidget(makeHdr("Progress"), 3);
    hh->addWidget(makeHdr("Borrows"), 1);
    hh->addWidget(makeHdr("Unique Users"), 1);
    m_popularLayout->insertWidget(0, header);

    for (int i = 0; i < sorted.size(); ++i) {
        const Book &b = sorted[i];
        if (b.borrowCount == 0) continue;

        // Count unique users
        QSet<int> uniqueUsers;
        for (auto &br : m_borrows)
            if (br.bookId == b.id) uniqueUsers.insert(br.userId);

        auto *row = new QWidget;
        row->setStyleSheet("QWidget { border-bottom: 1px solid #F1EFE8; }");
        auto *rl = new QHBoxLayout(row);
        rl->setContentsMargins(0, 12, 0, 12);
        rl->setSpacing(14);

        // Rank
        auto *rank = new QLabel(QString("#%1").arg(i+1));
        rank->setStyleSheet("color:#888780; font-size:12px;");
        rank->setFixedWidth(24);
        rank->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        // Title
        auto *titleLbl = new QLabel(b.title);
        titleLbl->setStyleSheet("font-size:13px; font-weight:600; color:#2C2C2A;");
        titleLbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        // Progress bar
        auto *barContainer = new QWidget;
        barContainer->setFixedHeight(8);
        barContainer->setStyleSheet("QWidget { background:#F1EFE8; border-radius:4px; border:none; }");
        barContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        auto *bar = new QWidget(barContainer);
        int pct = maxCount > 0 ? (b.borrowCount * 100 / maxCount) : 0;
        bar->setStyleSheet("background:#1D9E75; border-radius:4px;");
        bar->setGeometry(0, 0, barContainer->width() * pct / 100, 8);
        // Store pct for resize; use fixed width estimate
        bar->setFixedHeight(8);
        bar->setFixedWidth(std::max(1, 160 * pct / 100));

        // Count labels
        auto *countLbl = new QLabel(QString::number(b.borrowCount));
        countLbl->setStyleSheet("font-size:13px; color:#2C2C2A; font-weight:600;");
        countLbl->setFixedWidth(60);
        countLbl->setAlignment(Qt::AlignCenter);

        auto *usersLbl = new QLabel(QString::number(uniqueUsers.size()));
        usersLbl->setStyleSheet("font-size:13px; color:#2C2C2A;");
        usersLbl->setFixedWidth(80);
        usersLbl->setAlignment(Qt::AlignCenter);

        rl->addWidget(rank);
        rl->addWidget(titleLbl, 2);
        rl->addWidget(barContainer, 3);
        rl->addWidget(countLbl, 1);
        rl->addWidget(usersLbl, 1);

        m_popularLayout->insertWidget(m_popularLayout->count() - 1, row);
    }
}

// ─── Slots ────────────────────────────────────────────────────────────────────

void MainWindow::onAddUser() {
    AddUserDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_users.append({m_nextUserId++, dlg.name(), dlg.email()});
        saveData();
        refreshAll();
    }
}

void MainWindow::onAddBook() {
    AddBookDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        int s = dlg.stock();
        m_books.append({m_nextBookId++, dlg.title(), dlg.author(), s, s, 0});
        saveData();
        refreshAll();
    }
}

void MainWindow::onBorrowBook() {
    if (m_users.isEmpty()) { return; }
    BorrowDialog dlg(m_users, m_books, this);
    if (dlg.exec() == QDialog::Accepted) {
        int uid = dlg.selectedUserId();
        int bid = dlg.selectedBookId();
        for (auto &b : m_books) {
            if (b.id == bid && b.available > 0) {
                b.available--;
                b.borrowCount++;
                m_borrows.append({m_nextBorrowId++, uid, bid, QDate::currentDate(), false});
                break;
            }
        }
        saveData();
        refreshAll();
    }
}

void MainWindow::onReturnBook() {
    QVector<BorrowRecord*> active;
    for (auto &br : m_borrows) if (!br.returned) active.append(&br);
    if (active.isEmpty()) return;

    ReturnDialog dlg(active, m_users, m_books, this);
    if (dlg.exec() == QDialog::Accepted) {
        int borrowId = dlg.selectedBorrowId();
        for (auto &br : m_borrows) {
            if (br.id == borrowId && !br.returned) {
                br.returned = true;
                for (auto &b : m_books)
                    if (b.id == br.bookId) { b.available++; break; }
                break;
            }
        }
        saveData();
        refreshAll();
    }
}
