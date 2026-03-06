#include "../disk/disk.cpp"
#include "storage.cpp"
#include "../stream/stream.cpp"
#include "bitmap.cpp"
#include "../tree/tree.cpp"
#include "../utils/inited.cpp"
#include "../crypto/uuid.cpp"
#include "xcfs.cpp"

enum OpenMode {
    read = 1 << 0,
    write = 1 << 1,
    append = 1 << 2
};

class File {
    inode node;

public:
    uuid_t session;
    uint64_t seek;
    string fname;
    OpenMode mode;

    File(const string &filename, OpenMode mode) {
        fname = filename;
        seek = 0;
        this->mode = mode;
        session = uuid4();

        switch (mode) {
            case write: {
                if (filesystem::exists(filename)) {


                }
                else {


                }
                break;
            }
            case read: {

                break;
            }
            case append: {

                seek = node.getsize();
                break;
            }
            default: ;
        }
    }


};