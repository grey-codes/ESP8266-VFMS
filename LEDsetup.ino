#define PIN_RED 5
#define PIN_GRN 4
#define PIN_BLU 0

#define ID_R 4
#define ID_G 2
#define ID_B 1

char LED_R, LED_G, LED_B;

char LED_Status;
void LEDsetup() {

	// Auxiliar variables to store the current output state
	LED_Status = 0;
	
	// Assign output variables to GPIO pins
	
	// Initialize the output variables as outputs
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_GRN, OUTPUT);
	pinMode(PIN_BLU, OUTPUT);
	
	// Set output variables as outputs
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_GRN, OUTPUT);
	pinMode(PIN_BLU, OUTPUT);
  
	//Set LEDs to ON state, this confirms correct initialization of the pins
	analogWrite(PIN_RED, 255);
	delay(300);
	analogWrite(PIN_GRN, 255);
	delay(300);
	analogWrite(PIN_BLU, 255);
	delay(300);
	
	
	// Set LEDs to OFF state
	analogWrite(PIN_RED, 0);
	analogWrite(PIN_GRN, 0);
	analogWrite(PIN_BLU, 0);
	
}

void update_led() {
	analogWrite(PIN_RED, LED_R);
	analogWrite(PIN_GRN, LED_G);
	analogWrite(PIN_BLU, LED_B);
}

void svLEDJSON() {
	String sid = session_init();
	LoginInfo linfo = get_login(sid);
	if (linfo.userID==-1) { //not logged in
    	server.send(403, "text/plain", "no.");
		return;
	}
    String res = "{\"red\":" + String(LED_R,10) + ","
	+ "\"green\":" + String(LED_G,10) + ","
	+ "\"blue\":" + String(LED_B,10) + "}" ;
    server.send(200, "application/json", res);
}

void svLEDSet() {
	if (!server.hasArg("red") && !server.hasArg("green") && !server.hasArg("blue")) {
    	server.send(400, "text/plain", "no.");
		return;
	}
	String sid = session_init();
	LoginInfo linfo = get_login(sid);
	if (linfo.userID==-1) { //not logged in
    	server.send(403, "text/plain", "no.");
		return;
	}
	if ((linfo.group & ID_R) == ID_R && server.hasArg("red")) {
    	String str = server.arg("red");
		LED_R = (char)str.toInt();
	}
	if ((linfo.group & ID_G) == ID_G && server.hasArg("green")) {
    	String str = server.arg("green");
		LED_G = (char)str.toInt();
	}
	if ((linfo.group & ID_B) == ID_B && server.hasArg("blue")) {
    	String str = server.arg("blue");
		LED_B = (char)str.toInt();
	}
	update_led();
    server.send(200, "text/plain", "ok.");
}