#include <fcntl.h>
#include <pty.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


void set_noecho(int fd)
{
	struct termios stermios;

	if (tcgetattr(fd, &stermios) < 0)
	{
		return;
	}

	/* turn off echo */
	stermios.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);

	tcsetattr(fd, TCSANOW, &stermios);
}


void set_noblocking(int fd)
{
	/* set non blocking */
	int flags = fcntl(fd, F_GETFL, 0);

	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


int main(int argc, const char *argv[])
{
	(void)argc;
	(void)argv;
	printf("Start main\n");

	int  master;
	int  slave;
	char name[100]; /* Buffer large enough for device name? */

	printf("Open\n");
	int retval = openpty(&master, &slave, name, NULL, NULL);
	if (retval < 0)
	{
		return EXIT_FAILURE;
	}

	/* No echo en the terminal */
	set_noecho(master);
	/* No blocking when trying to read */
	set_noblocking(master);

	printf("Streaming data to: %s\n", name);

	/* Streaming data... */
	int index = 0;
	while (1)
	{
		/* Sleep half a second */
		usleep(500000);
		/* Temp buffer */
		char buffer[1000];
		/* Message to stream */
		snprintf(buffer, sizeof(buffer), "Hello World %20d\n\r", index);
		index++;

		/* Write message to pty */
		if (write(master, buffer, strlen(buffer)) < 0)
		{
			break;
		}

		/* Try to read a character */
		char    c;
		ssize_t read_count = read(master, &c, 1);
		/* Did we get a character? * */
		if (read_count == 1)
		{
			/* Print what we got.. */
			printf("Read [%c]\n", c);
			if (c == 'x')
			{
				printf("Got an [%c] exiting...\n", c);
				break;
			}
		}
	}

	/* Cleanup */
	printf("Close\n");
	close(master);
	close(slave);

	return EXIT_SUCCESS;
}
