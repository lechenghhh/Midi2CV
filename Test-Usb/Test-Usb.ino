#include <USB-MIDI.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();

int b = 0;
int note_no1, note_on_count1, tmp_last_note1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  // MIDI.read();
  if (MIDI.read()) {
    switch (MIDI.getType()) {
      case midi::NoteOn:  //if NoteOn

        note_no1 = MIDI.getData1()-21;

        Serial.print("note on ");
        Serial.println(note_no1);
        digitalWrite(13, HIGH);  //Gate》HIGH
        break;
      case midi::NoteOff:
        digitalWrite(13, LOW);  //Gate 》LOW
        Serial.println("note off ");
        break;
    }
  }
  // MIDI.sendNoteOn(42, 127, 1);
  // Serial.println("-------------------------");

  // put your main code here, to run repeatedly:
  // test();
}
void test() {
  delay(2000);
  // digitalWrite(13, b);
  if (b == 1) b = 0;
  else if (b == 0) b = 1;
  // digitalWrite(12, b);
  Serial.print("d13 ");
  Serial.println(b);
}
