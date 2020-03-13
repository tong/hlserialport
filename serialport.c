
#define HL_NAME(n) hlserialport_##n

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <hl.h>

HL_PRIM int HL_NAME(open_port)( vbyte *path, int baudRate ) {

    const char *_path = hl_to_utf8( (uchar*)path );
    //printf("############ %s\n",_path );

    //int fd = open( _path, O_RDWR | O_NOCTTY | O_NDELAY );
    //O_NDELAY //Use non-blocking I/O. On some systems this also means the RS232 DCD signal line is ignored
    //int fd = open( _path, O_RDWR | O_NOCTTY );
    int flags = (O_RDWR | O_NOCTTY | O_SYNC);
    int fd = open( _path, flags );
    if( fd == -1 ) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        //perror("open_port: Unable to open ");
        return fd;
    }
    if( !isatty(fd) ) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return fd;
    }

    struct termios tty;

    if( tcgetattr( fd, &tty ) < 0 ) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    //TODO configuration

    speed_t speed = (speed_t)baudRate;
    cfsetospeed( &tty, speed );
    cfsetispeed( &tty, speed );

    tty.c_cflag |= (CLOCAL | CREAD); 
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB; 

    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 0;

    if( tcsetattr(fd, TCSANOW, &tty) != 0 ) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }

    return fd;
}

HL_PRIM void HL_NAME(close_port)( int fd ) {
    close( fd );
}

HL_PRIM int HL_NAME(read)( int fd, vbyte *buf, int size ) {
    int len = read( fd, (char*)buf, sizeof(buf) );
    //printf("############ read %i\n", r );
    return len;
}

HL_PRIM int HL_NAME(read_char)( int fd ) {
    unsigned char chr;
    //int len = read( fd, &chr, 1 );
    read( fd, &chr, 1 );
    //printf("############ len %i\n", len );
    //printf("############ chr %i\n", chr );
    return chr;
}

HL_PRIM int HL_NAME(write)( int fd, vbyte *buf, int pos, int len ) {
    //printf("############ write %i %i %i\n", fd, pos, len );
   // char *_buf = hl_to_utf8( (uchar*)buf );
   // write(fd, msg, sizeof(msg));
    write( fd, (char*)buf, sizeof(buf) );
    return 0;
}

HL_PRIM bool HL_NAME(flush)( int fd ) {
    int r = tcflush( fd, TCIOFLUSH );
    return r == 0;
}

HL_PRIM bool HL_NAME(drain)( int fd ) {
    int r = tcdrain( fd );
    return r == 0;
}

/*
HL_PRIM int HL_NAME(set_interface_attribs)( int fd ) {
    return 0;
}
*/

/*
HL_PRIM int HL_NAME(set_baudrate)( int fd, int baudRate ) {
    //int speed = B9600;
    printf("#######set_baudrate##### \n" );
    //printf("############ %d\n", speed );
    
    struct termios tty;
    if(tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }
    //int speed = B115200;
    int speed = baudRate;
    cfsetospeed( &tty, (speed_t)speed );
    cfsetispeed( &tty, (speed_t)speed );

    return 0;
}
*/

DEFINE_PRIM(_I32, open_port, _BYTES _I32);
DEFINE_PRIM(_VOID, close_port, _I32);
DEFINE_PRIM(_I32, read, _I32 _BYTES _I32);
DEFINE_PRIM(_I32, read_char, _I32);
DEFINE_PRIM(_I32, write, _I32 _BYTES _I32 _I32);
DEFINE_PRIM(_BOOL, flush, _I32);
DEFINE_PRIM(_BOOL, drain, _I32);

//DEFINE_PRIM(_I32, set_baudrate, _I32 _I32);