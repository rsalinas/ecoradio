#include <QCoreApplication>
#include <QtSql/QtSql>


int main(int argc, char *argv[])
{
    qDebug() << "HOLA";
    QCoreApplication a(argc, argv);
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("db.sqlite");
    db.open();

    QSqlQuery query(db);
    if (query.exec("select 5+5")) {
        while (query.next())
        qDebug() << query.value(0).toString();
    } else {
        qDebug() << "error" <<  query.lastError();
    }
    db.close();

    return a.exec();
}

