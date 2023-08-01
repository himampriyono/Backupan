#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

Servo lf;
Servo gr;
LiquidCrystal_I2C lcd(0x27, 16, 2);

String text_1 = "TIM SECRET";
String text_2 = "UNY";

unsigned long int a, b, c;
int x[15], ch1[15], ch[7], i;

int power = 0;
int p, r, l, g;
int th = 20;
int ena1 = 12;
int mot2 = 14;
int mot1 = 27;
int mot4 = 26;
int mot3 = 25;
int ena2 = 33;

void setup() {
  Serial.begin(9600);
  loading();
  pinMode(13, INPUT_PULLUP);
  pinMode(mot1, OUTPUT);
  pinMode(mot2, OUTPUT);
  pinMode(mot3, OUTPUT);
  pinMode(mot4, OUTPUT);
  pinMode(ena1, OUTPUT);
  pinMode(ena2, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(13), read_me, FALLING);
  lf.attach(18);
  gr.attach(19);
}

void loop() {
  read_rc();
  power = (ch[3] - 512);
  r = (ch[1] - 512); //roll

  power = map(power, 0, 512, 0, 255);
  r = map(r, 0, 512, 0, 255);

  if (abs(r) <= th) {
    r = 0;
  }
  if (abs(power) <= th) {
    power = 0;
    setMotor(r, -r);
  }

  else if (r > th) {
    if (power > 0) {
      setMotor(power, power - r);
    }
    else {
      setMotor(power, power + r);
    }
  }

  else if (r < -th) {
    if (power > 0) {
      setMotor(power + r, power);
    }
    else {
      setMotor(power - r, power);
    }
  }

  else {
    setMotor(power, power);
  }

  g = ch[5];
  if (g > 512) {
    gr.write(160);
  }
  else {
    gr.write(30);
  }

  l = ch[6];
  if (l < 400) {
    lf.write(120);
  }
  else if (l > 605) {
    lf.write(30);
  }
  else if (410 < l < 600) {
    lf.write(103);
  }

  Serial.print(ch[1]); Serial.print("\t");
  Serial.print(ch[2]); Serial.print("\t");
  Serial.print(ch[3]); Serial.print("\t");
  Serial.print(ch[4]); Serial.print("\t");
  Serial.print(ch[5]); Serial.print("\t");
  Serial.print(ch[6]); Serial.print("\t");
  Serial.print(ch[7]); Serial.println("\t");
}

void read_me()  {
  a = micros();
  c = a - b;
  b = a;
  x[i] = c;
  i = i + 1;       if (i == 15) {
    for (int j = 0; j < 15; j++) {
      ch1[j] = x[j];
    }
    i = 0;
  }
}

void read_rc() {
  int i, j, k = 0;
  for (k = 14; k > -1; k--) {
    if (ch1[k] > 2100) {
      j = k;
    }
  }
  for (i = 1; i <= 6; i++) {
    ch[i] = (ch1[i + j] - 1000);
  }
}

void setMotor(int in1, int in2) {
  int o1, o2, o3, o4;
  if (in1 > 0) {
    o1 = HIGH;
    o2 = LOW;
  }
  else if (in1 < 0) {
    o1 = LOW;
    o2 = HIGH;
  }
  else {
    o1 = LOW;
    o2 = LOW;
  }

  if (in2 > 0) {
    o3 = HIGH;
    o4 = LOW;
  }
  else if (in2 < 0) {
    o3 = LOW;
    o4 = HIGH;
  }
  else {
    o3 = LOW;
    o4 = LOW;
  }

  in1 = abs(in1);
  in2 = abs(in2);

  digitalWrite(mot1, o1);
  digitalWrite(mot2, o2);
  digitalWrite(mot3, o3);
  digitalWrite(mot4, o4);
  analogWrite(ena1, in1);
  analogWrite(ena2, in2);

  //  Serial.print(o1); Serial.print("\t");
  //  Serial.print(o2); Serial.print("\t");
  //  Serial.print(o3); Serial.print("\t");
  //  Serial.print(o4); Serial.print("\t||\t");
  //  Serial.print(in1); Serial.print("\t");
  //  Serial.println(in2);
}

void loading() {
  lcd.begin();
  for (int i = 0; i < 8; i++) {
    lcd.setCursor(i, 0);
    lcd.write(255);
    lcd.setCursor(15 - i, 0);
    lcd.write(255);
    lcd.setCursor(i, 1);
    lcd.write(255);
    lcd.setCursor(15 - i, 1);
    lcd.write(255);
    delay(10);
  }
  delay(100);
  lcd.clear();
  delay(100);
  for (int i = 0; i < 3; i++) {
    lcd.setCursor(7, 1);
    lcd.print("__");
    lcd.setCursor(5, 0);
    lcd.print("-");
    lcd.setCursor(10, 0);
    lcd.print("-");
    delay(95);
    lcd.setCursor(5, 0);
    lcd.print("O");
    lcd.setCursor(10, 0);
    lcd.print("O");
    delay(400);
  }
  delay(350);
  lcd.setCursor(5, 0);
  lcd.print("^");
  lcd.setCursor(10, 0);
  lcd.print("^");
  delay(400);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print(text_1);
  lcd.setCursor(6, 1);
  lcd.print(text_2);
}
