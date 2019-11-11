void svLogIn() {
    String sessID = session_init();
	DBG_OUTPUT_PORT.println("Simulate log in by " + sessID);
    server.send(200, "text/plain", "Simulate log in by " + sessID);
}

void svLogOut() {
    String sessID = session_init();
	DBG_OUTPUT_PORT.println("Simulate log out by " + sessID);
    server.send(200, "text/plain", "Simulate log out by " + sessID);
}

void svRegister() {
    String sessID = session_init();
	DBG_OUTPUT_PORT.println("Simulate register by " + sessID);
    server.send(200, "text/plain", "Simulate register by " + sessID);
}