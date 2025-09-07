// ledAPP.c - 使用者空間操作 /dev/<name>
// 用法：
//   sudo ./ledAPP /dev/myled on
//   sudo ./ledAPP /dev/myled off
//   sudo ./ledAPP /dev/myled toggle
//   sudo ./ledAPP /dev/myled read
//   sudo ./ledAPP /dev/myled blink 200 800
//   sudo ./ledAPP /dev/myled blink_stop
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage:\n"
        "  sudo %s <devnode> on|off|toggle|read\n"
        "  sudo %s <devnode> blink <on_ms> <off_ms>\n"
        "  sudo %s <devnode> blink_stop\n"
        "Example:\n"
        "  sudo %s /dev/myled on\n", prog, prog, prog, prog);
}

int main(int argc, char *argv[])
{
    if (argc < 3) { usage(argv[0]); return 1; }

    const char *dev = argv[1];
    const char *cmd = argv[2];
    int fd = open(dev, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 2;
    }

    int rc = 0;
    if (!strcmp(cmd, "read")) {
        char buf[16] = {0};
        rc = read(fd, buf, sizeof(buf)-1);
        if (rc < 0) { perror("read"); close(fd); return 3; }
        printf("%s", buf);  // 會印出 "0\n" 或 "1\n"
    } else if (!strcmp(cmd, "on") || !strcmp(cmd, "1")) {
        rc = write(fd, "1", 1);
    } else if (!strcmp(cmd, "off") || !strcmp(cmd, "0")) {
        rc = write(fd, "0", 1);
    } else if (!strcmp(cmd, "toggle") || !strcmp(cmd, "t")) {
        rc = write(fd, "t", 1);
    } else if (!strcmp(cmd, "blink")) {
        if (argc != 5) { usage(argv[0]); close(fd); return 4; }
        char buf[64];
        snprintf(buf, sizeof(buf), "blink %lu %lu",
                 strtoul(argv[3], NULL, 0), strtoul(argv[4], NULL, 0));
        rc = write(fd, buf, strlen(buf));
    } else if (!strcmp(cmd, "blink_stop")) {
        rc = write(fd, "blink_stop", 10);
    } else {
        usage(argv[0]);
        close(fd);
        return 5;
    }

    if (rc < 0) { perror("write"); close(fd); return 6; }
    close(fd);
    return 0;
}
