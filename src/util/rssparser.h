#pragma once

#include <exception>

#include <QStringList>
#include <QtXml/QtXml>


class RssParser : private QObject, private QXmlDefaultHandler
{
public:

    RssParser(QIODevice *dev, QObject * parent=nullptr);
    RssParser(const QString &filename, QObject * parent=nullptr);
    QStringList getStreams();

    class RssException : public std::exception {
    public:
        RssException(const QString &cause) : m_cause(cause.toStdString()) {
        }

        const char * what() const noexcept(true) {
            return m_cause.c_str();
        }

    private:
        std::string m_cause;
    };
    ~RssParser();



private:
    bool startElement(const QString & namespaceURI, const QString & localName,
                      const QString & qName, const QXmlAttributes & atts ) override;
    bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName) override;

    QIODevice * m_file;

    QString printIndent() {
        return QString::fromStdString(std::string(std::max(stack.size()-1, 0)*4, ' '));
    }
    QStack<QString> stack;
    QStringList m_streams;
};
