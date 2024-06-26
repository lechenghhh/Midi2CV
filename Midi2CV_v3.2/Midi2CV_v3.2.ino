#include <MIDI.h>
#include <SPI.h>  //DAC通信用
// #include <FlexiTimer2.h>

#define CONFIG1_PIN 8   //配置1
#define CONFIG2_PIN 12  //配置2
#define CLOCK_PIN 2     //CLK
#define GATA1_PIN 4     //Gate
#define GATA2_PIN 7     //Gate2
#define CV1_PIN 3       //CV1
#define CV2_PIN 5       //CV2
#define CV3_PIN 6       //CV3

MIDI_CREATE_DEFAULT_INSTANCE();  //启用MIDI库

byte ch1 = 1;
byte ch2 = 2;

const byte LDAC = 9;  //SPI trans setting
byte bend_range = 0;
byte bend_msb = 0;
byte bend_lsb = 0;
int after_bend_pitch = 0;
byte note_no1 = 0;        //noteNo=21(A0)～60(A5) total 61,マイナスの値を取るのでint 因为取负值，所以int
byte note_no2 = 0;        //noteNo=21(A0)～60(A5) total 61,マイナスの値を取るのでint 因为取负值，所以int
byte poly_on1 = 0;        //noteNo=21(A0)～60(A5) total 61,マイナスの値を取るのでint 因为取负值，所以int
byte poly_on2 = 0;        //noteNo=21(A0)～60(A5) total 61,マイナスの値を取るのでint 因为取负值，所以int
byte note_on_count1 = 0;  //当多个音符打开且其中一个音符关闭时，最后一个音符不消失。
byte note_on_count2 = 0;  //当多个音符打开且其中一个音符关闭时，最后一个音符不消失。
byte poly_on_count = 0;   //当多个音符打开且其中一个音符关闭时，最后一个音符不消失。
byte clock_count0 = 0;
byte clock_max = 24;  //clock_max change by knob setting
int clock_rate = 0;   //knob CVin
int clock_div = 1;    //knob CVin
byte tmp_last_note1 = -1, tmp_last_note2 = -1;
byte cc_mode = 0;  //用于更改当前cc映射模式
byte Master = 1;
float OCT_CONST = 68.25;  // V/OCT 常量
// const int V_OCT[61] = {   // V/OCT LSB for DAC
//   0, 68, 137, 205, 273, 341, 410, 478, 546, 614, 683, 751,
//   819, 887, 956, 1024, 1092, 1161, 1229, 1297, 1365, 1434, 1502, 1570,
//   1638, 1707, 1775, 1843, 1911, 1980, 2048, 2116, 2185, 2253, 2321, 2389,
//   2458, 2526, 2594, 2662, 2731, 2799, 2867, 2935, 3004, 3072, 3140, 3209,
//   3277, 3345, 3413, 3482, 3550, 3618, 3686, 3755, 3823, 3891, 3959, 4028,
//   4095
// };
// int p3 = 0, p5 = 0, p6 = 0;
// int note4[4] = { 0, 0, 0, 0 };

void setup() {
  Serial.begin(31250);

  pinMode(LDAC, OUTPUT);       //DAC trans
  pinMode(SS, OUTPUT);         //DAC trans
  pinMode(CLOCK_PIN, OUTPUT);  //CLK_OUT
  pinMode(GATA1_PIN, OUTPUT);  //CLK_OUT
  pinMode(GATA2_PIN, OUTPUT);  //CLK_OUT
  pinMode(CV1_PIN, OUTPUT);
  pinMode(CV2_PIN, OUTPUT);
  pinMode(CV3_PIN, OUTPUT);
  pinMode(CONFIG1_PIN, INPUT_PULLUP);
  pinMode(CONFIG2_PIN, INPUT_PULLUP);

  MIDI.begin(MIDI_CHANNEL_OMNI);  // MIDI CH ALL Listen

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);            // bit order
  SPI.setClockDivider(SPI_CLOCK_DIV4);  // クロック(CLK)をシステムクロックの1/4で使用(16MHz/4)
  SPI.setDataMode(SPI_MODE0);           // クロック極性０(LOW)　クロック位相０

  Master = digitalRead(CONFIG1_PIN);  //读取d8跳线帽 默认1 插上0
  if (Master == 1) {                  //频道选择
    ch1 = 1;
    ch2 = 2;
  }
  if (Master == 0) {
    ch1 = 3;
    ch2 = 4;
    clock_div = 2;
  }

  digitalWrite(CLOCK_PIN, HIGH);
  digitalWrite(GATA1_PIN, HIGH);
  digitalWrite(GATA2_PIN, HIGH);
  delay(3000);
  digitalWrite(CLOCK_PIN, LOW);  //开机启动 三秒led显示
  digitalWrite(GATA1_PIN, LOW);  //开机启动 三秒led显示
  digitalWrite(GATA2_PIN, LOW);  //开机启动 三秒led显示

  // FlexiTimer2::set(5, 1.0 / 100000, timer_count);  // 50usec/count
  // FlexiTimer2::start();

  // TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  // TCCR2B = _BV(CS22);
}

void loop() {
  // Serial.println(" ");
  //-----------------------------clock_rate setting----------------------------

  if (clock_rate < 256) {
    clock_max = 24 * clock_div;  //slow
  } else if (clock_rate < 512 && clock_rate >= 256) {
    clock_max = 12 * clock_div;
  } else if (clock_rate < 768 && clock_rate >= 512) {
    clock_max = 6 * clock_div;
  } else if (clock_rate >= 768) {
    clock_max = 3 * clock_div;  //fast
  }

  //-----------------------------gate ratch----------------------------
  if (note_on_count1 != 0) {
    // Serial.println("1");
    // int TRIG_DEC = 20;
  }

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
        break;
      case midi::AfterTouchPoly:
        // if (cc_mode == 0) OUT_PWM(CV3_PIN, MIDI.getData2());  //3个cv映射输出力度cv
        break;
      case midi::PitchBend:
        bend_lsb = MIDI.getData1();  //LSB
        bend_msb = MIDI.getData2();  //MSB
        bend_range = bend_msb;       //0 to 127

        if (cc_mode != 3) {  //双通道模式
          if (bend_range > 64) {
            after_bend_pitch = OCT_CONST * note_no1 + OCT_CONST * note_no1 * (bend_range - 64) * 4 / 10000;
            OUT_CV1(after_bend_pitch);
            after_bend_pitch = OCT_CONST * note_no1 + OCT_CONST * note_no2 * (bend_range - 64) * 4 / 10000;
            OUT_CV2(after_bend_pitch);
          } else if (bend_range < 64) {
            after_bend_pitch = OCT_CONST * note_no1 - OCT_CONST * note_no1 * (64 - bend_range) * 4 / 10000;
            OUT_CV1(after_bend_pitch);
            after_bend_pitch = OCT_CONST * note_no1 - OCT_CONST * note_no2 * (64 - bend_range) * 4 / 10000;
            OUT_CV2(after_bend_pitch);
          }
        } else {
          if (bend_range > 64) {
            after_bend_pitch = OCT_CONST * poly_on1 + OCT_CONST * poly_on1 * (bend_range - 64) * 4 / 10000;
            OUT_CV1(after_bend_pitch);
            after_bend_pitch = OCT_CONST * poly_on1 + OCT_CONST * poly_on2 * (bend_range - 64) * 4 / 10000;
            OUT_CV2(after_bend_pitch);
          } else if (bend_range < 64) {
            after_bend_pitch = OCT_CONST * poly_on1 - OCT_CONST * poly_on1 * (64 - bend_range) * 4 / 10000;
            OUT_CV1(after_bend_pitch);
            after_bend_pitch = OCT_CONST * poly_on1 - OCT_CONST * poly_on2 * (64 - bend_range) * 4 / 10000;
            OUT_CV2(after_bend_pitch);
          }
        }
        break;
      case midi::AllNotesOff:
        note_on_count1 = 0;
        note_on_count2 = 0;
        digitalWrite(4, LOW);  //Gate》LOW
        digitalWrite(7, LOW);  //Gate》LOW
        poly_on_count = 0;
        break;
      case midi::Stop:
        note_on_count1 = 0;
        note_on_count2 = 0;
        tmp_last_note1 = -1;
        tmp_last_note2 = -1;
        poly_on_count = 0;

        clock_count0 = 0;
        digitalWrite(4, LOW);  //Gate》LOW
        digitalWrite(7, LOW);  //Gate》LOW
        break;
      case midi::ControlChange:
        switch (MIDI.getData1()) {  //D3 D5 D6
          case 11:                  //切换时钟DIV
          case 21:
          case 31:
            clock_rate = MIDI.getData2() << 3;
            break;
          case 12:  //切换四种模式 //change cc maping in modular
          case 22:
          case 32:
            cc_mode = MIDI.getData2() >> 5;
            break;
        }

        switch (cc_mode) {
          case 0:  //type=0 VEL/MOD/CC2 VEL请见Note On
            if (MIDI.getData1() == 1) OUT_PWM(CV3_PIN, MIDI.getData2());
            break;
          case 1:  //type=1 CC13/CC14/CC15
            if (MIDI.getData1() == 13) OUT_PWM(CV1_PIN, MIDI.getData2());
            if (MIDI.getData1() == 14) OUT_PWM(CV2_PIN, MIDI.getData2());
            if (MIDI.getData1() == 15) OUT_PWM(CV3_PIN, MIDI.getData2());
            break;
          case 2:  //type=2 CC23/CC24/CC25
            if (MIDI.getData1() == 23) OUT_PWM(CV1_PIN, MIDI.getData2());
            if (MIDI.getData1() == 24) OUT_PWM(CV2_PIN, MIDI.getData2());
            if (MIDI.getData1() == 25) OUT_PWM(CV3_PIN, MIDI.getData2());
            break;
          case 3:                  //type=3 复音模式
            digitalWrite(4, LOW);  //Gate 》LOW
            digitalWrite(7, LOW);  //Gate 》LOW

            if (MIDI.getData1() == 33) OUT_PWM(CV1_PIN, MIDI.getData2());
            if (MIDI.getData1() == 34) OUT_PWM(CV2_PIN, MIDI.getData2());
            if (MIDI.getData1() == 35) OUT_PWM(CV3_PIN, MIDI.getData2());
            break;
        }
        break;  //ControlChange
    }

    if (MIDI.getChannel() == ch1) {  //MIDI CH1
      switch (MIDI.getType()) {
        case midi::NoteOn:     //if NoteOn
          if (cc_mode != 3) {  //双通道模式
            note_on_count1++;
            note_no1 = MIDI.getData1() - 21;  //note number
            tmp_last_note1 = MIDI.getData1();
            if (note_no1 < 0) {
              note_no1 = 0;
            } else if (note_no1 >= 61) {
              note_no1 = 60;
            }
            // OUT_CV1(V_OCT[note_no1]);
            OUT_CV1(OCT_CONST * note_no1);                        //V/OCT LSB for DAC》参照
            if (cc_mode == 0) OUT_PWM(CV1_PIN, MIDI.getData2());  //3个cv映射输出力度cv
            digitalWrite(4, HIGH);                                //Gate》HIGH
          } else {                                                //复音模式
            poly_on_count++;
            if (poly_on_count == 1) {
              // if (poly_on_count % 2 == 1) {
              poly_on1 = MIDI.getData1() - 21;  //note number
              int velocity = MIDI.getData2();
              if (poly_on1 < 0) {
                poly_on1 = 0;
              } else if (poly_on1 >= 61) {
                poly_on1 = 60;
              }
              OUT_CV1(OCT_CONST * poly_on1);           //V/OCT LSB for DAC》参照
              if (cc_mode == 0) OUT_PWM(5, velocity);  //3个cv映射输出力度cv
              digitalWrite(4, HIGH);                   //Gate》HIGH
            }
            if (poly_on_count == 2) {
              // if (poly_on_count % 2 == 0) {
              poly_on2 = MIDI.getData1() - 21;  //note number
              int velocity = MIDI.getData2();
              if (poly_on2 < 0) {
                poly_on2 = 0;
              } else if (poly_on2 >= 61) {
                poly_on2 = 60;
              }
              OUT_CV2(OCT_CONST * poly_on2);           //V/OCT LSB for DAC》参照
              if (cc_mode == 0) OUT_PWM(5, velocity);  //3个cv映射输出力度cv
              digitalWrite(7, HIGH);                   //Gate》HIGH
            }
          }
          break;
        case midi::NoteOff:
          if (cc_mode != 3) {  //双通道模式
            if (tmp_last_note1 == MIDI.getData1())
              digitalWrite(4, LOW);  //Gate 》LOW
          } else {                   //双复音模式
            poly_on_count--;
            if (poly_on_count == 0) {
              // if (poly_on_count % 2 == 0) {
              digitalWrite(4, LOW);  //Gate 》LOW
            }
            if (poly_on_count == 1) {
              // if (poly_on_count % 2 == 1) {
              digitalWrite(7, LOW);  //Gate 》LOW
            }
          }
          break;
      }
    }

    if (MIDI.getChannel() == ch2) {  //MIDI CH2
      switch (MIDI.getType()) {
        case midi::NoteOn:     //if NoteOn
          if (cc_mode != 3) {  //双通道模式
            note_on_count2++;
            note_no2 = MIDI.getData1() - 21;  //note number
            tmp_last_note2 = MIDI.getData1();
            if (note_no2 < 0) {
              note_no2 = 0;
            } else if (note_no2 >= 61) {
              note_no2 = 60;
            }
            // OUT_CV2(V_OCT[note_no2]);  //V/OCT LSB for DAC》参照
            OUT_CV2(OCT_CONST * note_no2);                        //V/OCT LSB for DAC》参照
            if (cc_mode == 0) OUT_PWM(CV2_PIN, MIDI.getData2());  //3个cv映射输出力度cv
            digitalWrite(7, HIGH);                                //Gate》HIGH
          }

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
  analogWrite(pin, cc_value << 1);
}
