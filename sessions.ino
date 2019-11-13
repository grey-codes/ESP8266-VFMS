#include <time.h>
#include <stdio.h>
#include <string.h>

#define HASH_LEN 32
#define MAX_SESS 8 //8 max sessmaps
#define SESS_EXP 5*60 //sessmaps may be cannibalised after 5 minutes

struct sessmap {
	char hash[HASH_LEN];
	unsigned int userID;
	time_t lastUsed;
};

typedef struct sessmap SessionMap;

SessionMap sessmaps[MAX_SESS];

void init_sessmaps() {
	for (int i=0; i<MAX_SESS; i++) {
		strcpy(sessmaps[i].hash,"");
		sessmaps[i].userID = 0;
		sessmaps[i].lastUsed = 0;
	}
}

size_t find_sessmap(char *hash) {
	for (int i=0; i<MAX_SESS; i++) {
		if (strcmp(sessmaps[i].hash,hash) == 0) {
			return i;
		}
	}
	return -1;
}


size_t find_sessmap_uid(int userID) {
	for (int i=0; i<MAX_SESS; i++) {
		if ( sessmaps[i].userID == userID ){
			return i;
		}
	}
	return -1;
}


size_t find_empty_sessmap() {
	time_t ct = time(NULL);
	for (int i=0; i<MAX_SESS; i++) {
		time_t lu = sessmaps[i].lastUsed;
		if (lu == 0) {
			return i;
		} else if (ct - lu > SESS_EXP) {
			return i;
		}
	}
	return -1;
}

int map_sessmap(char *sessID, int userID) {
	//first, attempt to replace existing sessmap
	size_t i = find_sessmap(sessID);
	if (i!=-1) {
		sessmaps[i].userID = userID;
		sessmaps[i].lastUsed = time(NULL);
		return i;
	}
	//then try to use an empty sessmap
	i = find_empty_sessmap();
	if (i!=-1) {
		strcpy(sessmaps[i].hash,sessID);
		sessmaps[i].userID = userID;
		sessmaps[i].lastUsed = time(NULL);
		return i;
	}
	//otherwise, we can't
	return -1;
}

String session_id() {
  if (!server.hasHeader("Cookie"))
      return "";
  String cookies =	server.header("Cookie");
  int sessID_pos = cookies.indexOf("sessID");
  if (sessID_pos==-1)
      return "";
  int start = cookies.indexOf("sessID=")+7;
  if (start==6)
      return "";
  int end = cookies.indexOf(";",start);
  if (end==-1) {
      return cookies.substring(start);
  }
  return cookies.substring(start,end);
}

String session_init() {
	String sessID = session_id();
	if (sessID.length()>0) {
		return sessID;
	} else {
    String ua = server.hasHeader("User-Agent") ? server.header("User-Agent") : "noagent";
    time_t t=time(NULL);
    String ts = String(t);

    MD5Builder md5;
    md5.begin();
    md5.add(ts);
    md5.add(ua);
    md5.calculate();
    String md5s = md5.toString();

    DBG_OUTPUT_PORT.println("Initializing sessmap: " + md5s);
server.sendHeader("Set-Cookie", "sessID=" + md5s);

    return md5s;
	}
}