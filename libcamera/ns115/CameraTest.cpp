#define LOG_TAG "CameraTest"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cutils/log.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "CameraTest.h"

#define FILE_PATH_LEN 50
// You Must create dir '/tmp' and change mode to 777
#define CAMERA_DIR_PATH_NAME "/data/camera_test/"

namespace android {

/*
 * write data to file ,test camera sensor frame is correct
 */
int write2File(const char *path, void *data, int data_len){
    int fd;
    if(path == NULL){
        LOGE("Input path is NULL.");
        return -1;
    }
    if((fd = open(path,O_CREAT|O_RDWR, S_IRWXU)) < 0){
        LOGE("Open file:%s failed!ERROR FOR:%s(%d)", path, strerror(errno), errno);
        return -1;    
    }
    if(write(fd, data, data_len) < 0){
        LOGE("Write file:%s failed!ERROR FOR:%s(%d)", path, strerror(errno), errno);
        return -1;    
    }
    close(fd);
    return 0;
}


int writeFrame(int frameno, void *data, int data_len){
    char f_path[FILE_PATH_LEN];
    int ret;
    sprintf(f_path, "%sframe_%d.yuv", CAMERA_DIR_PATH_NAME, frameno);
    return write2File(f_path, data, data_len);
}

}; /* namespace android */
