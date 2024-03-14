#include <MIDI.h>
// #include <FlexiTimer2.h>
#include <SPI.h>                 //DAC通信用
MIDI_CREATE_DEFAULT_INSTANCE();  //MIDIライブラリを有効启用MIDI库

//2 4 7 gate
//3 5 6 CV
const int LDAC = 9;  //SPI trans setting
int note_no = 0;     //noteNo=21(A0)～60(A5) total 61,マイナスの値を取るのでint 因为取负值，所以int

int bend_range = 0;
int bend_msb = 0;
int bend_lsb = 0;
long after_bend_pitch = 0;

byte note_on_count = 0;       //当多个音符打开且其中一个音符关闭时，最后一个音符不消失。
unsigned long trigTimer = 0;  //for gate ratch

int p3 = 0, p5 = 0, p6 = 0;

void setup() {
  Serial.begin(115200);

  pinMode(LDAC, OUTPUT);  //DAC trans
  pinMode(SS, OUTPUT);    //DAC trans
  pinMode(2, OUTPUT);     //CLK_OUT
  pinMode(4, OUTPUT);     //CLK_OUT
  pinMode(7, OUTPUT);     //CLK_OUT
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  MIDI.begin(1);  // MIDI CH1をlisten

  delay(50);

  // TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  // TCCR2B = _BV(CS22);
}

int old_p3 = 0;
int filter_int = 10;
int isMidi = 0;
int testMod2 = 0;

void loop() {
  //循环测试
  // testMod2++;
  // if (testMod2 > 200) {
  //   testMod2 = 0;
  // }
  // analogWrite(3, testMod2);
  //-----------------------------midi operation----------------------------
  if (MIDI.read()) {  // 如果频道1有信号的话

    digitalWrite(2, HIGH);  //Gate》HIGH
    MIDI.setInputChannel(1);
    switch (MIDI.getType()) {

      case midi::ControlChange:
        switch (MIDI.getData1()) {  //D3 D5 D6
          default:
          case 1:  //modulation
            p3 = MIDI.getData2() << 1;
            // if (p3 - old_p3 < filter_int && p3 - old_p3 > -filter_int)
            //   analogWrite(3, p3);
            // old_p3 = p3;
            // OUT_PWM(3, MIDI.getData2() << 3);

            break;
        }
    }
  } else {
    digitalWrite(2, LOW);  //Gate》HIGH
  }
  analogWrite(3, p3);
}
