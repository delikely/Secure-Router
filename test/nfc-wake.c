//唤醒pn532
////发送的命令(十六进制）：
//55 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 03 FD D4 14 01 17 00
//成功的话PN532就会返回  
//00 00 FF 00 FF 00 00 00 FF 02 FE D5 15 16 00
//缺陷返回的数据没有以十六进制输出

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
    int n = 0;      
    char read_buf[256];
    char write_buf[256];
	char wake[24]={0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x03, 0xfd, 0xd4, 0x14, 0x01, 0x17, 0x00};
    struct termios opt; 
    
    fd = open("/dev/ttyS1", O_RDWR|O_NOCTTY|O_NDELAY);
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

    n = write(fd, wake, 24);
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
              
        printf("Len %d \n", len);
        printf("%s \n", write_buf);
       for(i=0;i<len;i++)
	   {
		   printf("%c ",read_buf[i]);
	   }
       // n = write(fd, write_buf, len);
      
		printf("\n %s ",read_buf);
	   	printf("write %d chars\n",n);
        
        sleep(2);
    }
	return 0;	
}
