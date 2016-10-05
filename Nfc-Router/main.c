//唤醒pn532
//发送的命令(十六进制）：
//55 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 03 FD D4 14 01 17 00
//成功的话PN532就会返回
//00 00 FF 00 FF 00 00 00 FF 02 FE D5 15 16 00
//串口接受到的数据放在read_buf中

/*
 *  读取m1 UID卡片正常
 * p2p 指令流程尚不清楚 思路整理中
 *
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<errno.h>
#include<string.h>

int send_recv(int fd,const u_int8_t write_buf[],int send_len,u_int8_t read_buf[],int recv_len);
int main()
{
    int fd;
    int i;
    int j;
    int len;
    int n = 0;
    u_int8_t read_buf[256];
    u_int8_t write_buf[256];
    const u_int8_t  wake[24]={0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x03, 0xfd, 0xd4, 0x14, 0x01, 0x17, 0x00};
    const u_int8_t uid[11]={0x00,0x00,0xff,0x04,0xfc,0xd4,0x4a,0x02,0x00,0xe0,0x00};
//    初始化PN532到normal状态
//    const u_int8_t p2p_normal[24]={0x55,0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00};
//    const u_int8_t p2p_inital[16]={0x00,0x00,0xff,0x09,0xf7,0xD4,0x4A,0x01,0x02,0x00,0xFF,0xFF,0x00,0x00,0xE1,0x00};
//    const u_int8_t p2p_activate[11]={0x00,0x00,0xff,0x04,0xfc,0xd4,0x50,0x01,0x00,0xdb,0x00};
//    const u_int8_t p2p_spped[12]={0x00,0x00,0xff,0x05,0xfb,0xd4,0x4e,0x01,0x00,0x00,0xdd,0x00};
//    const u_int8_t p2p_target_dep[46]={0x00,0x00,0xff,0x27,0xd9,0xd4,0x8c,0x00,0x08,0x00,0x12,0x34,0x56,0x40,0x01,0xfe,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xff,0xff,0xaa,0x99,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0x00,0x00,0x21,0x00};
//    const u_int8_t TgGetdata[9]={0x00,0x00,0xff,0x02,0xfe,0xd4,0x86,0xa6,0x00};
//    const u_int8_t TgSendData[12]={0x00,0x00,0xff,0x05,0xfB,0xd4,0x8e,0xAA,0xAA,0xAA,0xA0,0x00};
//    const u_int8_t InJumpForDep[17]={0x00,0x00,0xff,0x0a,0xf6,0xd4,0x56,0x01,0x02,0x01,0x00,0xff,0xff,0x00,0x00,0xd4,0x00};
//    const u_int8_t InDataExchange[26]={0x00,0x00,0xff,0x13,0xed,0xd4,0x40,0x01,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x73,0x00};
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
    printf("wake: ");
    send_recv(fd,wake,24,read_buf,15);
//    send_recv(fd,p2p_normal,24,read_buf,15);
//    send_recv(fd,p2p_inital,16,read_buf,35);
//    send_recv(fd,p2p_activate,11,read_buf,16);
//    send_recv(fd,p2p_spped,11,read_buf,16);
//    send_recv(fd,p2p_target_dep,46,read_buf,6);

//    for(i=0;i<15;i++)
//    {
//        printf("%02X ",read_buf[i]);
//    }
//    printf("\n");


    while(1)
    {
        //读取卡信息
        printf("UID:");
        send_recv(fd,uid,11,read_buf,25);

//        send_recv(fd,InJumpForDep,17,read_buf,6);
//        send_recv(fd,TgGetdata,9,read_buf,14);
//        send_recv(fd,InDataExchange,26,read_buf,6);
//       send_recv(fd,TgSendData,12,read_buf,50);
//        sleep(5);
    }
    return 0;
}


int send_recv(int fd,const u_int8_t write_buf[],int send_len,u_int8_t read_buf[],int recv_len)
{
    int i;//temp_buf下标
    int j;//read_buf下标
    int n = 0;
    int len = 0;//接受到数据的长度
    unsigned char temp_buf[256];//保存数据  read_buf接收数据
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
//                printf("%02X \n",temp_buf[i]);

            }
            len += n;
        }


        if(len == recv_len)
        {
            for(i=0;i<len;i++)
            {
                read_buf[i]=temp_buf[i];
                printf("%02X ",temp_buf[i]);
            }

            printf("\n");
            break;
        }
    }
}
