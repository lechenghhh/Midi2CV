#include <USB-MIDI.h>
#include <MIDI.h>

#include <SPI.h>  //DAC通信用

USBMIDI_CREATE_DEFAULT_INSTANCE();

const int LDAC = 9;  //SPI trans setting
int note_no1 = 0;    //noteNo=21(A0)～60(A5) total 61,マイナスの値を取るのでint 因为取负值，所以int
int note_no2 = 0;    //noteNo=21(A0)～60(A5) total 61,マイナスの値を取るのでint 因为取负值，所以int

int bend_range = 0;
int bend_msb = 0;
int bend_lsb = 0;
long after_bend_pitch = 0;

byte note_on_count1 = 0;      //当多个音符打开且其中一个音符关闭时，最后一个音符不消失。
byte note_on_count2 = 0;      //当多个音符打开且其中一个音符关闭时，最后一个音符不消失。
unsigned long trigTimer = 0;  //for gate ratch

byte clock_count0 = 0;
byte clock_count1 = 0;
byte clock_count2 = 0;
byte clock_max = 24;  //clock_max change by knob setting
byte clock_on_time = 0;
int clock_rate = 0;  //knob CVin

int tmp_last_note1 = -1, tmp_last_note2 = -1;

int cc_mode = 0;  //用于更改当前cc映射模式

const long cv[61] = {  // V/OCT LSB for DAC
  0, 68, 137, 205, 273, 341, 410, 478, 546, 614, 683, 751,
  819, 887, 956, 1024, 1092, 1161, 1229, 1297, 1365, 1434, 1502, 1570,
  1638, 1707, 1775, 1843, 1911, 1980, 2048, 2116, 2185, 2253, 2321, 2389,
  2458, 2526, 2594, 2662, 2731, 2799, 2867, 2935, 3004, 3072, 3140, 3209,
  3277, 3345, 3413, 3482, 3550, 3618, 3686, 3755, 3823, 3891, 3959, 4028, 4095
};
int p3 = 0, p5 = 0, p6 = 0;
int note4[4] = { 0, 0, 0, 0 };

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LDAC, OUTPUT);  //DAC trans
  pinMode(SS, OUTPUT);    //DAC trans
  pinMode(2, OUTPUT);     //CLK_OUT
  pinMode(4, OUTPUT);     //CLK_OUT
  pinMode(7, OUTPUT);     //CLK_OUT
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // SPI.begin();
  // SPI.setBitOrder(MSBFIRST);            // bit order
  // SPI.setClockDivider(SPI_CLOCK_DIV4);  // クロック(CLK)をシステムクロックの1/4で使用(16MHz/4)
  // SPI.setDataMode(SPI_MODE0);
}

void loop() {
  //-----------------------------midi operation----------------------------
  if (MIDI.read()) {  // 如果频道1有信号的话
    switch (MIDI.getType()) {
      case midi::Clock:
        clock_count0++;
        if (clock_count0 >= clock_max) {
          clock_count0 = 0;
        }
        if (clock_count0 == 1) {
          digitalWrite(2, HIGH);
        } else if (clock_count0 != 1) {
          digitalWrite(2, LOW);
        }
        // break;
      case midi::AfterTouchPoly:
        // if (cc_mode == 0) OUT_PWM(6, MIDI.getData2());  //3个cv映射输出力度cv
        break;
      case midi::PitchBend:
        bend_lsb = MIDI.getData1();  //LSB
        bend_msb = MIDI.getData2();  //MSB
        bend_range = bend_msb;       //0 to 127
        if (bend_range > 64) {
          after_bend_pitch = cv[note_no1] + cv[note_no1] * (bend_range - 64) * 4 / 10000;
          // OUT_CV1(after_bend_pitch);
        } else if (bend_range < 64) {
          after_bend_pitch = cv[note_no1] - cv[note_no1] * (64 - bend_range) * 4 / 10000;
          // OUT_CV1(after_bend_pitch);
        }
        break;

      case midi::AllNotesOff:
        clock_count1 = 0;
        note_on_count1 = 0;
        digitalWrite(4, LOW);  //Gate》LOW
        clock_count2 = 0;
        note_on_count2 = 0;
        digitalWrite(7, LOW);  //Gate》LOW
        break;
      case midi::Stop:
        note_on_count1 = 0;
        note_on_count2 = 0;
        tmp_last_note1 = -1;
        tmp_last_note2 = -1;
        // clock_count1 = 0;
        digitalWrite(4, LOW);  //Gate》LOW
        digitalWrite(7, LOW);  //Gate》LOW
        break;
      case midi::ControlChange:
        switch (MIDI.getData1()) {  //D3 D5 D6
          case 11:                  //切换时钟DIV
          case 21:
          case 31:
            clock_rate = MIDI.getData2() >> 5;
            break;
          case 12:  //切换四种模式 //change cc maping in modular
          case 22:
          case 32:
            cc_mode = MIDI.getData2() >> 5;
            break;
        }

        switch (cc_mode) {
          case 0:  //type=0 VEL/MOD/CC2 VEL请见Note On
            if (MIDI.getData1() == 1) OUT_PWM(5, MIDI.getData2());
            if (MIDI.getData1() == 2) OUT_PWM(6, MIDI.getData2());
            break;
          case 1:  //type=1 CC13/CC14/CC15
            if (MIDI.getData1() == 13) OUT_PWM(3, MIDI.getData2());
            if (MIDI.getData1() == 14) OUT_PWM(5, MIDI.getData2());
            if (MIDI.getData1() == 15) OUT_PWM(6, MIDI.getData2());
            break;
          case 2:  //type=2 CC23/CC24/CC25
            if (MIDI.getData1() == 23) OUT_PWM(3, MIDI.getData2());
            if (MIDI.getData1() == 24) OUT_PWM(5, MIDI.getData2());
            if (MIDI.getData1() == 25) OUT_PWM(6, MIDI.getData2());
            break;
          case 3:  //type=3 音符触发模式
            if (MIDI.getData1() == 33) OUT_PWM(3, MIDI.getData2());
            if (MIDI.getData1() == 34) OUT_PWM(5, MIDI.getData2());
            if (MIDI.getData1() == 35) OUT_PWM(6, MIDI.getData2());
            break;
        }
    }

    if (MIDI.getChannel() == 1) {  //MIDI CH1
      switch (MIDI.getType()) {
        case midi::NoteOn:  //if NoteOn
          note_on_count1++;
          note_no1 = MIDI.getData1() - 21;  //note number
          tmp_last_note1 = MIDI.getData1();
          if (note_no1 < 0) {
            note_no1 = 0;
          } else if (note_no1 >= 61) {
            note_no1 = 60;
          }
          digitalWrite(13, HIGH);  //Gate》HIGH
          // OUT_CV1(cv[note_no1]);   //V/OCT LSB for DAC》参照

          if (cc_mode == 0) OUT_PWM(3, MIDI.getData2());  //3个cv映射输出力度cv

          break;
        case midi::NoteOff:
          // if (note_on_count1 > 0) note_on_count1--;
          // if (note_on_count1 < 1) {
          if (tmp_last_note1 == MIDI.getData1())
            digitalWrite(13, LOW);  //Gate 》LOW
                                    // }
          Serial.println("note_off");
          break;
      }
    }

    if (MIDI.getChannel() == 2) { /*MIDI CH2*/
      switch (MIDI.getType()) {
        case midi::NoteOn:  //if NoteOn

          note_on_count2++;
          note_no2 = MIDI.getData1() - 21;  //note number
          tmp_last_note2 = MIDI.getData1();
          if (note_no2 < 0) {
            note_no2 = 0;
          } else if (note_no2 >= 61) {
            note_no2 = 60;
          }
          digitalWrite(7, HIGH);  //Gate》HIGH
          // OUT_CV2(cv[note_no2]);  //V/OCT LSB for DAC》参照

          break;
        case midi::NoteOff:  //if NoteOff 关闭后
          // if (note_on_count2 > 0) note_on_count2--;
          // if (note_on_count2 < 1) {
          if (tmp_last_note2 == MIDI.getData1())
            digitalWrite(7, LOW);  //Gate 》LOW
          // }
          break;

      }  //MIDI CH2
    }
  }
}
//DAC_CV output
void OUT_CV1(int cv) {
  Serial.print("OUT_CV1 ");
  Serial.println(cv);
  // digitalWrite(LDAC, HIGH);
  // digitalWrite(SS, LOW);
  // SPI.transfer((cv >> 8) | 0x30);  // H0x30=OUTA/1x
  // SPI.transfer(cv & 0xff);
  // digitalWrite(SS, HIGH);
  // digitalWrite(LDAC, LOW);
}

//DAC_CV2 output
void OUT_CV2(int cv2) {
  Serial.print("OUT_CV2 ");
  Serial.println(cv2);
  // digitalWrite(LDAC, HIGH);
  // digitalWrite(SS, LOW);
  // SPI.transfer((cv2 >> 8) | 0xB0);  // H0xB0=OUTB/1x
  // SPI.transfer(cv2 & 0xff);
  // digitalWrite(SS, HIGH);
  // digitalWrite(LDAC, LOW);
}
void OUT_PWM(int pin, int cc_value) {
  Serial.print(pin);
  Serial.print(" OUT_PWM ");
  Serial.println(cc_value);

  analogWrite(pin, cc_value << 1);
}