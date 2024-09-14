unsigned long mytimer_start_time = 0;       // 用于记录事件开始时间
unsigned long mytimer_delay_time = 100000;  // 用于记录事件开始时间

void myTimerStart(unsigned long time) {
  mytimer_delay_time = time;
}

void myTimerLoop() {
  if (millis() - mytimer_start_time >= mytimer_delay_time) {  //事件持续10秒钟或以上
    digitalWrite(8, 0);                                       //在这里编写你的代码来响应这个事件
  }
}

void myTimerReset() {
  mytimer_start_time = millis();  // 重置开始时间
}

void triggerOn() {
  int randomNum = random(0, 255);

  //rand trig
  if (randomNum < 223) {
    digitalWrite(2, 1);
    digitalWrite(2, 0);
  }
  if (randomNum < 192) {
    digitalWrite(3, 1);
  }
  if (randomNum < 160) {
    digitalWrite(4, 1);
  }
  if (randomNum < 128) {
    digitalWrite(5, 1);
  }
  if (randomNum < 96) {
    digitalWrite(6, 1);
  }
  if (randomNum < 32) {
    digitalWrite(7, 1);
  }

  //rand gate length
  digitalWrite(8, 1);
  myTimerStart(random(1, 32) * random(1, 32));
  myTimerReset();
}

void triggerOff() {
  digitalWrite(2, 0);
  digitalWrite(3, 0);
  digitalWrite(4, 0);
  digitalWrite(5, 0);
  digitalWrite(6, 0);
  digitalWrite(7, 0);
  // digitalWrite(GATE2_PIN, 0);
}

int trig_clk_in = 0;

void triggerListener() {  //In the loop
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

  myTimerLoop();
}
