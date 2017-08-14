// While running this test, the gas output should repeatedly
// cycle, 0.5 second on, 1.0 second off.  While holding the
// pushbutton, the relay wil cycle instead of the gas solenoid.
// The 5 LEDs will light, one at a time, as the output cycles
// A 3.6 Vrms (10.3 Vp-p)  1 kHz sine wave should be seen at the
// transformer input, and a ~40 Vrms (^112 Vp-p) waveform
// should appear at the transformer output.

//  Pin  Function
//   0   RS485 receive
//   1   RS485 transmit
//   2   Pushbutton - Turn on Pilot Gas
//   3   RS485 transmit enable
//   5   LED, Green #2
//   9   Timer Reset, HIGH = enable relay & gas
//  11   Timer Trigger, Rising Edge = turn on gas
//  12   Timer Trigger, Rising Edge = turn on relay
//  14   LED, Red
//  15   Transistor gain measurement
//  16   Flame Measurement Signal
//  17   LED, Green #3 (highest signal)
//  20   Input voltage (12V) measurement (divided by 11)
//  21   LED, Green #1 (lowest signal)
// A12/DAC  AC signal output

IntervalTimer dactimer;
uint16_t dacdata[32];
int led_state = 0;

void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(21, OUTPUT);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  delay(1);
  digitalWrite(9, HIGH); // enable timer chip
  analogWriteResolution(12);
  analogWriteFrequency(10, 1000);
  analogWrite(10, 2148);
  for (int i=0; i < 32; i++) {
    dacdata[i] = sinf(float(i) * (2.0 * 3.14159 / 32.0)) * 2046 + 2048;
  }
  dactimer.begin(dacupdate, 31.25);
}

void dacupdate() {
  static int index=0;
  analogWrite(A12, dacdata[index]);
  index = (index + 1) & 31;
}

void pulse() {
  int pin = 11;
  if (digitalRead(2) == LOW) pin = 12;
  digitalWrite(pin, HIGH);
  delayMicroseconds(50);
  digitalWrite(pin, LOW);
}

void loop() {
  pulse();
  delay(70);
  pulse();
  delay(200);
  pulse();
  delay(1230);
  
  digitalWrite(21, LOW);
  digitalWrite(5, LOW);
  digitalWrite(17, LOW);
  digitalWrite(14, LOW);
  digitalWrite(13, LOW);
  led_state = (led_state + 1) % 5;
  Serial.println(led_state);
  switch (led_state) {
    case 0: digitalWrite(21, HIGH); break;
    case 1: digitalWrite(5,  HIGH); break;
    case 2: digitalWrite(17, HIGH); break;
    case 3: digitalWrite(14, HIGH); break;
    default: digitalWrite(13, HIGH);
  }
}
