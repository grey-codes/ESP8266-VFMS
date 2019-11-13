#define HASH_LEN 32
#define MAX_SESS 8 //8 max sessmaps
#define SESS_EXP 60 //sessmaps may be cannibalised after 5 minutes

#ifndef LOGIN_STRUCTS
#define LOGIN_STRUCTS 1

#define USERNAME_MIN_LEN 3
#define USERNAME_MAX_LEN 32

#define PASSWORD_MIN_LEN 8
#define PASSWORD_MAX_LEN 128
#define PASSWORD_HASH_LEN 32

struct logininfo {
  char username[USERNAME_MAX_LEN+1]; //+1 for null terminators aaaa
  char password[PASSWORD_HASH_LEN+1]; //+1 for null terminators aaaa
  unsigned int userID;
  char group;
};

typedef struct logininfo LoginInfo;

struct sessmap {
	char hash[PASSWORD_HASH_LEN+1]; // do not forget terminator !!!
	struct logininfo userInfo;
	time_t lastUsed;
};

typedef struct sessmap SessionMap;
#endif

SessionMap sessmaps[MAX_SESS];

void copy_login(struct logininfo *dest, struct logininfo *src) {
  (*dest).userID = (*src).userID;
  strcpy((*dest).username,(*src).username);
  strcpy((*dest).password,(*src).password);
  (*dest).group = (*src).group;
}

void init_sessmaps() {
	for (int i=0; i<MAX_SESS; i++) {
		strcpy(sessmaps[i].hash,"");
		sessmaps[i].userInfo.userID = -1;
		sessmaps[i].lastUsed = 0;
	}
}

size_t find_sessmap(const char *hash) {
	for (int i=0; i<MAX_SESS; i++) {
		if (strcmp(sessmaps[i].hash,hash) == 0 && sessmaps[i].userInfo.userID!=-1) {
			sessmaps[i].lastUsed = time(NULL);
			return i;
		}
	}
	return -1;
}

size_t find_sessmap_uid(int userID) {
	for (int i=0; i<MAX_SESS; i++) {
		if ( sessmaps[i].userInfo.userID == userID ){
			sessmaps[i].lastUsed = time(NULL);
			return i;
		}
	}
	return -1;
}


size_t find_empty_sessmap() {
	time_t ct = time(NULL);
	for (int i=0; i<MAX_SESS; i++) {
		time_t lu = sessmaps[i].lastUsed;
		if (lu == 0 || sessmaps[i].userInfo.userID==-1 ) {
			return i;
		} else if (ct - lu > SESS_EXP) {
			return i;
		}
	}
	return -1;
}

int map_sessmap(const char *sessID, struct logininfo uinfo) {
	//first, attempt to replace existing sessmap
	size_t i = find_sessmap(sessID);
	if (i!=-1) {
		sessmaps[i].lastUsed = time(NULL);
		copy_login(&(sessmaps[i].userInfo),&uinfo);
		return i;
	}
	//then try to use an empty sessmap
	i = find_empty_sessmap();
	if (i!=-1) {
		strcpy(sessmaps[i].hash,sessID);
		copy_login(&(sessmaps[i].userInfo),&uinfo);
		sessmaps[i].lastUsed = time(NULL);
		return i;
	}
	//otherwise, we can't
	return -1;
}

struct logininfo get_login(String sid) {
  size_t sessionID = find_sessmap(sid.c_str());
  if (sessionID==-1) {
    static struct logininfo blankLogin;
    strcpy(blankLogin.username,"");
    strcpy(blankLogin.password,"");
    blankLogin.group = 0;
    blankLogin.userID = -1;
    return blankLogin;
  }
  return sessmaps[sessionID].userInfo;
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

    //DBG_OUTPUT_PORT.println("Initializing session: " + md5s);
    server.sendHeader("Set-Cookie", "sessID=" + md5s);
    server.sendHeader("Set-Cookie", "userID=-1" ); //clear out any previous cookie data when making a new session

    return md5s;
	}
}