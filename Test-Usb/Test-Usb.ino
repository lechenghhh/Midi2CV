#include <USB-MIDI.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  MIDI.begin(1);
}
int b = 0;


void loop() {
  MIDI.read();
  // put your main code here, to run repeatedly:
  delay(2000);
  // digitalWrite(13, b);
  if (b == 1) b = 0;
  else if (b == 0) b = 1;
  // digitalWrite(12, b);
  Serial.print("d13 ");
  Serial.println(b);
}
