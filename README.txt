-----------------------------------------------------------------
     _   ___ ___ ___ ___ _  _ __  __ ___ _  _ _____    __ ___ 
    /_\ / __/ __|_ _/ __| \| |  \/  | __| \| |_   _|  /  \_  )
   / _ \\__ \__ \| | (_ | .` | |\/| | _|| .` | | |   | () / / 
  /_/ \_\___/___/___\___|_|\_|_|  |_|___|_|\_| |_|    \__/___|
                                                            
-----------------------------------------------------------------
Author: 	Matt W. Martin, 4374851
		kaethis@tasmantis.net

Project:	CS3790, Assignment 02:
		Scheduler & Writer (w/ NCURSES & PIPES!)

Date Issued:	30-Sept-2015
Date Archived:	29-Oct-2015

File:		README.txt


Comments:	This assignment consists of two programs:

		- scheduler:	Designed to simulate the round-
				robin schedulig scheme of an
				operating system NEWING, RUNNING
				and BLOCKING a series of procs.
				Each process is allowed 3 real-
				time seconds per full cycle.

		- writer:	A process that writes a message
				up to 30 characters to a design-
				ated pipe (which is read by the
				OUPUT WINDOW in the scheduler).
				NOTE:  This prog is designed to
				       be executed by scheduler
				       and should NOT be run by
				       the user.

		In short, the scheduler program is divided into
		three main processes: the INPUT WINDOW, which
		handles user input; the OUTPUT WINDOW, which
		displays a message written by the writer program;
		and the SCHEDULER, which handles the scheduling
		and execution of up to 10 writers. 

		The INPUT WINDOW communicates with the SCHEDULER
		via an ANONYMOUS PIPE containing the message that 
		the new writer, executed by the SCHEDULER, will
		write every second.

		Each writer communicates to the OUTPUT WINDOW via
		a single NAMED PIPE, the contents of which are
		displayed on the OUTPUT WINDOW.


Instructions:	Input a message into the INPUT WINDOW which will
		execute up to 10 instances of a program that
		writes the message to the OUTPUT WINDOW (via a
		NAMED PIPE).  Each writer process is alloted 3
		full seconds of runtime before BLOCKING.


MAKE:		./scheduler	Creates both scheduler & writer.
		./clean		Removes programs (if they exist).
		./archive	Creates archive in parent-dir.


GIT Repository:	 https://github.com/kaethis/CS3790_Assign02.git


Notes:		- If a new instance of the writer program is
		  created (i.e., you input a message) while the
		  very last writer is currently RUNNING, that
		  last writer will sometimes have the opportunity
		  to write for an additional second or two before
		  BLOCKING.  This is a known issue (as in, I've
		  already brought it up with Carlacci).

		- This program was written using the NCURSES API.
		  You may need to download that library first in
		  order to compile the scheduler.

		- This is my very first time using both PIPES and
		  NCURSES, so I don't expect their implementation
		  to be quite perfect (especially the PIPES!).
