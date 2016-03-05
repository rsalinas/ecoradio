#include <ao/ao.h>
#include <cstdio>
#include <exception>
#include <cstring>
#include <memory>

class Decoder;

class ao_device;

class PcmPlayer
{
public:
    PcmPlayer();
    ~PcmPlayer();

    int addStream(std::shared_ptr<Decoder> s) {

    }

    size_t write(char * buf, int len);

    void copyFirst(char * buf) {
        memcpy(buffer, buf, buf_size);
    }

    int add(char * buffer);
    size_t writeMix() {
        return write(buffer, buf_size);
    }

    ao_sample_format format;
    size_t buf_size;
private:
    char * buffer = nullptr;
    ao_device *device = nullptr;
};


class PcmPlayerException : public std::exception
{

};
