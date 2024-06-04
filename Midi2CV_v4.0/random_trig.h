int trig_clk_in = 0;

void triggerOn() {
  int randomNum = random(0, 255);
  if (randomNum < 192)
    OUT_CV1(4095);
  if (randomNum < 128)
    digitalWrite(GATE1_PIN, 1);
  if (randomNum < 64)
    OUT_CV2(4095);
  if (randomNum < 32)
    digitalWrite(GATE2_PIN, 1);

  analogWrite(CV1_PIN, randomNum);         //8bit 0-256  2^8
  analogWrite(CV2_PIN, 255 - randomNum);   //inv
  analogWrite(CV3_PIN, (randomNum >> 1));  //7bit 0-128
}

void triggerOff() {
  OUT_CV1(0);
  digitalWrite(GATE1_PIN, 0);
  OUT_CV2(0);
  digitalWrite(GATE2_PIN, 0);
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
