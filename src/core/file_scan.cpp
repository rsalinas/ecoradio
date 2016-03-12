#include "file_scan.h"

#include <cassert>

class Rescan {
public:
    Rescan(const QDir &d, const QString &catalog) : m_d(d), all(catalog) {
        if (!all.open(QIODevice::WriteOnly| QIODevice::Truncate)) {
            qFatal("cannot open index file to write");
        }
        s.reset(new QTextStream(&all));
    }

    void process(const QDir &d, int depth) {
        if (depth > maxdepth)
            maxdepth = depth;

        for (auto e : d.entryInfoList(QStringList() << "*.mp3",
                                      QDir::NoDotAndDotDot | QDir::Files)) {
            assert( e.isFile());
            auto f = e.absoluteFilePath();
            f.remove(0, m_d.absolutePath().length());
            f="."+f;
            *s << f << endl;
            files.append(f);
            n++;
        }

        for (auto e : d.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs| QDir::Hidden )) {
            assert( e.isDir());
            process(e.absoluteFilePath(), depth + 1);
        }
    }
    QStringList run() {
        process(m_d, 0);
        qDebug() << n << "Max depth: " << maxdepth;
        return files;
    }
private:
    QDir m_d;
    QFile all;
    std::unique_ptr<QTextStream> s;
    int maxdepth = 0, n=0;
    QStringList files;
};

Traverse::Traverse(const QString &d, const QString &vistos, const QString &catalog) :
    m_dir(d), m_seenIndex(vistos), m_catalog(catalog), seen(0, false) {
    QFile f(m_catalog);
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream s(&f);
        while (!s.atEnd()) {
            files.append(s.readLine());
        }
        seen.fill(false, files.length());
        {
            if (!m_seenIndex.open(QIODevice::ReadOnly)) {
                qDebug() << "cannot open vistos";

            } else {
                QTextStream s(&m_seenIndex);
                int n=0;
                while (!s.atEnd()) {
                    auto l = s.readLine();
                    if (l.isEmpty()) {
                        qDebug() << l.length();
                        continue;
                    }
                    auto pos = l.toInt();
                    seen.setBit(pos);
                    n++;
                }
                m_seenIndex.close();
            }
        }
    } else {
        qDebug() << "cannot open";
        files = (Rescan(m_dir, m_catalog)).run();
        seen.fill(false, files.length());
    }

    f.close();
}


size_t Traverse::rescan() {
    files = (Rescan(m_dir, m_catalog)).run();
    seen.fill(false, files.length());
    saveVistos();
    return files.length();
}
QString Traverse::getRandom()
{
    if (seen.count(0) ==0 ) {
        seen.fill(false);
    }
    auto n = files.length();
    auto pos = qrand() % n;
    while (seen.testBit(pos)) {
        pos = (pos + 1) % n;
    }
    seen.setBit(pos);
    return files[pos];
}


void Traverse::saveVistos() {
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

Traverse::~Traverse() {
    saveVistos();
}
