
#include <MIDI.h>

#define RegionActiveSending 1

int num = 0;
MIDI_CREATE_DEFAULT_INSTANCE();  //启用MIDI库

void setup() {
  Serial.begin(31250);
  pinMode(12, INPUT_PULLUP);  //INPUT_PULLUP
  MIDI.begin(1);              //MIDI CH ALL Listen
  MIDI.setInputChannel(1);    //没什么用
}

int type, data1, data2;

void loop() {

  if (MIDI.read(1)) {  //MIDI CH1

    // if (type != MIDI.getType() && data1 != MIDI.getData1() && data2 != MIDI.getData2()) {
    type = MIDI.getType();
    data1 = MIDI.getData1();
    data2 = MIDI.getData2();

    Serial.println("---------");
    Serial.print(" type ");
    Serial.print(MIDI.getType());
    Serial.print(" data1 ");
    Serial.print(MIDI.getData1());
    Serial.print(" data2 ");
    Serial.println(MIDI.getData2());
    // }
    switch (MIDI.getType()) {
      case midi::NoteOn:     //if NoteOn
        digitalWrite(4, 1);  //Gate》HIGH
        break;
      case midi::NoteOff:
        digitalWrite(4, 0);  //Gate》HIGH
        break;
    }
  }
  if (MIDI.read(2)) {  //MIDI CH1

    switch (MIDI.getType()) {
      case midi::NoteOn:     //if NoteOn
        digitalWrite(7, 1);  //Gate》HIGH
        break;
      case midi::NoteOff:
        digitalWrite(7, 0);  //Gate》HIGH
        break;
    }
  }
}

