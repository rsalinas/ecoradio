#pragma once

#include <QDir>
#include <QTextStream>
#include <memory>
#include <QDebug>
#include <QBitArray>

class Traverse {
public:

    Traverse(const QString &d, const QString &vistos, const QString &catalog);


    void saveVistos() {
        if (!m_seenIndex.open(QIODevice::WriteOnly)) {
            qFatal("cannot open");
        }
        QTextStream s(&m_seenIndex);
        int n=0;
        for (int i=0; i < seen.size(); i++) {
            if (seen.testBit(i)) {
                s << i << endl;
                n++;
            }
        }
        m_seenIndex.close();

    }

    ~Traverse() {
        saveVistos();
    }

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

