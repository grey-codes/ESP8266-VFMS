#define USERNAME_MIN_LEN 3
#define USERNAME_MAX_LEN 32

#define PASSWORD_MIN_LEN 8
#define PASSWORD_MAX_LEN 128
#define PASSWORD_HASH_LEN 32

#ifndef LOGIN_FILENAME
#define LOGIN_FILENAME "/logins.dat"
#endif

struct logininfo {
    char username[USERNAME_MAX_LEN+1]; //+1 for null terminators aaaa
	char password[PASSWORD_HASH_LEN+1]; //+1 for null terminators aaaa
    unsigned int userID;
    char group;
};

typedef struct logininfo LoginInfo;

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

struct logininfo fuidLoginByName(String target) {
    LoginInfo l;
    strcpy(l.username,"");
    strcpy(l.password,"");
    l.userID = -1;
    String un, pw, uid_str, group_str;
    unsigned int uid;
    char group;
    File logins = filesystem->open(LOGIN_FILENAME,"r+");
    logins.setTimeout(10);
    do {
        un = logins.readStringUntil('\n');
        pw = logins.readStringUntil('\n');
        uid_str = logins.readStringUntil('\n');
        group_str = logins.readStringUntil('\n');
        un.trim();
        pw.trim();
        uid_str.trim();
        uid = uid_str.toInt();
        group = (char)group_str.toInt();
	    //DBG_OUTPUT_PORT.println("Read info for " + un);
        un.toLowerCase();
        un.trim();
        if (un.equals(target)) {
            strcpy(l.username,un.c_str());
            strcpy(l.password,pw.c_str());
            l.userID = uid;
            l.group = group;
	        DBG_OUTPUT_PORT.println("Found info for " + String(uid,10) + ":" + un );
            return l;
        }
    } while (un.length()!=0);
    logins.close();
}

unsigned int nextUserID() {
    String un, dummy;
    unsigned int uid;
    unsigned int ret = -1;
    File logins = filesystem->open(LOGIN_FILENAME,"r+");
    logins.setTimeout(10);
    do {
        un = logins.readStringUntil('\n'); //username
        dummy = logins.readStringUntil('\n'); //pw hash
        dummy = logins.readStringUntil('\n'); //user id
        dummy.trim();
        uid = dummy.toInt();
        dummy = logins.readStringUntil('\n'); //group binary
	    //DBG_OUTPUT_PORT.println("Read info for " + un + "," + pw + "," + String(uid,10) );
        if (un.length()!=0) {
            ret = uid;
        }
    } while (un.length()!=0);
    logins.close();
    ret++;
    return ret;
}

void appendNewUser(String username, String password) {
    unsigned int id = nextUserID();
    File logins = filesystem->open(LOGIN_FILENAME,"a+");
    logins.setTimeout(10);
    logins.println(username);
    MD5Builder md5;
    md5.begin();
    md5.add(password);
    md5.calculate();
    String md5s = md5.toString();
    logins.println(md5s);
    logins.println(String(id,10));
    logins.close();
	DBG_OUTPUT_PORT.println("Wrote new user: " + username + "\n" + md5s + "\n" + String(id,10));
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
    
    LoginInfo l = fuidLoginByName(username);
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

    map_sessmap(sessID.c_str(),l.userID);

    server.sendHeader("Set-Cookie", "userID=" + String(l.userID,10));
    server.send(200, "text/plain", "Successful log in by " + sessID);
}

void svLogOut() {
    if (!validReq())
        return;
    String sessID = session_init();
	DBG_OUTPUT_PORT.println("Initialize log out by " + sessID);

    size_t sessionID = fuid_sessmap(sessID.c_str());
    if (sessionID==-1) {
        server.send(403, "text/plain", "Error 403 - Not logged in");         // return invalid request
        return;
    }

    
    map_sessmap(sessID.c_str(),-1);

    server.sendHeader("Set-Cookie", "userID=-1");
    server.send(200, "text/plain", "Simulate log out by " + sessID);
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

    LoginInfo l = fuidLoginByName(username);
    if (l.userID!=-1) {
        server.send(406, "text/plain", "Error 406 - User already exists");         // return invalid request
        return;
    }

    server.send(200, "text/plain", "Simulate register by " + sessID);

    appendNewUser(username,password);
}