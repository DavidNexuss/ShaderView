#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

namespace Nutils {

    inline  size_t getFileSize(const char* filename) { 
        struct stat st;
        stat(filename, &st);
        return st.st_size;
    }
    
    inline std::vector<uint8_t> readFromFile(int fd) {
        std::vector<uint8_t> buffer;
        int buff_size = 4096 * 16;
        int actual_new_size;
        int head = 0;
    
        buffer.resize(head + buff_size);
    
        while((actual_new_size = read(fd,&buffer[head],buff_size)) > 0) { 
            head += actual_new_size;
            buffer.resize(head + buff_size);
        }
    
        buffer.resize(head);
        return buffer;
    } 
    
    inline std::vector<uint8_t> readFromFile(const char* filename) { 
        std::vector<uint8_t> buffer(getFileSize(filename));
        int n;
        int idx = 0;
        int fd = open(filename,O_RDONLY);
        while(idx < buffer.size() && (n = read(fd,&buffer[idx],buffer.size() - idx))) { idx += n; }
        close(fd);
        return buffer;
    }
}
