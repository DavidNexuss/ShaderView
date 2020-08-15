#include <thread>
class InotifyHandler
{
    const char *file_path;

    bool look_for_events;

    std::thread inotify_thread;

    void query_events();

    public:
    bool reload_shader;
    InotifyHandler(const char* file_path);
};
