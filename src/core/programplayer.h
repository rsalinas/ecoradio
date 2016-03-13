#pragma once

#include <memory>

#include "common/program.h"
#include "file_scan.h"

class ProgramPlayer  {
public:
    virtual std::shared_ptr<SoundSource> getNextSong() = 0;
    virtual ~ProgramPlayer() {
    }
};

class FolderProgramPlayer : public ProgramPlayer {
public:
    FolderProgramPlayer(const FolderProgram &p);
    std::shared_ptr<SoundSource> getNextSong() override;

private:
    FolderProgram m_program;
    Traverse traverse;
};

class StreamProgramPlayer : public ProgramPlayer {
public:
    StreamProgramPlayer(const StreamProgram &p);
    std::shared_ptr<SoundSource> getNextSong() override;

private:
    StreamProgram m_program;
};

class LiveProgramPlayer : public ProgramPlayer {
public:
    LiveProgramPlayer(const LiveProgram &p);
    std::shared_ptr<SoundSource> getNextSong() override;

private:
    LiveProgram m_program;
};

class PodcastProgramPlayer : public ProgramPlayer {
public:
    PodcastProgramPlayer(const PodcastProgram &p);
    std::shared_ptr<SoundSource> getNextSong() override;


private:
    PodcastProgram m_program;
};

