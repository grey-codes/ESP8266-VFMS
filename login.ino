#ifndef LOGIN_FILENAME
#define LOGIN_FILENAME "/logins.dat"
#endif

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

int validReq() {
    if( ! server.hasArg("username") || ! server.hasArg("password") 
        || server.arg("username") == NULL || server.arg("password") == NULL) { // no username/password?
        server.send(400, "text/plain", "Error 400 - Invalid Request");         // return invalid request
        return 0;
    }
    return 1;
}

int infoRequirements(String un, String pw) {
    if (un.length()>USERNAME_MAX_LEN) return 0;
    if (un.length()<USERNAME_MIN_LEN) return 0;
    if (pw.length()>PASSWORD_MAX_LEN) return 0;
    if (pw.length()<PASSWORD_MIN_LEN) return 0;
    return 1;
}

struct logininfo findLoginByName(String target) {
    LoginInfo l;
    strcpy(l.username,"");
    strcpy(l.password,"");
    l.userID = -1;

    unsigned int uid;
    char group;
    char un[sizeof(l.username)];
    char pw[sizeof(l.password)];
    unsigned char un_ar[sizeof(l.username)];
    unsigned char pw_ar[sizeof(l.password)];
    unsigned char uid_ar[sizeof(uid)];
    unsigned char group_ar[sizeof(group)];

    const char *un_target = target.c_str();

    File logins = filesystem->open(LOGIN_FILENAME,"r+");
    logins.setTimeout(10);
    do {
        logins.read(un_ar,sizeof(un_ar));
        logins.read(pw_ar,sizeof(pw_ar));
        logins.read(uid_ar,sizeof(uid_ar));
        logins.read(group_ar,sizeof(group_ar));
        memcpy(un,un_ar,sizeof(un));
        memcpy(pw,pw_ar,sizeof(pw));
        memcpy(&uid,uid_ar,sizeof(uid));
        memcpy(&group,group_ar,sizeof(group));
        
        if (strcmp(un,un_target)==0) {
            memcpy(l.username,un,sizeof(l.username));
            memcpy(l.password,pw,sizeof(l.password));
            l.userID = uid;
            l.group = group;
	        DBG_OUTPUT_PORT.println("Found info for " + String(uid,10) + ":" + String(un) );
            return l;
        }
    } while (logins.peek()!='\0');
    logins.close();
}

unsigned int nextUserID() {
    LoginInfo l;
    unsigned int ret;
    char group;
    unsigned char un_ar[sizeof(l.username)];
    unsigned char pw_ar[sizeof(l.password)];
    unsigned char uid_ar[sizeof(ret)];
    unsigned char group_ar[sizeof(group)];
    File logins = filesystem->open(LOGIN_FILENAME,"r+");
    logins.setTimeout(10);
    do {
        logins.read(un_ar,sizeof(un_ar));
        logins.read(pw_ar,sizeof(pw_ar));
        logins.read(uid_ar,sizeof(uid_ar));
        logins.read(group_ar,sizeof(group_ar));
        memcpy(&ret,uid_ar,sizeof(ret));
    } while (logins.peek()!='\0');
    logins.close();
    ret++;
    return ret;
}

void appendNewUser(String username, String password) {
    unsigned int uid = nextUserID();
    char group = 0;
    File logins = filesystem->open(LOGIN_FILENAME,"a+");
    logins.setTimeout(10);
    MD5Builder md5;
    md5.begin();
    md5.add(password);
    md5.calculate();
    String md5s = md5.toString();
    /*
    logins.println(username);
    logins.println(md5s);
    logins.println(String(uid,10));
    */
    LoginInfo l;
    unsigned char un_ar[sizeof(l.username)];
    unsigned char pw_ar[sizeof(l.password)];
    unsigned char uid_ar[sizeof(uid)];
    unsigned char group_ar[sizeof(group)];
    memcpy(un_ar,username.c_str(),sizeof(un_ar));
    memcpy(pw_ar,password.c_str(),sizeof(pw_ar));
    memcpy(uid_ar,&uid,sizeof(uid));
    memcpy(group_ar,&group,sizeof(group));
    logins.write(un_ar,sizeof(un_ar));
    logins.write(pw_ar,sizeof(pw_ar));
    logins.write(uid_ar,sizeof(uid));
    logins.write(group_ar,sizeof(group));
    logins.close();
	DBG_OUTPUT_PORT.println("Wrote new user: " + username + "\n" + md5s + "\n" + String(uid,10));
}

void svLogIn() {
    if (!validReq())
        return;
    char hash[PASSWORD_HASH_LEN];
    String sessID = session_init();
	DBG_OUTPUT_PORT.println("Initializing login by " + sessID);

    String username = server.arg("username");
    String password = server.arg("password");

    username.toLowerCase();
    username.trim();
    username.replace("\n",""); //don't f*ck my database :(
    
    LoginInfo l = findLoginByName(username);
    if (l.userID==-1) {
        server.send(403, "text/plain", "Error 403 - Invalid username or password");         // return invalid request
        return;
    }

    MD5Builder md5;
    md5.begin();
    md5.add(password);
    md5.calculate();
    String md5s = md5.toString();
    strcpy(hash,md5s.c_str());

    if (strcmp(hash,l.password)!=0) {
	    DBG_OUTPUT_PORT.println("Received: " + md5s);
	    DBG_OUTPUT_PORT.println("Compare to: " + String(l.password));
        server.send(403, "text/plain", "Error 403 - Invalid username or password");         // return invalid request
        return;
    }

    map_sessmap(sessID.c_str(),l);

    server.sendHeader("Set-Cookie", "userID=" + String(l.userID,10));
    handleFileRead("/succ.htm");
    //server.send(200, "text/plain", "Successful log in by " + sessID);
}

void svLogOut() {
    String sessID = session_init();
	DBG_OUTPUT_PORT.println("Initialize log out by " + sessID);

    size_t sessionID = find_sessmap(sessID.c_str());
    if (sessionID==-1) {
        server.send(403, "text/plain", "Error 403 - Not logged in");         // return invalid request
        return;
    }
    
    LoginInfo dummy;
    dummy.userID=-1;
    map_sessmap(sessID.c_str(),dummy);

    server.sendHeader("Set-Cookie", "userID=-1");
    handleFileRead("/succ.htm");
    //server.send(200, "text/plain", "Simulate log out by " + sessID);
}

void svRegister() {
    if (!validReq())
        return;
    String sessID = session_init();
	DBG_OUTPUT_PORT.println("Initializing register by " + sessID);

    String username = server.arg("username");
    String password = server.arg("password");

    username.toLowerCase();
    username.trim();
    username.replace("\n",""); //don't f*ck my database :(

    if (!infoRequirements(username,password)) {
        server.send(406, "text/plain", "Error 406 - Information not acceptable");         // return invalid request
        return;
    }

    LoginInfo l = findLoginByName(username);
    if (l.userID!=-1) {
        server.send(406, "text/plain", "Error 406 - User already exists");         // return invalid request
        return;
    }

    appendNewUser(username,password);
    handleFileRead("/succ.htm");
    //server.send(200, "text/plain", "Simulate register by " + sessID);

}

void svLoginJSON() {
    String sessID = session_init();
	LoginInfo linfo = get_login(sessID);
    String res = "{\"userName\":\"" + String(linfo.username) + "\",\"userID\":"+String(linfo.userID,10)+",\"group\":"+String(linfo.group,10)+"}";
    server.send(200, "application/json", res);
}