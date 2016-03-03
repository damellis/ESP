int vinpin = A0;
int voutpin = A1;
int gndpin = A2;
int zpin = A3;
int ypin = A4;
int xpin = A5;

void setup() {
  Serial.begin(9600);
  pinMode(vinpin, OUTPUT); digitalWrite(vinpin, HIGH);
  pinMode(gndpin, OUTPUT); digitalWrite(gndpin, LOW);
  pinMode(voutpin, INPUT);
  pinMode(xpin, INPUT);
  pinMode(ypin, INPUT);
  pinMode(zpin, INPUT);
}

void loop() {
  Serial.print(analogRead(xpin)); Serial.print("\t");
  Serial.print(analogRead(ypin)); Serial.print("\t");
  Serial.print(analogRead(zpin)); Serial.println();
}
