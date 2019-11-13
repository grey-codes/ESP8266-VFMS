#define USERNAME_MIN_LEN 3
#define USERNAME_MAX_LEN 32

#define PASSWORD_MIN_LEN 8
#define PASSWORD_MAX_LEN 128
#define PASSWORD_HASH_LEN 32

#define LOGIN_FILENAME "/logins.dat"

struct logininfo {
    char username[USERNAME_MAX_LEN];
	char password[PASSWORD_HASH_LEN];
    unsigned int userID;
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

struct logininfo findLoginByName(String target) {
    LoginInfo l;
    strcpy(l.username,"");
    strcpy(l.password,"");
    l.userID = -1;
    String un, pw, inds;
    unsigned int ind;
    File logins = filesystem->open(LOGIN_FILENAME,"r+");
    logins.setTimeout(10);
    do {
        un = logins.readStringUntil('\n');
        pw = logins.readStringUntil('\n');
        inds = logins.readStringUntil('\n');
        ind = inds.toInt();
	    //DBG_OUTPUT_PORT.println("Read info for " + un);
        un.toLowerCase();
        un.trim();
        if (un.equals(target)) {
            strcpy(l.username,un.c_str());
            strcpy(l.password,pw.c_str());
            l.userID = ind;
	        DBG_OUTPUT_PORT.println("Found info for " + String(ind,10) + ":" + un );
            return l;
        }
    } while (un.length()!=0);
    logins.close();
}

unsigned int nextUserID() {
    String un, pw, inds;
    unsigned int ind;
    unsigned int ret = -1;
    File logins = filesystem->open(LOGIN_FILENAME,"r+");
    logins.setTimeout(10);
    do {
        un = logins.readStringUntil('\n');
        pw = logins.readStringUntil('\n');
        inds = logins.readStringUntil('\n');
        ind = inds.toInt();
	    //DBG_OUTPUT_PORT.println("Read info for " + un + "," + pw + "," + String(ind,10) );
        if (un.length()!=0) {
            ret = ind;
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
    String sessID = session_init();
	DBG_OUTPUT_PORT.println("Simulate log in by " + sessID);
    server.send(200, "text/plain", "Simulate log in by " + sessID);
}

void svLogOut() {
    if (!validReq())
        return;
    String sessID = session_init();
	DBG_OUTPUT_PORT.println("Simulate log out by " + sessID);
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

    if (!infoRequirements(username,password)) {
        server.send(406, "text/plain", "Error 406 - Information not acceptable");         // return invalid request
        return;
    }

    LoginInfo l = findLoginByName(username);
    if (l.userID!=-1) {
        server.send(406, "text/plain", "Error 406 - User already exists");         // return invalid request
        return;
    }

    server.send(200, "text/plain", "Simulate register by " + sessID);

    appendNewUser(username,password);
}