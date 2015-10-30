/* Author:      Matt W. Martin, 4374851
 *              kaethis@tasmantis.net
 *
 * Project:     CS3790, Assignment 02:
 *              Scheduler & Writer (w/ NCURSES & PIPES!)
 *
 * File:        scheduler.cpp */

#include <fcntl.h>
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct sigaction newhandler,
		 oldhandler;

const char  *mypipe = "/tmp/mypipe";

int  fd_named;
int  fd_anon[2];

bool  msg_avail = false;

int  pid[10];
int  num_procs = 0;
int  current = 0;

char buffer[30];


void checkAnonPipe(int code){
/* This FUNCTION sets msg_avail to TRUE (via signal SIGUSR1).  This signifies that
 *  there is a message available to be read in the ANONYMOUS PIPE. */

	msg_avail = true;
}

char* readAnonPipe(){
// This FUNCTION reads and RETURNS the contents of the ANONYMOUS PIPE.

	msg_avail = false;


	read(fd_anon[0], buffer, sizeof(buffer));

	char*  msg = buffer;


	return msg;
}

void writeAnonPipe(std::string msg){
// This FUNCTION writes a string message to the ANONYMOUS PIPE.

	// Write the remainder of the buffer as  whitespace characters:
	for(int i = 0; i < (sizeof(buffer) - msg.length() - 2); i++)
		msg.append(" ");

	write(fd_anon[1], msg.c_str(), sizeof(buffer));
}

char* readNamedPipe(){
// This FUNCTION reads and RETURNS the message contents of the NAMED PIPE.

	mkfifo(mypipe, 0666);

	fd_named = open(mypipe, O_RDONLY);

	read(fd_named, buffer, sizeof(buffer));

	char  *msg = buffer;


	return msg;
}

void execute(std::string msg){
// This FUNCTION creates a new instance of the writer program.

	pid[num_procs] = fork();

	if(pid[num_procs] == 0)
		execl("./writer", "writer", msg.c_str(), NULL);

	if(num_procs != 0)
		kill(pid[num_procs], SIGSTOP);


	num_procs++;
}

void next(int code){
/* This FUNCTION (via signal SIGALRM) stops the current process scheduled to run
 *  before proceeding to the next process in the list of PIDs. */

	if(num_procs > 0){

		kill(pid[current], SIGSTOP);

		current++;

		if(current == num_procs)
			current = 0;

		kill(pid[current], SIGCONT);
	}
}

int main(void){

	/* Create the ANONYMOUS PIPE.  This is used sending a message from the CHILD
	 *  to the PARENT.  The contents of this message are then passed as an
	 *  ARGUMENT when executing the writer program. */
	pipe(fd_anon);


	// Install SIGNAL HANDLERS:

	sigset_t  sig;

	sigemptyset(&sig);

	newhandler.sa_mask = sig;
	newhandler.sa_flags = 0;
	newhandler.sa_handler = next;

	if(sigaction(SIGALRM, &newhandler, &oldhandler) == -1){

		printf("\nERROR INSTALLING SIGALRM!\n");
		exit(-1);
	}

	newhandler.sa_handler = checkAnonPipe;

	if(sigaction(SIGUSR1, &newhandler, &oldhandler) == -1){

		printf("\nERROR INSTALLING SIGUSR1!\n");
		exit(-1);
	}


	int  ppid = getpid();


	int  cpid = fork();

	if(cpid == 0){
	// IF CHILD, initiate the NCURSES ENVIRONMENT.

		/* NOTE: It's worth mentioning that this process will later be
		 * 	  FORKED again to create a second process that will handle
		 *  	  the INPUT WINDOW while this one handles the OUTPUT WINDOW.
		 *	 From hereon, I'll refer to this process as CHILD0 and the
		 *	  CHILD of this process as CHILD1. */ 

		initscr();
		cbreak();
		noecho();
		keypad(stdscr, TRUE);
		nodelay(stdscr,TRUE);

		start_color();
		init_pair(1, COLOR_WHITE, COLOR_BLUE);

		refresh();



		// Create the OUTPUT WINDOW:

		int  wid = 10; 
		int  len = 30;

		int  y_min = 1;
		int  y_max = (wid-2);
		int  x_min = 1;
		int  x_max = (len-2);

		int  y_curr = y_min;
		int  x_curr = x_min;

		WINDOW  *win = newwin(wid, len, 5, 1);

		box(win, 0, 0);

		wbkgd(win, COLOR_PAIR(1));

		wrefresh(win);

		wmove(win, y_curr, x_curr);

		refresh();


		cpid = fork();

		if(cpid == 0){
		// IF CHILD1, handle the OUTPUT WINDOW...

			char *msg;

		  output_start:
	
			msg = readNamedPipe();


			getyx(win, y_curr, x_curr);

			if(y_curr > y_max)
				y_curr = y_min;


			wmove(win, y_curr, x_curr);

			wprintw(win, msg);

			wrefresh(win);

			wmove(win, (y_curr+1), x_min);

			refresh();

	
			goto output_start;


		} else{
		// IF CHILD0, handle the INPUT WINDOW...

			/* NOTE: Remember how I created the OUTPUT WINDOW before the
			 *	  FORK?  Because that WINDOW now exists in both the
			 *	  context of CHILD0 and CHILD1, I'm going to delete
			 *	  it in CHILD0 before proceeding to the creation of
			 *	  the INPUT WINDOW.
			 *	 The reason I created the OUTPUT WINDOW first was
			 *	  because if it was initialized inside of CHILD1, it
			 *	  caused a protential issue where if CHILD0 finished
			 *	  creating the INPUT WINDOW before CHILD1 finished
			 *	  its WINDOW, it would affect the way the latter was
			 *	  being drawn. */

			delwin(win);


			// Create INPUT WINDOW:

			wid = 3;
			len = 30;

			y_max = (wid-2);
			x_max = (len-2);

			win = newwin(wid, len, 22, 1);

			box(win, 0, 0);

			wbkgd(win, COLOR_PAIR(1));

			wrefresh(win);


			std::string  msg;

		  input_start:

			wmove(win, y_min, x_min);

			refresh();

			getyx(win, y_curr, x_curr);


			int ch;

			while(ch = getch()){

				if(ch == '\n')
					break;

				if(ch != ERR){

					if(ch == KEY_BACKSPACE){

						if(strlen(msg.c_str()) != 0){

							msg.pop_back();

							getyx(win, y_curr, x_curr);

							wmove(win,
							      y_curr, (x_curr-1));

							wprintw(win, " ");

							wmove(win,
							      y_curr, (x_curr-1));
						}

					} else if(ch == KEY_UP ||
						  ch == KEY_DOWN ||
						  ch == KEY_LEFT ||
						  ch == KEY_RIGHT){
			
						; /* Ignore input of arforementioned
						   *  key codes in IF STMT. */

					} else{

						if(strlen(msg.c_str()) < x_max){

							std::string  chr;

							chr.push_back((char)ch);

							msg.push_back((char)ch);

							
							wprintw(win, chr.c_str());
						}
					}

					wrefresh(win);

					getyx(win, y_curr, x_curr);

					wmove(win, y_curr, x_curr);

					refresh();

				}
			}


			writeAnonPipe(msg);

			/* Signal the PARENT that there's a message in the ANONYMOUS
			 *  PIPE (see line above).  This simply changes the msg_avail
			 *  BOOLEAN to TRUE in the context of the PARENT. */
			kill(ppid, SIGUSR1);


			msg.clear();

			wmove(win, y_min, x_min);

			refresh();

			for(int i = x_min; i <= x_max; i++)
				wprintw(win, " ");

			wrefresh(win);

			goto input_start;

		}

	} else{
	// IF PARENT, begin the round-robin SCHEDULER routine...

		char  *msg;

	  scheduler_start:

		if(msg_avail){

			msg = readAnonPipe();

			execute(msg);
		}

			
		alarm(3);
		pause();

		goto scheduler_start;
	}

	return 0;
}
