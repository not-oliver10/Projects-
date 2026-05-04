#pragma once
#include <QString>
#include <QVector>
#include <QDate>

struct Book {
    int id;
    QString title;
    QString author;
    int totalStock;
    int available;
    int borrowCount;
};

struct User {
    int id;
    QString name;
    QString email;
};

struct BorrowRecord {
    int id;
    int userId;
    int bookId;
    QDate borrowDate;
    bool returned;
};
