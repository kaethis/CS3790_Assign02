/* Author:      Matt W. Martin, 4374851
 *              kaethis@tasmantis.net
 *
 * Project:     CS3790, Assignment 02:
 *              Scheduler & Writer (w/ NCURSES & PIPES!)
 *
 * File:        writer.cpp */

#include <fcntl.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]){

	int  fd;
	int  buffer_size = 30;
	const char  *mypipe = "/tmp/mypipe";


	fd = open(mypipe, O_WRONLY);


	std::string  msg = argv[1];

	for(int i = 0; i < (buffer_size - msg.length() - 2); i++)
		msg.append(" ");


	while(true){

		write(fd, msg.c_str(), buffer_size);
		sleep(1);
	}

	close(fd);

	return 0;
}
