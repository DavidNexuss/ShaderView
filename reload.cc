#include <sys/inotify.h>
#include <limits.h>
#include <iostream>
#include <unistd.h>
#include "reload.hh"

using namespace std;

bool reload_shader = false;
bool look_for_events = true;

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

void InotifyHandler::query_events()
{
    int inotifyFd,wd;
    inotifyFd = inotify_init();
    if (inotifyFd < 0)
    {
        cerr << "Error initial " << endl;
        return;
    }
    
    cout << inotifyFd << " " << file_path << " " << IN_MODIFY << endl;
    wd = inotify_add_watch(inotifyFd,file_path,IN_MODIFY);
    
    if (wd < 0)
    {
        cerr << "Error 2" << endl;
        return;
    }
    
    inotify_event* event;
    static char buf[BUF_LEN]; __attribute__((aligned(8)));
    char* p;
    ssize_t numRead;
    
    while(look_for_events)
    {
        cout << "Start" << endl;
        numRead = read(inotifyFd,buf,BUF_LEN);
        for(p = buf; p < buf + numRead; )
        {
            event = (struct inotify_event*) p;
            if (event->mask & IN_MODIFY)
            {
                cout << "IN_MODIFY";
                reload_shader = true;
            }
            p+= sizeof(struct inotify_event) + event->len;
        }
    }

}
InotifyHandler::InotifyHandler(const char* p_file_path) : 
    file_path(p_file_path), inotify_thread(&InotifyHandler::query_events,*this) { }
