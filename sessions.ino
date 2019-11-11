#include <time.h>

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