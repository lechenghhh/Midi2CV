#include <MIDI.h>
// #include <FlexiTimer2.h>
#include <SPI.h>                 //DAC通信用
MIDI_CREATE_DEFAULT_INSTANCE();  //MIDIライブラリを有効启用MIDI库

//2 4 7 gate
//3 5 6 CV
const int LDAC = 9;  //SPI trans setting
int poly_on = 0;     //noteNo=21(A0)～60(A5) total 61,マイナスの値を取るのでint 因为取负值，所以int

int bend_range = 0;
int bend_msb = 0;
int bend_lsb = 0;
long after_bend_pitch = 0;

byte poly_on_count = 0;       //当多个音符打开且其中一个音符关闭时，最后一个音符不消失。
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
int note4[4] = { 0, 0, 0, 0 };

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

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);            // bit order
  SPI.setClockDivider(SPI_CLOCK_DIV4);  // クロック(CLK)をシステムクロックの1/4で使用(16MHz/4)
  SPI.setDataMode(SPI_MODE0);           // クロック極性０(LOW)　クロック位相０

  digitalWrite(2, HIGH);
  delay(3000);
  digitalWrite(2, LOW);  //开机启动 三秒led显示

  // FlexiTimer2::set(5, 1.0 / 100000, timer_count);  // 50usec/count
  // FlexiTimer2::start();

  // TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  // TCCR2B = _BV(CS22);
}

void loop() {
  // Serial.println(" ");
  //-----------------------------clock_rate setting----------------------------
  // clock_rate = 1;  //read knob voltage

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
  if (poly_on_count != 0) {
    // Serial.println("1");

    int TRIG_DEC = 20;
    // if ((millis() - trigTimer <= TRIG_DEC) && (millis() - trigTimer > 10)) {
    //   digitalWrite(2, LOW);
    // }
    // if ((trigTimer > 0) && (millis() - trigTimer > TRIG_DEC)) {
    //   digitalWrite(2, HIGH);
    // }
  }

  //-----------------------------midi operation----------------------------
  if (MIDI.read()) {  // 如果频道1有信号的话
    MIDI.setInputChannel(1);
    switch (MIDI.getType()) {

      case midi::NoteOn:  //if NoteOn
        poly_on_count++;
        // trigTimer = millis();
        if (poly_on_count == 1) {
          // if (poly_on_count % 2 == 1) {
          poly_on = MIDI.getData1() - 21;  //note number
          int velocity = MIDI.getData2();
          if (poly_on < 0) {
            poly_on = 0;
          } else if (poly_on >= 61) {
            poly_on = 60;
          }

          digitalWrite(4, HIGH);                   //Gate》HIGH
          if (cc_mode == 0) OUT_PWM(5, velocity);  //3个cv映射输出力度cv


          OUT_CV(cv[poly_on]);  //V/OCT LSB for DAC》参照
        }
        if (poly_on_count == 2) {
          // if (poly_on_count % 2 == 0) {
          poly_on = MIDI.getData1() - 21;  //note number
          int velocity = MIDI.getData2();
          if (poly_on < 0) {
            poly_on = 0;
          } else if (poly_on >= 61) {
            poly_on = 60;
          }
          digitalWrite(7, HIGH);                   //Gate》HIGH
          if (cc_mode == 0) OUT_PWM(5, velocity);  //3个cv映射输出力度cv
          OUT_CV2(cv[poly_on]);                    //V/OCT LSB for DAC》参照
        }

        break;

      case midi::NoteOff:  //if NoteOff 关闭后

        poly_on_count--;
        if (poly_on_count == 0) {
          // if (poly_on_count % 2 == 0) {
          digitalWrite(4, LOW);  //Gate 》LOW
        }
        if (poly_on_count == 1) {
          // if (poly_on_count % 2 == 1) {
          digitalWrite(7, LOW);  //Gate 》LOW
        }
        break;

      case midi::AfterTouchPoly:
        if (cc_mode == 0) OUT_PWM(6, MIDI.getData2());  //3个cv映射输出力度cv
        break;

      case midi::ControlChange:
        switch (MIDI.getData1()) {  //D3 D5 D6
          case 11:
          case 21:
          case 31:
            clock_rate = MIDI.getData2() >> 5;
            break;
          case 12:
          case 22:
          case 32:
            cc_mode = MIDI.getData2() >> 5;  //一共四种模式 //change cc maping in modular
            break;
        }

        switch (cc_mode) {
          case 0:
            if (MIDI.getData1() == 1) OUT_PWM(3, MIDI.getData2());
            break;
          case 1:
            if (MIDI.getData1() == 13) OUT_PWM(3, MIDI.getData2());
            if (MIDI.getData1() == 14) OUT_PWM(5, MIDI.getData2());
            if (MIDI.getData1() == 15) OUT_PWM(6, MIDI.getData2());
            break;
          case 2:
            if (MIDI.getData1() == 23) OUT_PWM(3, MIDI.getData2());
            if (MIDI.getData1() == 24) OUT_PWM(5, MIDI.getData2());
            if (MIDI.getData1() == 25) OUT_PWM(6, MIDI.getData2());
            break;
          case 3:  //音符触发模式
            if (MIDI.getData1() == 33) OUT_PWM(3, MIDI.getData2());
            if (MIDI.getData1() == 34) OUT_PWM(5, MIDI.getData2());
            if (MIDI.getData1() == 35) OUT_PWM(6, MIDI.getData2());
            break;
        }
        break;
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
          after_bend_pitch = cv[poly_on] + cv[poly_on] * (bend_range - 64) * 4 / 10000;
          OUT_CV(after_bend_pitch);
        }

        else if (bend_range < 64) {
          after_bend_pitch = cv[poly_on] - cv[poly_on] * (64 - bend_range) * 4 / 10000;
          OUT_CV(after_bend_pitch);
        }
        break;
    }
  }
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
  analogWrite(pin, cc_value << 1);
}
