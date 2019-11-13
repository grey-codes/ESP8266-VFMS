#define PIN_RED 5
#define PIN_GRN 4
#define PIN_BLU 0

#define ID_R 4
#define ID_G 2
#define ID_B 1

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
	if ((LED_Status & ID_R) == ID_R) {
		analogWrite(PIN_RED, 255);
	} else {
		analogWrite(PIN_RED, 0);
	}
	if ((LED_Status & ID_G) == ID_G) {
		analogWrite(PIN_GRN, 255);
	} else {
		analogWrite(PIN_GRN, 0);
	}
	if ((LED_Status & ID_B) == ID_B) {
		analogWrite(PIN_BLU, 255);
	} else {
		analogWrite(PIN_BLU, 0);
	}
}

void svLEDJSON() {
    String res = "{\"status\":\"" + String(LED_Status,10) + "\"}";
    server.send(200, "application/json", res);
}

void svLEDSet() {
	if (!server.hasArg("led")) {
    	server.send(400, "text/plain", "no.");
		return;
	}
	String sid = session_init();
	LoginInfo linfo = get_login(sid);
	if (linfo.userID==-1) { //not logged in
    	server.send(403, "text/plain", "no.");
		return;
	}
    String leds = server.arg("led");
	char ledstat = (char)leds.toInt();
	if ((linfo.group & ID_B) == ID_B) {
		LED_Status=LED_Status^(ledstat & ID_B);
	}
	if ((linfo.group & ID_G) == ID_G) {
		LED_Status=LED_Status^(ledstat & ID_G);
	}
	if ((linfo.group & ID_R) == ID_R) {
		LED_Status=LED_Status^(ledstat & ID_R);
	}
	update_led();
    server.send(200, "text/plain", "ok.");
}