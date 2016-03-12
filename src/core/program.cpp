#include "program.h"

#include <memory>
#include "snd/sources/mpg123wrap.h"

std::shared_ptr<SoundSource> LiveProgram::getNextSong() {
return nullptr;
}
std::shared_ptr<SoundSource> StreamProgram::getNextSong() {
    return nullptr;
}
std::shared_ptr<SoundSource> FolderProgram::getNextSong() {

    auto ret = std::make_shared<Mpg123>(traverse.basedir()+"/"+traverse.getRandom());
//    ret->stopFadeOut(1000);
    return ret;
}

std::shared_ptr<SoundSource> PodcastProgram::getNextSong() {
    return nullptr;
}


LiveProgram::LiveProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
    Program(id, dow, ts, name)
{

}


FolderProgram::FolderProgram(rowid_t id, int dow, const QDateTime &ts, const QString &name) :
    Program(id, dow, ts, name),
    traverse("/home/rsalinas/Sync/mp3", "/tmp/vistos", "/tmp/catalog")
{

}
