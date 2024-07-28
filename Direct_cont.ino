#include "Keyboard.h"
#include "math.h"

const int SW[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; //指令線の定義
const int SW_value[] = { -10, -10, -10, -10, -40, -1, 1, 2, 4, 1000, -1000 }; //指令線のON/OFFに値を持たせて演算
const int ledPin = 13;
int power_current_state_value = 0;  
int power_before_state_value = 0; //力行指令の値
int brake_current_state_value = 0;  
int brake_before_state_value = 0; //制動指令の値
int reverser_current_state_value = 0;  
int reverser_before_state_value = 0; //レバーサ指令の値
int power_notch = 0;
int brake_notch = 0;
int i,j,k=0;

void setup() {
  //直列段指令
  pinMode(SW[0], INPUT);
  pinMode(SW[1], INPUT);
  pinMode(SW[2], INPUT);
  pinMode(SW[3], INPUT);
  //並列段指令
  pinMode(SW[4], INPUT);
  //多段式中継弁
  pinMode(SW[5], INPUT);
  pinMode(SW[6], INPUT);
  pinMode(SW[7], INPUT);
  pinMode(SW[8], INPUT);
  //レバーサ
  pinMode(SW[9], INPUT);
  pinMode(SW[10], INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
  Keyboard.begin();
}

void loop() {
  power_current_state_value = 0;
  brake_current_state_value = 0;
  reverser_current_state_value = 0; //変数を初期化
  if (k==0){
    delay(1000);//シリアル通信を確実に拾えるようにするため初回のみ1秒の時素を入れる
  } else {
    delay(100); //チャタリング防止
  }
  k=1;
  for (int i = 0; i < 5; i++) {
    //現在の力行の状態を読み取り
    if (digitalRead(SW[i]) == HIGH) {
      power_current_state_value = power_current_state_value + SW_value[i];
    }
  }
  for (int i = 5; i < 9; i++) {
    //現在の制動の状態を読み取り
    if (digitalRead(SW[i]) == HIGH) {
      brake_current_state_value = brake_current_state_value + SW_value[i];
    }
  }
  for (int i = 9; i <11; i++) {
    //現在のレバーサの状態を読み取り
    if (digitalRead(SW[i]) == HIGH) {
      reverser_current_state_value = reverser_current_state_value + SW_value[i];
    }
  }
  //力行指令
  if (power_current_state_value < power_before_state_value ) {
    power_notch = -(power_current_state_value - power_before_state_value)/10;
    for (int j = 0; j < power_notch; j++) {
      Keyboard.press('z');
      Keyboard.release('z'); //上位段に進断
    }
  } else if (power_current_state_value > power_before_state_value ) {
    power_notch = (power_current_state_value - power_before_state_value)/10;
    for (int j = 0; j < power_notch; j++) {
      Keyboard.press('a');
      Keyboard.release('a'); //下位段に戻す
    }
  }
    //常用制動指令
  if ((brake_current_state_value - brake_before_state_value) > 0) {
    brake_notch = brake_current_state_value - brake_before_state_value;
    for (int j = 0; j < brake_notch; j++) {
      Keyboard.press(0X2E);
      Keyboard.release(0X2E); //込め
    }
  } else if ((brake_current_state_value - brake_before_state_value) < 0) {
    brake_notch = -(brake_current_state_value - brake_before_state_value);
    for (int j = 0; j < brake_notch; j++) {
      Keyboard.press(0X2C);
      Keyboard.release(0X2C); //弛め
    }
  }
    //前後進指令
  if (reverser_current_state_value == reverser_before_state_value + 1000) {
    for (int j = 0; j < 1; j++) {
      Keyboard.press(0XDA);
      Keyboard.release(0XDA); //前進
    }
  } else if (reverser_current_state_value == reverser_before_state_value - 1000) {
    for (int j = 0; j < 1; j++) {
      Keyboard.press(0XD9);
      Keyboard.release(0XD9); //後進
    }
  } 
  power_before_state_value = power_current_state_value;
  brake_before_state_value = brake_current_state_value;
  reverser_before_state_value = reverser_current_state_value;
  Serial.println(power_current_state_value);
  Serial.println(brake_current_state_value);
  Serial.println(reverser_current_state_value);
}