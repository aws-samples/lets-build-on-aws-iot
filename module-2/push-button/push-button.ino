#define BUTTON_PIN 23   // push-button pin

int buttonState = 0;    // variable for reading the push-button status

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  // initialize the push-button pin as an input
  // and enable the internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the state of the push-button value:
  buttonState = digitalRead(BUTTON_PIN);

  // check if the push-button is pressed
  // if it is, the buttonState is LOW:
  if (buttonState == LOW) {
    Serial.println("Button Down");
  } else {
    Serial.println("Button Up");
  }
}
