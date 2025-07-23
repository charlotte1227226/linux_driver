#include <sys/types.h> 
// ssize_t 是 POSIX（Unix/Linux API）裡常見的一種「有號整數」型別，用來表示「物件大小」或「操作回傳值」
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
/*
* argc: 應用程序參數個數
* argv[]: 具體的參數內容，字串陣列形式
* ./chrdevbaseAPP <filename>
* argv[0] = ./chrdevbaseAPP, argv[1] = filename
*/
int main(int argc, char *argv[])
{
    int ret = 0;
    int file_descriptor = 0; // 用open打開一個文件的時候要有一個標記，那個標記就是file_descriptor
    char *filename;
    char readbuf[100], writebuf[100];

    filename = argv[1];

    file_descriptor = open(filename, O_RDWR); // O_RDONLY:只能讀，O_WRONLY:只能寫，O_RDWR:讀寫皆可
    //open()返回值， -1 : 錯誤
    if(file_descriptor < 0){
        printf("Can't open file %s \r\n", filename);
        return -1;
    }
    printf("Open file %s success\r\n", filename);
    printf("file_descriptor = %d ", file_descriptor);
    /* read */
    ret = read(file_descriptor, readbuf, 50); // 50是從驅動裡面讀50個字節
    // -1 : 錯誤，讀取錯誤
    // 沒有錯誤的話，有可能是要看一下，返回值跟50不同
    // file_descriptor是你要寫的文件，readbuf是讀取到的字節存在這裡，50是讀取50個字節
    if(ret < 0){
        printf("read file %s failed!\r\n", filename);
        return -1;
    }
    else{
        printf("read file %s success\r\n", filename);
        printf("ret = %d \r\n", ret);
    }
    
    /* write */
    ret = write(file_descriptor, writebuf, 50);
    // -1 : 錯誤，寫入錯誤
    // 沒有錯誤的話，有可能是要看一下，只能寫<50個字節(看返回值)
    if(ret < 0){
        printf("write file %s failed!\r\n", filename);
        return -1;
    }
    else{
        printf("write file %s success\r\n", filename);
        printf("ret = %d \r\n", ret);
    }

    /* close */
    ret = close(file_descriptor);
    // -1 : 錯誤，關閉錯誤
    if(ret < 0){
        printf("close file %s failed!", filename);
        return -1;
    }
    else{
        printf("close file %s success\r\n", filename);
        printf("ret = %d \r\n", ret);
    }

    return 0;
}