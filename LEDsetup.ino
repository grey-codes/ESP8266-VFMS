#define PIN_RED 5
#define PIN_GRN 4
#define PIN_BLU 0

#define ID_R 4
#define ID_G 2
#define ID_B 1

//adafruit's gamma LUT 
const uint8_t PROGMEM gamma8[] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

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
	analogWrite(PIN_RED, (int)(pgm_read_byte(&gamma8[LED_R])) );
	analogWrite(PIN_GRN, (int)(pgm_read_byte(&gamma8[LED_G])) );
	analogWrite(PIN_BLU, (int)(pgm_read_byte(&gamma8[LED_B])) );
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