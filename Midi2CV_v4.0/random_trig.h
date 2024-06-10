#include <MsTimer2.h>

int trig_clk_in = 0;

void gate2Stop() {
  digitalWrite(GATE2_PIN, 0);
}

void triggerOn() {
  int randomNum = random(0, 255);
  //rand trig
  if (randomNum < 192)
    OUT_CV1(4095);
  if (randomNum < 128)
    digitalWrite(GATE1_PIN, 1);
  if (randomNum < 64)
    OUT_CV2(4095);
  //rand gate length
  digitalWrite(GATE2_PIN, 1);
  MsTimer2::set(random(0, 1024), gate2Stop);  // 0-1024 ms //此功能可能导致d3 cv1无效
  MsTimer2::start();
  //rabd level
  analogWrite(CV1_PIN, randomNum);         //8bit 0-256  2^8
  analogWrite(CV2_PIN, 255 - randomNum);   //inv
  analogWrite(CV3_PIN, (randomNum >> 1));  //7bit 0-128
}

void triggerOff() {
  OUT_CV1(0);
  digitalWrite(GATE1_PIN, 0);
  OUT_CV2(0);
  // digitalWrite(GATE2_PIN, 0);
}

void triggerListener() {
  // Serial.println(" triggerListener");
  pinMode(CLOCK_PIN, INPUT);  //CLK_OUT


  if (digitalRead(CLOCK_PIN) == 1 && trig_clk_in == 0) {
    trig_clk_in = 1;
    triggerOn();
  }
  if (digitalRead(CLOCK_PIN) == 0 && trig_clk_in == 1) {
    trig_clk_in = 0;
    triggerOff();
  }
}
