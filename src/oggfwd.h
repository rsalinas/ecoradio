
#include <QProcess>

class OggFwd
{
public:
    OggFwd();
    virtual int writeBytes(const char * buf, int length) = 0;


};


class ProcessOggFwd : public OggFwd
{
public:
    ProcessOggFwd();
    virtual int writeBytes(const char * buf, int length) = 0;

private:
    QProcess m_process;
};
