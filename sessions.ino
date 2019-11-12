#include <time.h>
#include <stdio.h>
#include <string.h>

#define HASH_LEN 32
#define MAX_SESS 8 //8 max sessions
#define SESS_EXP 5*60 //sessions may be cannibalised after 5 minutes

struct session {
  char hash[HASH_LEN];
  unsigned int userID;
  time_t lastUsed;
};

typedef struct session Session;

Session sessions[MAX_SESS];

void init_sessions() {
  for (int i=0; i<MAX_SESS; i++) {
    strcpy(sessions[i].hash,"");
    sessions[i].userID = 0;
    sessions[i].lastUsed = 0;
  }
}

Session *get_session_by_hash(char *hash) {
  size_t indx = find_session(hash);
  if (indx==-1)
    return NULL;
  return &(sessions[indx]);
}

size_t find_session(char *hash) {
  for (int i=0; i<MAX_SESS; i++) {
    if (strcmp(sessions[i].hash,hash) == 0) {
      return i;
    }
  }
  return -1;
}


size_t find_session_uid(int userID) {
  for (int i=0; i<MAX_SESS; i++) {
    if ( sessions[i].userID == userID ){
      return i;
    }
  }
  return -1;
}


size_t find_empty_session() {
  time_t ct = time(NULL);
  for (int i=0; i<MAX_SESS; i++) {
    time_t lu = sessions[i].lastUsed;
    if (lu == 0) {
      return i;
    } else if (ct - lu > SESS_EXP) {
      return i;
    }
  }
  return -1;
}

int map_session(char *sessID, int userID) {
  //first, attempt to replace existing session
  size_t i = find_session(sessID);
  if (i!=-1) {
    sessions[i].userID = userID;
    sessions[i].lastUsed = time(NULL);
    return i;
  }
  //then try to use an empty session
  i = find_empty_session();
  if (i!=-1) {
    strcpy(sessions[i].hash,sessID);
    sessions[i].userID = userID;
    sessions[i].lastUsed = time(NULL);
    return i;
  }
  //otherwise, we can't
  return -1;
}

String session_id() {
    if (!server.hasHeader("Cookie"))
        return "";
    String cookies =  server.header("Cookie");
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

        DBG_OUTPUT_PORT.println("Initializing session: " + md5s);
		server.sendHeader("Set-Cookie", "sessID=" + md5s);

        return md5s;
	}
}