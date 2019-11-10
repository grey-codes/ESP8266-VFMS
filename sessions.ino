#include <time.h>

void session_init() {
	if (server.hasHeader("Cookie")) {
		DBG_OUTPUT_PORT.println("Found cookie:");
		String cookie = server.header("Cookie");
		DBG_OUTPUT_PORT.println(cookie);
	} else {
        String ua = server.hasHeader("User-Agent") ? server.header("User-Agent") : "noagent";
        time_t t=time(NULL);
        String ts = String(t);

        MD5Builder md5;
        md5.begin();
        md5.add(ts);
        md5.add(ua);
        md5.calculate();

        DBG_OUTPUT_PORT.println(md5.toString());
		server.sendHeader("Set-Cookie", "sessID=" + md5.toString());
	}
}