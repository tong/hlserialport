
#define HL_NAME(n) hlserialport_##n

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <hl.h>

speed_t toBaudConstant(int baudRate) {
  switch(baudRate) {
    case 0: return B0;
    case 50: return B50;
    case 75: return B75;
    case 110: return B110;
    case 134: return B134;
    case 150: return B150;
    case 200: return B200;
    case 300: return B300;
    case 600: return B600;
    case 1200: return B1200;
    case 1800: return B1800;
    case 2400: return B2400;
    case 4800: return B4800;
    case 9600: return B9600;
    case 19200: return B19200;
    case 38400: return B38400;
    case 57600: return B57600;
    case 115200: return B115200;
    case 230400: return B230400;
    #if defined(__linux__)
    case 460800: return B460800;
    case 500000: return B500000;
    case 576000: return B576000;
    case 921600: return B921600;
    case 1000000: return B1000000;
    case 1152000: return B1152000;
    case 1500000: return B1500000;
    case 2000000: return B2000000;
    case 2500000: return B2500000;
    case 3000000: return B3000000;
    case 3500000: return B3500000;
    case 4000000: return B4000000;
    #endif
  }
  return -1;
}

int toDataBitsConstant(int dataBits) {
  switch(dataBits) {
    case 8: default: return CS8;
    case 7: return CS7;
    case 6: return CS6;
    case 5: return CS5;
  }
  return -1;
}

HL_PRIM int HL_NAME(open_port)( vbyte *path, int baudRate, int dataBits ) {

    const char *_path = hl_to_utf8( (uchar*)path );
    const speed_t _baudRate = toBaudConstant( baudRate );
    const int _dataBits = toDataBitsConstant( dataBits );

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

    cfsetospeed( &tty, _baudRate );
    cfsetispeed( &tty, _baudRate );

    tcflush( fd, TCIOFLUSH ); // throw away all the buffered data

    tty.c_cflag |= (CLOCAL | CREAD); 
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= _dataBits;
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
    return read( fd, (char*)buf, sizeof(buf) );
}

HL_PRIM int HL_NAME(read_char)( int fd ) {
    unsigned char chr;
    //int len = read( fd, &chr, 1 );
    read( fd, &chr, 1 );
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
    return tcflush( fd, TCIOFLUSH ) == 0;
}

HL_PRIM bool HL_NAME(drain)( int fd ) {
    return tcdrain( fd ) == 0;
}

DEFINE_PRIM(_I32, open_port, _BYTES _I32 _I32);
DEFINE_PRIM(_VOID, close_port, _I32);
DEFINE_PRIM(_I32, read, _I32 _BYTES _I32);
DEFINE_PRIM(_I32, read_char, _I32);
DEFINE_PRIM(_I32, write, _I32 _BYTES _I32 _I32);
DEFINE_PRIM(_BOOL, flush, _I32);
DEFINE_PRIM(_BOOL, drain, _I32);
