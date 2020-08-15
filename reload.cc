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
     
    inotify_event* event;
    static char buf[BUF_LEN];// __attribute__((aligned(8)));
    char* p;
    ssize_t numRead;
    
    wd = inotify_add_watch(inotifyFd,file_path,IN_ALL_EVENTS);
    while(look_for_events)
    {
        numRead = read(inotifyFd,buf,BUF_LEN);
        for(p = buf; p < buf + numRead; )
        {
            event = (struct inotify_event*) p;
            if (event->mask & (IN_MODIFY | IN_DELETE_SELF | IN_CLOSE_WRITE))
            {
                reload_shader = true;
            }
            /*
             printf("mask = ");
            if (event->mask & IN_ACCESS)        printf("IN_ACCESS ");
            if (event->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
            if (event->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
            if (event->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
            if (event->mask & IN_CREATE)        printf("IN_CREATE ");
            if (event->mask & IN_DELETE)        printf("IN_DELETE ");
            if (event->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
            if (event->mask & IN_IGNORED)       printf("IN_IGNORED ");
            if (event->mask & IN_ISDIR)         printf("IN_ISDIR ");
            if (event->mask & IN_MODIFY)        printf("IN_MODIFY ");
            if (event->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
            if (event->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
            if (event->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
            if (event->mask & IN_OPEN)          printf("IN_OPEN ");
            if (event->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
            if (event->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
            cout << event->mask; */
            p+= sizeof(struct inotify_event) + event->len;
        }
    }

}
InotifyHandler::InotifyHandler(const char* p_file_path) : 
    file_path(p_file_path), inotify_thread(&InotifyHandler::query_events,this) { }
InotifyHandler::~InotifyHandler()
{
    inotify_thread.detach();
}
