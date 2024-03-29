#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

//everything in hex except for error count, which is in decimal

FILE *target;
char *location;
int fd;
struct {
  int total;
  int current;
  int spin;
} bar;

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

void set_mincount(int fd, int mcount)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error tcgetattr: %s\n", strerror(errno));
        return;
    }

    tty.c_cc[VMIN] = mcount ? 1 : 0;
    tty.c_cc[VTIME] = 5;        /* half second timer */

    if (tcsetattr(fd, TCSANOW, &tty) < 0)
        printf("Error tcsetattr: %s\n", strerror(errno));
}

void getchip(int chip)
{
  printf("insert chip %d, then press enter", chip);
  getchar();
  write(fd, "#", 1);
}

void reset(int chip)
{
  fclose(target);
  target = fopen(location, "r");
  for (; chip > 0; chip--)
    while (fgetc(target) != 'z')
      ;
  write(fd, "#", 1);
}

void sendpage()
{
  char c;
  char holder[2];
  while ((c = fgetc(target)) != 'x' && c != 'z') {
    holder[0] = c;
    holder[1] = '\0';
    write(fd, holder, 1);
  }
  if (c == 'x')
    write(fd, "ff", 2);
  else
    write(fd, "ffff", 4);
}

int errusr(int chip)
{
  char buff[20];
  int len;
  int werr, rerr;
  write(fd, "#", 1);
  len = read(fd, buff, sizeof(buff)-1);
  buff[len] = '\0';
  printf("errors during write: %s\n", buff);
  write(fd, "#", 1);
  len = read(fd, buff, sizeof(buff)-1);
  buff[len] = '\0';
  printf("errors during check: %s\n", buff);
  printf("re-write? (y/n)");
  if (getchar() != 'y')
    chip++;
  getchar();
  return(chip);
}

void initbar(int written)
{
  int i;
  char c;
  printf("\033[s");
  fclose(target);
  target = fopen(location, "r");
  bar.current = 0;
  bar.spin = 0;
  for (i = 0; (c = fgetc(target)) != 'z';)
    if (c == 'x')
      i++;
  bar.total = i;
  printf("[                    ] %d/%d pages     ",
	 bar.current, bar.total); //20 spaces
  if (!written)
    printf("written");
  else
    printf("checked");
  fflush(stdout);
}

void incbar()
{
  int i;
  int shift;
  int after;
  if (bar.current < bar.total)
    bar.current++;
  if (bar.total > 0)
    after = (bar.current*20)/bar.total;
  else
    after = 20;
  bar.spin++;
  if (bar.spin == 16)
    bar.spin = 0;
  printf("\033[u");
  printf("[");
  for (i = 0; i < after; i++)
    printf("=");
  if (bar.current < bar.total)
    switch (bar.spin/4) {
    case 0:
      printf("-");
      break;
    case 1:
      printf("\\");
      break;
    case 2:
      printf("|");
      break;
    case 3:
      printf("/");
      break;
    }
  for (i = after+1; i < 20; i++)
    printf(" ");
  printf("]");
  printf(" %d/%d pages", bar.current, bar.total);
  fflush(stdout);
}

void endbar()
{
  printf("\n");
  fflush(stdout);
}

void sendhighlow(int high_prog) {
	if (high_prog)
		write(fd, "1", 1);
	else
		write(fd, "0", 1);
	write(fd, "#", 1);
}

int main(int argc, char* argv[])
{
    char *portname = "/dev/ttyUSB0";
    int rlen, wlen;
    int i;
    int chip;
    int done;
    int written;
    int high_prog;
    unsigned char buff[20];

    if (argc != 2) {
      printf("Usage: ./romsend <file to upload>\n");
      return (-1);
    }
    printf("Reset arduino, then press enter");
    getchar();
    location = argv[1];
    target = fopen(location, "r");
    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Error opening %s: %s\n", portname, strerror(errno));
        return (-1);
    }
    /*baudrate 500000, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(fd, B500000);
    //set_mincount(fd, 0);                /* set to pure timed read */
    
    printf("Value of upper most addr bit (1/0): ");
    if(getchar() == '1')
	high_prog = 1;
    else
	high_prog = 0;
    getchar();
    for (chip = 0, done = 0, written = 0; chip <= 4; done = 0, written = 0) {
      getchip(chip);
      while (!done) {
	read(fd, buff, sizeof(buff)-1);
	switch(buff[0]) {
	case 'r':
	  if (written)
	    endbar();
	  initbar(written);
	  written = 1;
	  reset(chip);
	  break;
	case 'p':
	  sendpage();
	  incbar();
	  break;
	case 'e':
	  endbar();
	  chip = errusr(chip);
	  done = 1;
	  break;
	case 'h':
	  sendhighlow(high_prog);
	  break;
	}
      }
    }

    close(fd);
    fclose(target);
    return (0);
}
