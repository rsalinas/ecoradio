#pragma once

#include <QDir>
#include <QTextStream>
#include <memory>
#include <QDebug>
#include <QBitArray>

class Traverse {
public:
    Traverse(const QString &d, const QString &vistos, const QString &catalog);
    void saveVistos();
    ~Traverse();
    size_t rescan();
    QString getRandom();
    size_t size() {
        return files.size();
    }

private:
    QDir m_dir;
    QFile m_seenIndex;
    QString m_catalog;
    QStringList files;
    QBitArray seen;
};

