//pn532 寻卡
//发送的命令(十六进制）：
//0x00,0x00,0xff,0x04,0xfc,0xd4,0x4a,0x02,0x00,0xe0,0x00
//成功的话PN532就会返回
//25位十六进制卡片信息
//缺陷 数据为FF输出异常为 FFFFFFFF 另外部分影响其他数据

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<errno.h>
#include<string.h>

int main()
{
    int fd;
    int i;
    int len;
    int total_len;
    int n = 0;
    char read_buf[256];
    char write_buf[256];
    char wake[24]={0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x03, 0xfd, 0xd4, 0x14, 0x01, 0x17, 0x00};
    char uid[11]={0x00,0x00,0xff,0x04,0xfc,0xd4,0x4a,0x02,0x00,0xe0,0x00};
    struct termios opt;

    fd = open("/dev/ttyUSB0", O_RDWR|O_NOCTTY|O_NDELAY);
    if(fd == -1)
    {
        perror("open serial 0\n");
        exit(0);
    }

    tcgetattr(fd, &opt);
    bzero(&opt, sizeof(opt));

    tcflush(fd, TCIOFLUSH);

    cfsetispeed(&opt, B115200);
    cfsetospeed(&opt, B115200);

    opt.c_cflag &= ~CSIZE;
    opt.c_cflag |= CS8;
    opt.c_cflag &= ~CSTOPB;
    opt.c_cflag &= ~PARENB;
    opt.c_cflag &= ~CRTSCTS;
    opt.c_cflag |= (CLOCAL | CREAD);

    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    opt.c_oflag &= ~OPOST;

    opt.c_cc[VTIME] = 0;
    opt.c_cc[VMIN] = 0;

    tcflush(fd, TCIOFLUSH);

    printf("configure complete\n");

    if(tcsetattr(fd, TCSANOW, &opt) != 0)
    {
        perror("serial error");
        return -1;
    }

    printf("start send and receive data\n");
    //唤醒
    n = write(fd, wake, 24);
    total_len=0;
    while(1)
    {
        n = 0;
        len = 0;
        bzero(read_buf, sizeof(read_buf));
        bzero(write_buf, sizeof(write_buf));

        while( (n = read(fd, read_buf, sizeof(read_buf))) > 0 )
        {
            for(i = len; i < (len + n); i++)
            {
                write_buf[i] = read_buf[i];
            }
            len += n;
        }
        write_buf[len] = '\0';

//      printf("wake len: %d \n", len);
        for(i=0;i<len;i++)
        {
            printf("%02X ",read_buf[i]);
        }
        total_len+=len;
        if(total_len==15)
        {
            printf("\n");
            break;
        }

    }

    while(1)
    {
        //寻卡
        total_len = 0;
        n = write(fd, uid, 11);
        while(1)
        {
            n = 0;
            len = 0;
            bzero(read_buf, sizeof(read_buf));

            while( (n = read(fd, read_buf, sizeof(read_buf))) > 0 )
            {
                for(i = len; i < (len + n); i++)
                {
                    write_buf[i] = read_buf[i];
                }
                len += n;
            }
            for(i=0;i<len;i++)
            {
                printf("%02X ",write_buf[i]);
            }
            total_len += len;
            if(total_len == 25)
            {
                printf("\n");
                break;
            }
        }
        sleep(2);
    }
    return 0;
}

