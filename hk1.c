/* Harman Kardon AVR 525,325, 7200 serial control program 
a hodge-podge of examples thrown together in no perticular order 
PurpleJesus Apr 28, 2007 
GCC 
Do with this whatever you need to do.. 
If you find this program useful... wake the neighbors by pressing '+' to increase volume 
Set up for ttyS0 serial port, or COM1. 
*/ 
 
 
#include <stdio.h>   /* Standard input/output definitions */ 
#include <string.h>  /* String function definitions */ 
#include <unistd.h>  /* UNIX standard function definitions */ 
#include <fcntl.h>   /* File control definitions */ 
#include <errno.h>   /* Error number definitions */ 
#include <termios.h> /* POSIX terminal control definitions */ 
 
 
int writeport(int fd, char *chars) { 
        int len = strlen(chars); 
        //chars[len] = 0x0d; // stick a <CR> after the command 
        chars[len+1] = 0x00; // terminate the string properly 
        int n = write(fd, chars, strlen(chars)); 
        if (n < 0) { 
                fputs("write failed!\n", stderr); 
                return 0; 
        } 
        return 1;                                                                                                                                                                                                                
} 
 
int readport(int fd, char *result) { 
        int iIn = read(fd, result, 254); 
        result[iIn-1] = 0x00; 
        if (iIn < 0) { 
                if (errno == EAGAIN) { 
                        printf("SERIAL EAGAIN ERROR\n"); 
                        return 0; 
                } else { 
                        printf("SERIAL read error %d %s\n", errno, strerror(errno)); 
                        return 0; 
                } 
        }                                        
        return iIn; 
} 
 
 
int getch(void)  //this is to get a key without waiting for enter to be pressed...  I think it temporally turns off 'line buffering' not sure though. 
{ 
int ch; 
struct termios oldt; 
struct termios newt; 
tcgetattr(STDIN_FILENO, &oldt); /*store old settings */ 
newt = oldt; /* copy old settings to new settings */ 
newt.c_lflag &= ~(ICANON | ECHO); /* make one change to old settings in new settings */ 
tcsetattr(STDIN_FILENO, TCSANOW, &newt); /*apply the new settings immediatly */ 
ch = getchar(); /* standard getchar call */ 
tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /*reapply the old settings */ 
return ch; /*return received char */ 
} 
 
 
 
int fd;   //this will be the handle for the serial port 



int initport(int fd) {
        struct termios options;
        // Get the current options for the port...
        tcgetattr(fd, &options);
        // Set the baud rates to 4800...
        cfsetispeed(&options, B38400);
        cfsetospeed(&options, B38400);
        // Enable the receiver and set local mode...
        options.c_cflag |= (CLOCAL | CREAD);
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        tcsetattr(fd, TCSANOW, &options);
        return 1;
};


 
int max = 7; 
unsigned char command[][5]={            //Define the key commands and the data to be sent with them 
                                         
        {' ',   0x80,0x70,0xcc,0x33},   // video 
        {'v',   0x80,0x70,0xcb,0x34},   // tape //looks like I need to look deeper into structs...  maybe add some text to these... 
        {'t',   0x80,0x70,0xcc,0x33},   // video 
        {'+',   0x80,0x70,0xc7,0x38},   // Volume up 
        {'-',   0x80,0x70,0xc8,0x37},   // Volume down 
        {'d',   0x80,0x70,0xd0,0x2f},   // DVD 
        {'m',   0x80,0x70,0xc1,0x3e},   // Mute 
        {'x',   0x00,0x00,0x00,0x00}    // button to push to exit. 
         
}; 
//**************************************************************************************************** 
*********** 

/* 
static int loop = 0; 
char sCmd[5]; 
int dogcat,indx = 3; 
*/

 main(int argc, char **argv) { 
char sCmd[5]; 
int dogcat,indx = 3; 
char buf[512];
 
                 
        fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);  //Change for whatever serial port... 
        if (fd == -1) { 
                perror("open_port: Unable to open /dev/ttyS0 - ");  //opps something bad happened... 
                return 1; 
        } else { 
                fcntl(fd, F_SETFL, 0); 
        } 
         
        //printf("baud=%d\n", getbaud(fd)); 
        initport(fd); 
        //printf("baud=%d\n", getbaud(fd)); 
 
        printf("\n\nHarmam Kardon serial remote control\n"); 
        printf("tested on AVR 525, should work with 325 and 7200 also\n"); 
        printf("Press ^c to exit\n\n"); 
         
        printf("Command Keys:"); 
        for (int loop =0; loop <max; loop++) 
                { 
                printf(" %c ", command[loop][0]); 
                } 
        printf("\n"); 
         
        //////////////////////////////////////////////////////////////////////////////////////// 
       while (1)
	{
	int n;
       if ( n=readport(fd,buf)>0)
        			
				for (int i=0;i<n;i++) if (buf[i]>31) printf(buf);
	}

        while (0 != 1)  //main loop, run forever... ^c to force quit 
        { 
                unsigned char key ='q'; 
                key =getch();  //get a keystroke from the keyboard 
                         
                for (indx =0;indx < max; indx++)  //loop through, see if it matches anything 
                { 
                        if (key == command[indx][0]) 
                        {printf("%c ",key); 
         
                        sCmd[0]=command[indx][1];  // load the command to be sent. 
                        sCmd[1]=command[indx][2]; 
                        sCmd[2]=command[indx][3]; 
                        sCmd[3]=command[indx][4]; 
                        sCmd[4]=0x00; 
                        if (!writeport(fd, sCmd)){ 
                                        printf("write failed\n");  //something when horribly wrong..  
                        close(fd); 
                        return 1; 
                } 
                break; 
                } 
                } 
         
         
         
} 
}
