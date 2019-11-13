#define USERNAME_MIN_LEN 3
#define USERNAME_MAX_LEN 32

#define PASSWORD_MIN_LEN 8
#define PASSWORD_MAX_LEN 128

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
	DBG_OUTPUT_PORT.println("Simulate register by " + sessID);

    String username = server.arg("username");
    String password = server.arg("password");

    if (!infoRequirements(username,password)) {
        server.send(406, "text/plain", "Error 406 - Information not acceptable");         // return invalid request
        return;
    }

    //server.send(200, "text/plain", "Simulate register by " + sessID);
}