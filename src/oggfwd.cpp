#include "oggfwd.h"

#include <QDebug>

#include <shout/shout.h>

OggFwd::OggFwd(const Config &cfg) {
    shout_init();
    if ((shout = shout_new()) == NULL) {
        throw OggFwdException("Could not allocate shout_t");
    }

    if (shout_set_host(shout, cfg.hostName.toStdString().c_str()) != SHOUTERR_SUCCESS) {
        throw OggFwdException("Error setting hostname: "+cfg.hostName+ " " +shout_get_error(shout));
    }

    if (shout_set_port(shout, cfg.port) != SHOUTERR_SUCCESS) {
        throw OggFwdException("Error setting port: "+QString::number(cfg.port)+ " " +shout_get_error(shout));
    }

    if (shout_set_password(shout, cfg.passwd.toStdString().c_str()) != SHOUTERR_SUCCESS) {
        throw OggFwdException(QString("Error setting password: ")+shout_get_error(shout));
    }

    if (shout_set_mount(shout, cfg.mount.toStdString().c_str()) != SHOUTERR_SUCCESS) {
        throw OggFwdException(QString("Error setting mount:") + shout_get_error(shout));
    }

    shout_set_format(shout, SHOUT_FORMAT_VORBIS);
    unsigned int pFlag = 1;
    shout_set_public(shout, pFlag);


    if (cfg.description.size())
        shout_set_description(shout, cfg.description.toStdString().c_str());

    if (cfg.genre.size())
        shout_set_genre(shout, cfg.genre.toStdString().c_str());

    if (cfg.name.size())
        shout_set_name(shout, cfg.name.toStdString().c_str());

    if (cfg.url.size())
        shout_set_url(shout, cfg.url.toStdString().c_str());

    if (cfg.user.size() && !shout_set_user(shout, cfg.user.toStdString().c_str()) != SHOUTERR_SUCCESS) {
        throw OggFwdException("Error setting user: "+cfg.user+ " " +shout_get_error(shout));
    }

    int err = shout_open(shout);
    if (err == SHOUTERR_SUCCESS) {
        qDebug() << "Connected to server";
    } else {
        throw OggFwdException(QString("Cannot connect ")+shout_get_error(shout) + " " +QString::number(err));
    }

    int major, minor, patch;
    const char * s = shout_version(&major, &minor, &patch);
    qDebug() << "Connected to icecast " << s;
    open(WriteOnly);
}


ProcessOggFwd::ProcessOggFwd() {
}


qint64 OggFwd::readData(char *data, qint64 maxlen)  {
    return -1;
}


qint64 OggFwd::writeData(const char *data, qint64 len) {
    m_total += len;
    int ret = shout_send(shout, reinterpret_cast<const unsigned char*>(data), len);
    if (ret != SHOUTERR_SUCCESS) {
        qDebug() << "Send error" << shout_get_error(shout);
        //        quit = 1; FIXME!
    }

    auto sync = false;
    if (sync)
        shout_sync(shout);
    return len;
}

OggFwd::~OggFwd() {
    shout_close(shout);
    shout_free(shout);
}

OggFwd::OggFwdException::OggFwdException(const QString &msg) : m_msg(msg.toStdString()){
}


void OggFwd::setMetadata(const Metadata &metadata) {

    /* Sets MP3 metadata.
 * Returns:
 *   SHOUTERR_SUCCESS
 *   SHOUTERR_UNSUPPORTED if format isn't MP3
 *   SHOUTERR_MALLOC
 *   SHOUTERR_INSANE
 *   SHOUTERR_NOCONNECT
 *   SHOUTERR_SOCKET
 */
    int ret = shout_set_metadata(shout, metadata.self);
    if (ret != SHOUTERR_SUCCESS)
        qDebug() << "error setting metadata";

}

OggFwd::Metadata::Metadata() : self(shout_metadata_new()) {
}

OggFwd::Metadata::~Metadata() {
    shout_metadata_free(self);
}



void OggFwd::Metadata::add(const QString &key, const QString &value) {
    if (shout_metadata_add(self, key.toStdString().c_str(),
                       value.toStdString().c_str()) != SHOUTERR_SUCCESS) {
        qWarning() << "error in shout_metadata_add";
    }
}
