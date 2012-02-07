// Include the Linux Input/Output Library.
#include <iostream>
#include <fstream>
// Include Strings Library.
#include <string>
#include <time.h>
#include <map>
// Some crap...
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sstream>
// Our headers
#include "ttraff.h"
#include "functions.h"
#include "config.h"
#include "libs/pidfile.h"

using namespace std;

int main(){
	/* Our process ID and Session ID */
	pid_t pid, sid;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		return 0;
	}
	/* If we got a good PID, then we can exit the parent process. */
	if (pid > 0) {
		return 1;
	}

	/* Change the file mode mask */
	umask(0);

	/* Open any logs here */
	//cout << "Opening the log..." << endl;
	openLog();
	//cout << "Done opening the log..." << endl;
	logWrite("Starting daemon...");

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		/* Log the failure */
		return 0;
	}

	/* Change the current working directory */
	//if ((chdir("/var/ttraff/")) < 0) {
	//	return 0;
	//}
    
    pidfile pf("/var/run/nttraffd.pid");
    
    pf.write();

	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	// Define error holding variable...
	char *zErrMsg = 0;
	// Define holding variable...
	int rc;
    
    ConfigFile cf("/etc/nttraffd.conf");
    
    string urlString = cf.Value("server", "url");
    int clientID = cf.Value("client", "id");
    string authKey = cf.Value("client", "authcode");
		
	runttraffd(urlString, clientID, authKey);
	
	closeLog();
	return 0;
}