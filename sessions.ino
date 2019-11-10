void session_init() {
  if (server.hasHeader("Cookie")) {
    DBG_OUTPUT_PORT.println("Found cookie:");
    String cookie = server.header("Cookie");
    DBG_OUTPUT_PORT.println(cookie);
  } else {
    server.sendHeader("Set-Cookie", "test=cookie");
  }
}