#include <MIDI.h>
// #include <FlexiTimer2.h>
#include <SPI.h>                 //DAC通信用
MIDI_CREATE_DEFAULT_INSTANCE();  //MIDIライブラリを有効启用MIDI库

const int LDAC = 9;  //SPI trans setting
int note_no = 0;     //noteNo=21(A0)～60(A5) total 61,マイナスの値を取るのでint 因为取负值，所以int

int bend_range = 0;
int bend_msb = 0;
int bend_lsb = 0;
long after_bend_pitch = 0;

byte note_on_count = 0;       //当多个音符打开且其中一个音符关闭时，最后一个音符不消失。
unsigned long trigTimer = 0;  //for gate ratch

byte clock_count = 0;
byte clock_max = 24;  //clock_max change by knob setting
byte clock_on_time = 0;
int clock_rate = 0;  //knob CVin

int cc_mode = 0;  //用于更改当前cc映射模式

const long cv[61] = {  // V/OCT LSB for DAC
  0, 68, 137, 205, 273, 341, 410, 478, 546, 614, 683, 751,
  819, 887, 956, 1024, 1092, 1161, 1229, 1297, 1365, 1434, 1502, 1570,
  1638, 1707, 1775, 1843, 1911, 1980, 2048, 2116, 2185, 2253, 2321, 2389,
  2458, 2526, 2594, 2662, 2731, 2799, 2867, 2935, 3004, 3072, 3140, 3209,
  3277, 3345, 3413, 3482, 3550, 3618, 3686, 3755, 3823, 3891, 3959, 4028, 4095
};
int p3 = 0, p5 = 0, p6 = 0;

void setup() {
  Serial.begin(115200);

  pinMode(LDAC, OUTPUT);  //DAC trans
  pinMode(SS, OUTPUT);    //DAC trans
  pinMode(4, OUTPUT);     //CLK_OUT
  pinMode(5, OUTPUT);     //GATE_OUT

  MIDI.begin(1);  // MIDI CH1をlisten

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);            // bit order
  SPI.setClockDivider(SPI_CLOCK_DIV4);  // クロック(CLK)をシステムクロックの1/4で使用(16MHz/4)
  SPI.setDataMode(SPI_MODE0);           // クロック極性０(LOW)　クロック位相０
  delay(50);

  // FlexiTimer2::set(5, 1.0 / 100000, timer_count);  // 50usec/count
  // FlexiTimer2::start();
}

void loop() {
  // Serial.println(" ");
  // Serial.println("  ");

  //-----------------------------clock_rate setting----------------------------
  clock_rate = analogRead(1);  //read knob voltage

  if (clock_rate < 256) {
    clock_max = 24;  //slow
  } else if (clock_rate < 512 && clock_rate >= 256) {
    clock_max = 12;
  } else if (clock_rate < 768 && clock_rate >= 512) {
    clock_max = 6;
  } else if (clock_rate >= 768) {
    clock_max = 3;  //fast
  }

  //-----------------------------gate ratch----------------------------
  if (note_on_count != 0) {
    Serial.println("1111111111111");

    if ((millis() - trigTimer <= 20) && (millis() - trigTimer > 10)) {
      digitalWrite(5, LOW);
    }
    if ((trigTimer > 0) && (millis() - trigTimer > 20)) {
      digitalWrite(5, HIGH);
    }
  }

  //-----------------------------midi operation----------------------------
  if (MIDI.read()) {  // 如果频道1有信号的话
    MIDI.setInputChannel(1);
    switch (MIDI.getType()) {

      case midi::NoteOn:  //if NoteOn
        note_on_count++;
        trigTimer = millis();
        if (note_on_count == 1) {
          note_no = MIDI.getData1() - 21;  //note number
          int velocity = MIDI.getData1();
          if (note_no < 0) {
            note_no = 0;
          } else if (note_no >= 61) {
            note_no = 60;
          }

          digitalWrite(4, HIGH);                   //Gate》HIGH
          if (cc_mode == 0) OUT_PWM(5, velocity);  //3个cv映射输出力度cv
          OUT_CV(cv[note_no]);                     //V/OCT LSB for DAC》参照
        }
        if (note_on_count == 2) {
          note_no = MIDI.getData1() - 21;  //note number
          int velocity = MIDI.getData1();
          if (note_no < 0) {
            note_no = 0;
          } else if (note_no >= 61) {
            note_no = 60;
          }

          digitalWrite(7, HIGH);                   //Gate》HIGH
          if (cc_mode == 0) OUT_PWM(5, velocity);  //3个cv映射输出力度cv
          OUT_CV2(cv[note_no]);                    //V/OCT LSB for DAC》参照
        }


        break;


      case midi::NoteOff:  //if NoteOff 关闭后

        note_on_count--;
        if (note_on_count == 0) {
          digitalWrite(4, LOW);  //Gate 》LOW
        }
        if (note_on_count == 1) {
          digitalWrite(7, LOW);  //Gate 》LOW
        }
        break;

      case midi::AfterTouchPoly:
        if (cc_mode == 0) OUT_PWM(6, MIDI.getData2() << 3);  //3个cv映射输出力度cv
        break;

      case midi::ControlChange:
        switch (MIDI.getData1()) {  //D3 D5 D6
          case 11:
          case 21:
          case 31:
          case 41:
          case 51:                           //change cc maping in modular
            cc_mode = MIDI.getData2() >> 5;  //一共四种模式
            break;
          case 1:  //modulation
            if (cc_mode == 0) OUT_PWM(3, MIDI.getData2() << 3);
            break;
          case 12:
            if (cc_mode == 1) OUT_PWM(3, MIDI.getData2() << 3);
            break;
          case 13:
            if (cc_mode == 1) OUT_PWM(5, MIDI.getData2() << 3);
            break;
          case 14:
            if (cc_mode == 1) OUT_PWM(6, MIDI.getData2() << 3);
            break;
          case 22:
            if (cc_mode == 2) OUT_PWM(3, MIDI.getData2() << 3);
            break;
          case 23:
            if (cc_mode == 2) OUT_PWM(5, MIDI.getData2() << 3);
            break;
          case 24:
            if (cc_mode == 2) OUT_PWM(6, MIDI.getData2() << 3);
            break;
          case 32:
            if (cc_mode == 3) OUT_PWM(3, MIDI.getData2() << 3);
            break;
          case 33:
            if (cc_mode == 3) OUT_PWM(5, MIDI.getData2() << 3);
            break;
          case 34:
            if (cc_mode == 3) OUT_PWM(6, MIDI.getData2() << 3);
            break;
        }



      case midi::Clock:
        clock_count++;

        if (clock_count >= clock_max) {
          clock_count = 0;
        }

        if (clock_count == 1) {
          digitalWrite(2, HIGH);
        } else if (clock_count != 1) {
          digitalWrite(2, LOW);
        }
        break;


      case midi::Stop:
        clock_count = 0;
        digitalWrite(4, LOW);  //Gate》LOW
        digitalWrite(7, LOW);  //Gate》LOW
        break;


      case midi::PitchBend:
        bend_lsb = MIDI.getData1();  //LSB
        bend_msb = MIDI.getData2();  //MSB
        bend_range = bend_msb;       //0 to 127

        if (bend_range > 64) {
          after_bend_pitch = cv[note_no] + cv[note_no] * (bend_range - 64) * 4 / 10000;
          OUT_CV(after_bend_pitch);
        }

        else if (bend_range < 64) {
          after_bend_pitch = cv[note_no] - cv[note_no] * (64 - bend_range) * 4 / 10000;
          OUT_CV(after_bend_pitch);
        }
        break;
    }
  }
  // OUT_PWM_GO(3, p3);
  // OUT_PWM_GO(5, p5);
  // OUT_PWM_GO(6, p6);
//  p6++;
 // if (p6 > 1) {
 //   p3++;
//    p6 = 0;
//  }
 // if (p3 > 255) p3 = 0;
  //OUT_PWM_GO(3, p3 / 1);
}


//DAC_CV output
void OUT_CV(int cv) {
  digitalWrite(LDAC, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer((cv >> 8) | 0x30);  // H0x30=OUTA/1x
  SPI.transfer(cv & 0xff);
  digitalWrite(SS, HIGH);
  digitalWrite(LDAC, LOW);
}

//DAC_CV2 output
void OUT_CV2(int cv2) {
  digitalWrite(LDAC, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer((cv2 >> 8) | 0xB0);  // H0xB0=OUTB/1x
  SPI.transfer(cv2 & 0xff);
  digitalWrite(SS, HIGH);
  digitalWrite(LDAC, LOW);
}

void OUT_PWM(int pin, int cc_value) {
  // analogWrite(pin, cc_value);
  switch (pin) {
    case 3:
      p3 = cc_value;
      break;
    case 5:
      break;
      p5 = cc_value;
    case 6:
      break;
      p6 = cc_value;
  }
}

void OUT_PWM_GO(int pin, int cc_value) {
  analogWrite(pin, cc_value);
}
