#pragma once

#include <exception>

#include <QStringList>
#include <QtXml/QtXml>


class RssParser : private QXmlDefaultHandler
{
public:
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

    RssParser(const QString &filename);
    QStringList getStreams();

private:
    bool startElement(const QString & namespaceURI, const QString & localName,
                      const QString & qName, const QXmlAttributes & atts ) override;
    bool endElement(const QString & namespaceURI, const QString & localName, const QString & qName) override;

    QString printIndent() {
        return QString::fromStdString(std::string(std::max(stack.size()-1, 0)*4, ' '));
    }
    QStack<QString> stack;
    QStringList m_streams;
};
