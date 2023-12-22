import processing.serial.*;

Serial myPort;  // Create object from Serial class
int resetValue = 0;

void setup() {
  size(400, 200);
  // Modify the line below to match the COM port of your Arduino

  String portName = Serial.list()[2]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 9600);

}

void draw() {
  background(255);

  // Read data from Arduino
  while (myPort.available() > 0) {
    String inString = myPort.readStringUntil('\n');
    if (inString != null) {
      inString = trim(inString); // Remove whitespace characters
      resetValue = int(inString);
    }
  }

  // Display the received reset value
  textSize(20);
  fill(0);
  textAlign(CENTER, CENTER);
  text("Reset Value: " + resetValue, width/2, height/2);
}
