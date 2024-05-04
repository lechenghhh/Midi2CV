#include <MIDI.h>
#include <SPI.h>  //DAC通信用

#define CONFIG1_PIN 8   //配置1
#define CONFIG2_PIN 12  //配置2
#define CLOCK_PIN 2     //CLK
#define GATE1_PIN 4     //Gate1
#define GATE2_PIN 7     //Gate2
#define CV1_PIN 3       //CV1
#define CV2_PIN 5       //CV2
#define CV3_PIN 6       //CV3
const byte LDAC = 9;    //SPI trans setting

byte ch1 = 1;
byte ch2 = 2;

byte bend_range = 0;
byte bend_msb = 0;
byte bend_lsb = 0;
int after_bend_pitch = 0;

int note_no1 = 0;  //noteNo=21(A0)～60(A5) total 61,マイナスの値を取るのでint 因为取负值，所以int
int note_no2 = 0;  //noteNo=21(A0)～60(A5) total 61,マイナスの値を取るのでint 因为取负值，所以int
int poly_on1 = 0;  //noteNo=21(A0)～60(A5) total 61,マイナスの値を取るのでint 因为取负值，所以int
int poly_on2 = 0;  //noteNo=21(A0)～60(A5) total 61,マイナスの値を取るのでint 因为取负值，所以int

byte note_on_count1 = 0;  //当多个音符打开且其中一个音符关闭时，最后一个音符不消失。
byte note_on_count2 = 0;  //当多个音符打开且其中一个音符关闭时，最后一个音符不消失。
byte poly_on_count = 0;   //当多个音符打开且其中一个音符关闭时，最后一个音符不消失。
byte tmp_last_note1 = -1;
byte tmp_last_note2 = -1;
// int p3 = 0, p5 = 0, p6 = 0;
// int note4[4] = { 0, 0, 0, 0 };

byte clock_count = 0;  //clock计数器
byte clock_max = 24;   //clock分辨率
int clock_rate = 0;    //Clock速率
int clock_div = 1;     //Clock div 特殊用途

float OCT_CONST = 68.25;  // V/OCT 常量
byte cc_mode = 0;         //用于更改当前cc映射模式

byte seq_pitch[64] = { 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24 };
byte seq_gate[64] = { 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63 };
byte seq_vel[64] = { 0 };
byte seq_bpm = 0;       //音序器内部速度 暂不可用
byte seq_position = 0;  //音序器下标
byte seq_length = 8;    //音序器长度
byte seq_page = 0;      //音序器页码
byte seq_loopmode = 0;  //0:1:倒序2/3:随机
byte seq_state = 1;     //0:播放 1/2:暂停 3停止
byte seq_select = 1;    //当前选中的音序1-16

MIDI_CREATE_DEFAULT_INSTANCE();  //启用MIDI库

void setup() {
  Serial.begin(31250);  //midi协议的波特率就是31.25k 所以这里也要使用否则乱码

  pinMode(LDAC, OUTPUT);               //DAC trans
  pinMode(SS, OUTPUT);                 //DAC trans
  pinMode(CLOCK_PIN, OUTPUT);          //CLK_OUT
  pinMode(GATE1_PIN, OUTPUT);          //CLK_OUT
  pinMode(GATE2_PIN, OUTPUT);          //CLK_OUT
  pinMode(CV1_PIN, OUTPUT);            //CV_OUT
  pinMode(CV2_PIN, OUTPUT);            //CV_OUT
  pinMode(CV3_PIN, OUTPUT);            //CV_OUT
  pinMode(CONFIG1_PIN, INPUT_PULLUP);  //config
  pinMode(CONFIG2_PIN, INPUT_PULLUP);  //config

  MIDI.begin(MIDI_CHANNEL_OMNI);  // MIDI CH ALL Listen

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);            // bit order
  SPI.setClockDivider(SPI_CLOCK_DIV4);  // クロック(CLK)をシステムクロックの1/4で使用(16MHz/4)
  SPI.setDataMode(SPI_MODE0);           // クロック極性０(LOW)　クロック位相０

  if (digitalRead(CONFIG1_PIN) == 0) cc_mode = 1;  //读取d8跳线帽 默认1 插上0 如果插上给则默认进入
  if (digitalRead(CONFIG2_PIN) == 0) cc_mode = 2;  //读取d12跳线帽 默认1 插上0 如果插上给则默认进入
  // cc_mode = 2;

  digitalWrite(CLOCK_PIN, HIGH);
  digitalWrite(GATE1_PIN, HIGH);
  digitalWrite(GATE2_PIN, HIGH);
  delay(3000);
  digitalWrite(CLOCK_PIN, LOW);  //开机启动 三秒led显示
  digitalWrite(GATE1_PIN, LOW);  //开机启动 三秒led显示
  digitalWrite(GATE2_PIN, LOW);  //开机启动 三秒led显示
}

void loop() {
  // Serial.println(" ");

  controlChange();  //midi operation
  firstChannel();
  secondChannel();
}

void controlChange() {
  if (MIDI.read()) {
    switch (MIDI.getType()) {
      case midi::Clock:
        if (clock_count == 0) {
          digitalWrite(2, HIGH);
          // sequencerNext();  //音序器执行下一步
        }
        if (clock_count != 0) {
          digitalWrite(2, LOW);
        }
        clock_count++;
        if (clock_count >= clock_max) {
          clock_count = 0;
        }
        break;
      case midi::Start:
        clock_count = 0;
        break;
      case midi::AfterTouchPoly:
        // if (cc_mode == 0) OUT_PWM(CV3_PIN, MIDI.getData2());  //3个cv映射输出力度cv
        break;
      case midi::PitchBend:
        bend_lsb = MIDI.getData1();  //LSB
        bend_msb = MIDI.getData2();  //MSB
        bend_range = bend_msb;       //0 to 127

        if (cc_mode != 1) {  //常规通道模式
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
        digitalWrite(GATE1_PIN, LOW);  //Gate》LOW
        note_on_count2 = 0;
        digitalWrite(GATE2_PIN, LOW);  //Gate》LOW
        poly_on_count = 0;
        break;
      case midi::Stop:
        note_on_count1 = 0;
        note_on_count2 = 0;
        tmp_last_note1 = -1;
        tmp_last_note2 = -1;
        poly_on_count = 0;

        clock_count = 0;
        digitalWrite(GATE1_PIN, LOW);  //Gate》LOW
        digitalWrite(GATE2_PIN, LOW);  //Gate》LOW
        break;
      case midi::ControlChange:
        switch (MIDI.getData1()) {
          case 10:  //切换四种模式 //change cc maping in modular
            cc_mode = MIDI.getData2() >> 5;
            if (cc_mode == 0) {  //1/2通道模式
              ch1 = 1;
              ch2 = 2;
            }
            if (cc_mode == 1) {  //3/4通道模式
              ch1 = 3;
              ch2 = 4;
              clock_div = 2;
            }
            if (cc_mode == 2) {  //Seq模式
              ch1 = 1;
              ch2 = 2;
            }
            if (cc_mode == 3) {  //复音模式
              ch1 = 1;
              ch2 = 2;
            }
            break;
          case 1:  //输出mod转化的CV
            OUT_PWM(CV3_PIN, MIDI.getData2());
            break;
          case 21:  //seq pitch
            seq_pitch[seq_select] = MIDI.getData2() >> 1;
            if (seq_pitch[seq_select] > 60) seq_pitch[seq_select] = 60;
            break;
          case 22:  //gate pitch
            seq_gate[seq_select] = MIDI.getData2() >> 1;
            break;
          case 23:  //vel pitch
            seq_vel[seq_select] = MIDI.getData2();
            break;
          case 24:  //切换时钟div //clock_rate setting
            clock_rate = MIDI.getData2() >> 5;
            clock_max = 24 * clock_div / clock_rate;
            break;
          case 25:  //调整seq length //length范围:1-16
            seq_length = (MIDI.getData2() >> 3) + 1;
            break;
          case 26:  //page 预留
            seq_page = MIDI.getData2() >> 8;
            break;
          case 27:  //调整loop mode //范围0-3
            seq_loopmode = MIDI.getData2() >> 5;
            break;
          case 28:  //调整播放状态 //范围0-3
            seq_state = MIDI.getData2() >> 5;
            break;
        }
        if (100 < MIDI.getData1() && MIDI.getData1() < 117) {
          seq_select = MIDI.getData1() - 101;
        }
        break;  //ControlChange
    }
  }
}

void sequencerNext() {       //音序器执行下一步
  if (cc_mode != 2) return;  //如果是音序器模式 则不监听第二个通道的midi音符

  int tmp_position = 0;    //播放模式下标计算
  switch (seq_loopmode) {  //当前播放模式下的播放顺序
    default:               //正序
      tmp_position = seq_position;
      break;
    case 1:  //倒序
      tmp_position = seq_length - seq_position - 1;
      break;
    case 2:  //随机1
      tmp_position = random(0, seq_length - 1);
      break;
    case 3:  //随机2 后一半随机播放
      tmp_position = random(seq_length / 2, seq_length - 1);
      break;
  }
  //gate随机功能判断
  byte tmp_gate = seq_gate[tmp_position] >> 3;  //得到0-7
  switch (tmp_gate) {
    default:  //1-6随机
      if (random(0, tmp_gate) > 3) digitalWrite(GATE2_PIN, HIGH);
      else digitalWrite(GATE2_PIN, LOW);
      break;
    case 0:  //低电平
      digitalWrite(GATE2_PIN, LOW);
      break;
    case 7:  //高电平
      digitalWrite(GATE2_PIN, HIGH);
      break;
  }
  // digitalWrite(GATE2_PIN, seq_gate[tmp_position] > 32 ? HIGH : LOW);  //GATE 大于32高电平 否则低电平
  OUT_CV2(OCT_CONST * seq_pitch[tmp_position]);  //V/OCT LSB for DAC》参照
  OUT_PWM(CV2_PIN, seq_vel[tmp_position]);       //VEL

  //播放状态控制
  if (seq_state == 0) {  // 播放
    seq_position++;
    if (seq_position >= seq_length) seq_position = 0;
  }
  if (seq_state == 3) {  // 停止
    seq_position = 0;
  }
  sequencerView(tmp_position);  //音序器视图
  //触发模式恢复触发
  digitalWrite(GATE2_PIN, LOW);  //TRIG 增加此行则表示触发
}

void sequencerView(int tmp_position) {  //音序器视图
  String view_str = "";
  view_str += " cc:" + String(cc_mode);
  view_str += " pos:" + String(tmp_position + 1);
  view_str += " rat:" + String(clock_rate);
  view_str += " len:" + String(seq_length);
  // view_str += " pag:" + String(seq_page);
  view_str += " mod:" + String(seq_loopmode);
  view_str += " ste:" + String(seq_state);
  view_str += " seq:";
  for (int i = 0; i < seq_length; i++) view_str += String(seq_pitch[i]) + " ";

  Serial.println(view_str);  //音序器视图发送
}

void firstChannel() {
  if (MIDI.getChannel() == ch1) {  //MIDI CH1
    switch (MIDI.getType()) {
      case midi::NoteOn:     //if NoteOn
        if (cc_mode != 1) {  //常规通道模式
          note_on_count1++;
          note_no1 = MIDI.getData1() - 24;  //note number
          tmp_last_note1 = MIDI.getData1();
          if (note_no1 < 0) {
            note_no1 = 0;
          } else if (note_no1 >= 61) {
            note_no1 = 60;
          }
          Serial.println("note_no1 ");
          Serial.println( MIDI.getData1());
          Serial.println(note_no1);

          // OUT_CV1(V_OCT[note_no1]);
          OUT_CV1(OCT_CONST * note_no1);                            //V/OCT LSB for DAC》参照
          /*if (cc_mode == 0)*/ OUT_PWM(CV1_PIN, MIDI.getData2());  //3个cv映射输出力度cv
          digitalWrite(GATE1_PIN, HIGH);                            //Gate》HIGH
        } else {                                                    //复音模式
          poly_on_count++;
          if (poly_on_count == 1) {
            // if (poly_on_count % 2 == 1) {
            poly_on1 = MIDI.getData1() - 24;  //note number
            int velocity = MIDI.getData2();
            if (poly_on1 < 0) {
              poly_on1 = 0;
            } else if (poly_on1 >= 61) {
              poly_on1 = 60;
            }
            OUT_CV1(OCT_CONST * poly_on1);                 //V/OCT LSB for DAC》参照
            if (cc_mode != 3) OUT_PWM(CV2_PIN, velocity);  //3个cv映射输出力度cv
            digitalWrite(GATE1_PIN, HIGH);                 //Gate》HIGH
          }
          if (poly_on_count == 2) {
            // if (poly_on_count % 2 == 0) {
            poly_on2 = MIDI.getData1() - 24;  //note number
            int velocity = MIDI.getData2();
            if (poly_on2 < 0) {
              poly_on2 = 0;
            } else if (poly_on2 >= 61) {
              poly_on2 = 60;
            }
            OUT_CV2(OCT_CONST * poly_on2);           //V/OCT LSB for DAC》参照
            if (cc_mode == 0) OUT_PWM(5, velocity);  //3个cv映射输出力度cv
            digitalWrite(GATE2_PIN, HIGH);           //Gate》HIGH
          }
        }
        break;
      case midi::NoteOff:
        if (cc_mode != 1) {  //常规通道模式
          if (tmp_last_note1 == MIDI.getData1())
            digitalWrite(GATE1_PIN, LOW);  //Gate 》LOW
        } else {                           //双复音模式
          poly_on_count--;
          if (poly_on_count == 0) {
            // if (poly_on_count % 2 == 0) {
            digitalWrite(GATE1_PIN, LOW);  //Gate 》LOW
          }
          if (poly_on_count == 1) {
            // if (poly_on_count % 2 == 1) {
            digitalWrite(GATE2_PIN, LOW);  //Gate 》LOW
          }
        }
        break;
    }
  }
}

void secondChannel() {
  if (cc_mode == 2) return;        //如果是音序器模式 则不监听第二个通道的midi音符
  if (MIDI.getChannel() == ch2) {  //MIDI CH2
    switch (MIDI.getType()) {
      case midi::NoteOn:     //if NoteOn
        if (cc_mode != 1) {  //常规通道模式
          note_on_count2++;
          note_no2 = MIDI.getData1() - 24;  //note number
          tmp_last_note2 = MIDI.getData1();
          if (note_no2 < 0) {
            note_no2 = 0;
          } else if (note_no2 >= 61) {
            note_no2 = 60;
          }
          // OUT_CV2(V_OCT[note_no2]);  //V/OCT LSB for DAC》参照
          OUT_CV2(OCT_CONST * note_no2);                        //V/OCT LSB for DAC》参照
          if (cc_mode == 0) OUT_PWM(CV2_PIN, MIDI.getData2());  //3个cv映射输出力度cv
          digitalWrite(GATE2_PIN, HIGH);                        //Gate》HIGH
        }
        break;
      case midi::NoteOff:  //if NoteOff 关闭后
        // if (note_on_count2 > 0) note_on_count2--;
        // if (note_on_count2 < 1) {
        if (tmp_last_note2 == MIDI.getData1())
          digitalWrite(GATE2_PIN, LOW);  //Gate 》LOW
        // }
        break;

    }  //MIDI CH2
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

void OUT_PWM(int pin, int cc_value) {  //pwm 0-255
  analogWrite(pin, cc_value << 1);
}
