int validReq() {
    if( ! server.hasArg("username") || ! server.hasArg("password") 
        || server.arg("username") == NULL || server.arg("password") == NULL) { // no username/password?
        server.send(400, "text/plain", "Error 400 - Invalid Request");         // return invalid request
        return 0;
    }
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
    server.send(200, "text/plain", "Simulate register by " + sessID);
}