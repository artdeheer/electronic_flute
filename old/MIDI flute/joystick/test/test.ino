// Joystick pins
const int horPin = A7;   // Horizontal axis
const int verPin = A6;   // Vertical axis
const int selPin = 2;    // Select / pushbutton

void setup() {
  Serial.begin(9600);

  pinMode(selPin, INPUT_PULLUP); // Button reads HIGH when not pressed, LOW when pressed
}

void loop() {
  int hor = analogRead(horPin);
  int ver = analogRead(verPin);
  int sel = digitalRead(selPin);

  Serial.print("HOR: ");
  Serial.print(hor);
  Serial.print("\tVER: ");
  Serial.print(ver);
  Serial.print("\tSEL: ");
  Serial.println(sel == LOW ? "Pressed" : "Released");

  delay(100);
}
