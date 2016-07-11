//唤醒pn532
//发送的命令(十六进制）：
//55 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 03 FD D4 14 01 17 00
//成功的话PN532就会返回
//00 00 FF 00 FF 00 00 00 FF 02 FE D5 15 16 00
//添加了send_recv函数 模块化


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<errno.h>
#include<string.h>
int send_recv(int fd,char write_buf[],int send_len,char read_buf[],int recv_len);


int main()
{
    int fd;
    int i;
    int j;
    int len;
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
    send_recv(fd,wake,24,read_buf,15);
    while(1)
    {
        //读取卡信息
        send_recv(fd,uid,11,read_buf,25);
        sleep(2);
    }
    return 0;
}


int send_recv(int fd,char write_buf[],int send_len,char read_buf[],int recv_len)
{
    int i;//temp_buf下标
    int j;//read_buf下标
    int n = 0;
    int len = 0;//接受到数据的长度
    char temp_buf[256];//保存数据  read_buf接收数据
    bzero(temp_buf, sizeof(temp_buf));
    n = write(fd, write_buf, send_len);
    while(1)
    {
        n = 0;
        bzero(read_buf, sizeof(read_buf));
        while( (n = read(fd, read_buf, sizeof(read_buf))) > 0 )
        {
            for(i = len,j=0; i < (len + n); i++,j++)
            {
                temp_buf[i] = read_buf[j];
            }
            len += n;
        }
        if(len == recv_len)
        {
            for(i=0;i<len;i++)
            {
                printf("%02X ",temp_buf[i]);
            }
            printf("\n");
            break;
        }
    }
}




