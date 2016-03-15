#include "programplayer.h"

#include "snd/sources/mpg123wrap.h"

std::shared_ptr<SoundSource> LiveProgramPlayer::getNextSong()
{
    return nullptr;
}
std::shared_ptr<SoundSource> StreamProgramPlayer::getNextSong()
{
    return nullptr;
}

FolderProgramPlayer::FolderProgramPlayer(const FolderProgram &p) :
    m_program(p),
    traverse("/home/rsalinas/Sync/mp3", "/tmp/vistos", "/tmp/catalog") {
}

std::shared_ptr<SoundSource> FolderProgramPlayer::getNextSong()
{

    auto ret = std::make_shared<Mpg123>(traverse.basedir()+"/"+traverse.getRandom());
    //    ret->stopFadeOut(1000);
    return ret;
}

std::shared_ptr<SoundSource> PodcastProgramPlayer::getNextSong()
{
    return nullptr;
}


PodcastProgramPlayer::PodcastProgramPlayer(const PodcastProgram &p) :
    m_program(p) {
}



LiveProgramPlayer::LiveProgramPlayer(const LiveProgram &p) :
    m_program(p)
{
}


StreamProgramPlayer::StreamProgramPlayer(const StreamProgram &p) :
    m_program(p)
{
}


std::shared_ptr<ProgramPlayer> getProgramPlayer(const Program &p) {
    if (dynamic_cast<const FolderProgram*>(&p)) {
        return std::make_shared<FolderProgramPlayer>(*dynamic_cast<const FolderProgram*>(&p));
    } else if (dynamic_cast<const PodcastProgram*>(&p)) {
        return std::make_shared<PodcastProgramPlayer>(*dynamic_cast<const PodcastProgram*>(&p));
    } else if (dynamic_cast<const LiveProgram*>(&p)) {
        return std::make_shared<LiveProgramPlayer>(*dynamic_cast<const LiveProgram*>(&p));
    } else if (dynamic_cast<const StreamProgram*>(&p)) {
        return std::make_shared<StreamProgramPlayer>(*dynamic_cast<const StreamProgram*>(&p));
    } else
        return nullptr;
}


