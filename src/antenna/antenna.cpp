#include <ogg/ogg.h>

#include <iostream>
#include <fstream>
#include <cassert>

int main(int argc, char *argv[])
{
    //http://radiomalva.ddns.net:8000/rm.ogg
    std::ifstream file("test.ogg", std::ios::in | std::ios::binary);
    ogg_sync_state state;
    int ret = ogg_sync_init(&state);
    assert(ret==0);


    ogg_page page;
    while(ogg_sync_pageout(&state, &page) != 1) {
        char* buffer = ogg_sync_buffer(&state, 4096);
        assert(buffer);

        file.read(buffer, 4096);
        int bytes = file.gcount();
        if (bytes == 0) {
            // End of file
            break;
        }

        int ret = ogg_sync_wrote(&state, bytes);
        assert(ret == 0);
    }


    return 0;
}
