#include <time.h>

void session_init() {
	if (server.hasHeader("Cookie")) {
		DBG_OUTPUT_PORT.println("Found cookie:");
		String cookie = server.header("Cookie");
		DBG_OUTPUT_PORT.println(cookie);
	} else {
        String ua = server.hasHeader("User-Agent") ? server.header("User-Agent") : "noagent";
        time_t t=time(NULL);
        String hashIn = String(t) + ua;
		server.sendHeader("Set-Cookie", "test=cookie");
	}
}