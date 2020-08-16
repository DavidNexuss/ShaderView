#include <thread>
#include "main.hh"

class InotifyHandler
{
    const char *file_path;
    int inotifyFd,wd;   //Requested by linux inotify api

    bool look_for_events;
    std::thread inotify_thread;

    void query_events();

    public:
    InotifyHandler(const char* file_path);
    ~InotifyHandler();
};
