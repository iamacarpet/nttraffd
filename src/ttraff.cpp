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
// Our database stuff...
#include <sstream>
#include "curl/curl.h"
// Our headers
#include "functions.h"
#include "main.h"

// Use the standard namespace - Unsure.. Was Just there...
using namespace std;

template <class T> string to_string (const T& t){
	stringstream ss;
	ss << t;
	return ss.str();
}

struct adapter {
	unsigned long in_dev;
	unsigned long out_dev;
	unsigned long in_diff;
	unsigned long out_diff;
	unsigned long in_dev_last;
	unsigned long out_dev_last;
	int gotbase;
	unsigned long megcounti, megcounto;
	unsigned long megi;
	unsigned long mego;
	unsigned long dbsync_in_diff;
	unsigned long dbsync_out_diff;
};

map<string, adapter, less<string> >  stats;

static char errorBuffer[CURL_ERROR_SIZE];

static string buffer;

static int writer(char *data, size_t size, size_t nmemb, std::string *buffer){
    // What we will return  
    int result = 0;
    // Is there anything in the buffer?
    if (buffer != NULL){
        // Append the data to the buffer
        buffer->append(data, size * nmemb);

        // How much did we write?
        result = size * nmemb;
    }
    return result;
}

bool write_to_db(string url, int clientid, string authKey, int day, int month, int year, string ifname, unsigned long in_diff, unsigned long out_diff){
	url += "?clientid=";
    url += to_string(clientid);
    url += "&authkey=";
    url += authKey;
    url += "&iface=";
    url += ifname;
    url += "&year=";
    url += to_string(year);
    url += "&month=";
    url += to_string(month);
    url += "&day=";
    url += to_string(day);
    url += "&in=";
    url += to_string(in_diff);
    url += "&out=";
    url += to_string(out_diff);
     
    CURL *curl;
    CURLcode result;

    curl = curl_easy_init();

    if (curl){
        // Now set up all of the curl options  
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HEADER, 0);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
  
        // Attempt to retrieve the remote page  
        result = curl_easy_perform(curl);  
  
        // Always cleanup
        curl_easy_cleanup(curl);
        
        // Did we succeed?  
        if (result == CURLE_OK){
            logWrite("Pushed to server OK");
            return true;
        } else {
            logWrite("Failed to push to server.");
            return false; 
        }
    } else {
        logWrite("Failed to load CURL module");
        return false;
    } 
}

unsigned long get_indiff(std::string iface){
	//logWrite("Getting indiff...");
	return stats[iface].dbsync_in_diff;
}

unsigned long get_outdiff(std::string iface){
	//logWrite("Getting outdiff...");
	return stats[iface].dbsync_out_diff;
}

void runttraffd(string url, int clientid, string authKey){
	struct tm *currtime;
	long tloc;
	bool sleepLogDone = false;

	time(&tloc);            // get time in seconds since epoch
	currtime = localtime(&tloc);    // convert seconds to date structure

	while (currtime->tm_year < 110){ // loop until ntp time is set (year >= 2010)
		if (!sleepLogDone)
			logWrite("NTP date has note been set. Sleeping until corrected...");
		sleepLogDone = true;
		sleep(15);
		time(&tloc);
		currtime = localtime(&tloc);
	}
	if (sleepLogDone)
		logWrite("NTP date has now been set. Starting process...");

	logWrite("Starting data collection...");

	int needcommit = 0;
	int commited = 0;
	int day, month, year;
	int ifl;
	char line[256];
	FILE *in;
	FILE *trace;

	while (1){
		time(&tloc);
		currtime = localtime(&tloc);

		day = currtime->tm_mday;
		month = currtime->tm_mon + 1;   // 1 - 12
		year = currtime->tm_year + 1900;
		if ((in = fopen("/proc/net/dev", "rb")) != NULL) {
			/* eat first two lines */
			fgets(line, sizeof(line), in);
			fgets(line, sizeof(line), in);

			//logWrite(" Starting to go through interfaces...");

			while (fgets(line, sizeof(line), in) != NULL) {
				//logWrite("  Reading interface...");
				ifl = 0;
				string ifname;

				while (line[ifl] != ':'){
					ifname += line[ifl];
					ifl++;
				}

				ifname = trim(ifname);

				//logWrite(ifname);

				line[ifl] = 0;

				unsigned long in_dev = 0;
				unsigned long out_dev = 0;

				sscanf(line + ifl + 1, "%lu %*ld %*ld %*ld %*ld %*ld %*ld %*ld %lu %*ld %*ld %*ld %*ld %*ld %*ld %*ld", &stats[ifname].in_dev, &stats[ifname].out_dev);

				//string logString;
				//logString = "   Interface: ";
				//logString += ifname.c_str();
				//logString += " In: ";
				//char s[(CHAR_BIT * sizeof stats[ifname].in_dev + 2) / 3 + 1];
				//sprintf(s, "%lu", stats[ifname].in_dev);
				//logString += s;
				//char p[(CHAR_BIT * sizeof stats[ifname].in_dev + 2) / 3 + 1];
				//sprintf(p, "%lu", stats[ifname].out_dev);
				//logString += " Out: ";
				//logString += p;
				//logWrite(logString);

				if (stats[ifname].gotbase == 0) {
					//logWrite("Setting base...");
					stats[ifname].in_dev_last = stats[ifname].in_dev;
					stats[ifname].out_dev_last = stats[ifname].out_dev;
					stats[ifname].gotbase = 1;
				}

				if (stats[ifname].in_dev_last > stats[ifname].in_dev){  // 4GB limit was reached or couter reseted
					stats[ifname].megi = (stats[ifname].in_dev_last >> 20) + (in_dev >> 20);        // to avarage loss and gain here to 0 over long time
					stats[ifname].in_diff = (stats[ifname].in_dev >> 20) * 2;       // to avarage loss and gain here to 0 over long time
					stats[ifname].in_dev_last = stats[ifname].in_dev;
				} else {
					stats[ifname].in_diff = (stats[ifname].in_dev - stats[ifname].in_dev_last) >> 20;       // MB
					stats[ifname].in_dev_last += (stats[ifname].in_diff << 20);
				}

				if (stats[ifname].out_dev_last > stats[ifname].out_dev){        // 4GB limit was reached or counter reseted
					stats[ifname].mego = (stats[ifname].out_dev_last >> 20) + (stats[ifname].out_dev >> 20);        // to avarage loss and gain here to 0 over long time
					stats[ifname].out_diff = (stats[ifname].out_dev >> 20) * 2;     // to avarage loss and gain here to 0 over long time
					stats[ifname].out_dev_last = stats[ifname].out_dev;
				} else {
					stats[ifname].out_diff = (stats[ifname].out_dev - stats[ifname].out_dev_last) >> 20;    // MB
					stats[ifname].out_dev_last += (stats[ifname].out_diff << 20);
				}

				if (stats[ifname].in_diff || stats[ifname].out_diff) {
					stats[ifname].dbsync_in_diff = stats[ifname].dbsync_in_diff + stats[ifname].in_diff;
					stats[ifname].dbsync_out_diff = stats[ifname].dbsync_out_diff + stats[ifname].out_diff;
					//char q[(CHAR_BIT * sizeof stats[ifname].in_diff + 2) / 3 + 1];
					//sprintf(q, "%lu", stats[ifname].in_diff);
					//char a[(CHAR_BIT * sizeof stats[ifname].out_diff + 2) / 3 + 1];
					//sprintf(a, "%lu", stats[ifname].out_diff);
					//string logStr = "Caught Difference - In: ";
					//logStr += q;
					//logStr += " MB - Out: ";
					//logStr += a;
					//logStr += " MB";
					//logWrite(logStr);
				}

				if (stats[ifname].dbsync_in_diff || stats[ifname].dbsync_out_diff){
					// Push down stats every 5 mins
                    if (currtime->tm_min % 5 == 0){
						if (write_to_db(url, clientid, authKey, day, month, year, ifname, stats[ifname].dbsync_in_diff, stats[ifname].dbsync_out_diff)){
						    stats[ifname].dbsync_in_diff = 0;
						    stats[ifname].dbsync_out_diff = 0;
						}
					}
				}

				if (stats[ifname].megi || stats[ifname].mego){  // leave trace in /tmp/.megc
					stats[ifname].megcounti = 0;
					stats[ifname].megcounto = 0;
					string fname = "/tmp/.megc.";
					fname += ifname;
					if ((trace = fopen(fname.c_str(), "r")) != NULL) {
						fgets(line, sizeof(line), trace);
						sscanf(line, "%lu:%lu", &stats[ifname].megcounti, &stats[ifname].megcounto);
						fclose(trace);
					}
					trace = fopen(fname.c_str(), "w");
					sprintf(line, "%lu:%lu", stats[ifname].megcounti + stats[ifname].megi, stats[ifname].megcounto + stats[ifname].mego);
					fputs(line, trace);
					fclose(trace);
					stats[ifname].megi = 0;
					stats[ifname].mego = 0;
				}

			}

			fclose(in);
		}
		sleep(58);
	}
}
