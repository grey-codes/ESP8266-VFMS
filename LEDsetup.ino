void LEDsetup() {

	// Auxiliar variables to store the current output state
	String output5State = "off";
	String output4State = "off";
	String output0State = "off";
	
	// Assign output variables to GPIO pins
	const int RED = 5;
	const int GREEN = 4;
	const int BLUE = 0;
	
	// Initialize the output variables as outputs
	pinMode(RED, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(BLUE, OUTPUT);
	
	// Set output variables as outputs
	pinMode(RED, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(BLUE, OUTPUT);
  
	//Set LEDs to ON state, this confirms correct initialization of the pins
	analogWrite(RED, 255);
	delay(300);
	analogWrite(GREEN, 255);
	delay(300);
	analogWrite(BLUE, 255);
	delay(300);
	
	
	// Set LEDs to OFF state
	analogWrite(RED, 0);
	analogWrite(GREEN, 0);
	analogWrite(BLUE, 0);
	
}