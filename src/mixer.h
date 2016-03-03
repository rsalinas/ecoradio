#pragma once

#include <memory>

#include <QObject>
#include "soundsource.h"
#include "pcm_player.h"
#include "ogg_encoder.h"

class Mixer : public QObject
{
    Q_OBJECT
public:
    explicit Mixer(QObject *parent = 0);
    virtual ~Mixer();

signals:

public slots:
    void prepare(std::shared_ptr<SoundSource> source) {}
    void run();
    void add(std::shared_ptr<SoundSource> source) {

    }

    void remove(std::shared_ptr<SoundSource> source) {

    }

private:
    std::shared_ptr<SoundSource> m_current;
    std::shared_ptr<SoundSource> m_next;

    std::vector<SoundSource> m_active;
    OggEncoder oggEncoder;
    PcmPlayer pcmPlayer;
};

