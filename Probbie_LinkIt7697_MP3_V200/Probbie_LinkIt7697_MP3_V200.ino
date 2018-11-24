// 最後編輯 2018-8-29 by ShinWei Chiou
// 初版

// 最後編輯 2018-9-01 by ShinWei Chiou
// 修正手機 APP UI 更新率過高，導致連線異常失敗。

// 最後編輯 2018-9-02 by ShinWei Chiou
// 修正 IR 感測程式。

// 最後編輯 2018-9-03 by ShinWei Chiou
// 修正 UI 程式。
// 新增 MP3 程式。
// 新增 Tone 程式。

// 最後編輯 2018-9-04 by ShinWei Chiou
// 修正 LED 程式。
// 新增 IR 感測倍率程式。

// 最後編輯 2018-9-10 by ShinWei Chiou
// 修正 LED 程式。
// 增加 MP3 程式。

// 最後編輯 2018-9-15 by ShinWei Chiou
// 修正 AutoDemo 程式。

// 最後編輯 2018-9-18 by ShinWei Chiou
// 修正 Prepare AutoDemo 程式。

// 最後編輯 2018-11-09 by ShinWei Chiou
// 修正 PCBA Pin 腳位程式。
// 修正 analogRead(BatteryPin) * 0.00280)

// 最後編輯 2018-11-16 by ShinWei Chiou
// 增加 低壓 程式。

// 最後編輯 2018-11-21 by ShinWei Chiou
// 修正 analogRead(BatteryPin) * 0.00285)

// Pro's Kit Probbie
// https://science.prokits.com.tw/tw/Product/GE-893/

// LinkIt Remote
// https://docs.labs.mediatek.com/resource/linkit7697-arduino/en/developer-guide/using-linkit-remote

// BlocklyDuino
// https://docs.labs.mediatek.com/linkit-7697-blocklyduino/blocklyduino-12880262.html

// LinkIt 7697 - Development Guide for Arduino IDE
// https://docs.labs.mediatek.com/resource/linkit7697-arduino/en

// DFPlayer mini mp3 module
// https://github.com/DFRobot/DFPlayer-Mini-mp3


#include <LRemote.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <DFPlayer_Mini_Mp3.h>

int BLE_connect = 0;

// Motor
#define TurnMotor_A   13
#define TurnMotor_B   10
#define LegMotor_A    12
#define LegMotor_B    17

// DFplayer
#define DFPlayerBusyPin   4
const int VolumeMax = 30;
const int VolumeMin = 5;
int Volume_Value = 25;
// On LinkIt 7697, the RX pin must be one of the EINT pins.
// There are no limitations on TX pin.
SoftwareSerial mp3Serial(3, 2); // RX, TX

// NeoPixels
#define NeoPixelPin    5
#define NeoPixelnum    2
Adafruit_NeoPixel Pixels = Adafruit_NeoPixel(NeoPixelnum, NeoPixelPin, NEO_GRB + NEO_KHZ800);

// IR Proximity Sensor
#define irEmitterPin    11
#define irRightPin      14
#define irLeftPin       15
#define irRightDivide   5
#define irLeftDivide    4
int AutoDemo_OnOff = 0;
int irRightAmbient;
int irLeftAmbient;
int irRightObstacle;
int irLeftObstacle;
int irRightDistance;
int irLeftDistance;
int irRightValue[10];
int irLeftValue[10];
int ProximityValue = 0;
int ProximityLmaxValue = 0;
int ProximityRmaxValue = 0;
int ProximityLeftValue = 0;
int ProximityRightValue = 0;

// Battery
#define BatteryPin    16
const float BatteryLowVol = 4.8;
float BatteryVol = 0;
int BatteryCount = 0;
int BatteryLow = 0;

// IR Battle System
int Battle_OnOff = 0;
int IR_ADValue_L = 0;
int IR_ADValue_R = 0;
int IR_ADValue_Lavg = 0;
int IR_ADValue_Ravg = 0;

// Button
LRemoteButton ledRbutton;
LRemoteButton ledGbutton;
LRemoteButton ledBbutton;
LRemoteButton ledWbutton;
LRemoteButton buzzerbutton;
LRemoteButton forwardbutton;
LRemoteButton backwardbutton;
LRemoteButton turnleftbutton;
LRemoteButton turnrightbutton;

// Slider
LRemoteSlider setBrightnessslider;

// Switch
LRemoteSwitch autoswitch;
LRemoteSwitch battleswitch;
LRemoteSwitch music1switch;
LRemoteSwitch music2switch;

// Label
LRemoteLabel batterylabel;

// Buzzer
#define BuzzerPin     7
int Tone_counter = 0;
const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;


/*------------------------------------------------------------*/
// Beep Sound
void Beep(int note, int duration)
{
  // Play tone on buzzerPin
  tone(BuzzerPin, note, duration);

  // Play different depending on value of 'Tone_counter'
  if (Tone_counter % 2 == 0)
  {
    delay(duration);
  }
  else
  {
    delay(duration);
  }

  // Stop tone on buzzerPin
  noTone(BuzzerPin);

  // Buzzer
  pinMode(BuzzerPin, OUTPUT);

  delay(50);
}


/*------------------------------------------------------------*/
// Ray Gun Sound
void Ray_Gun_Sound()
{
  for (int i = 0; i < 300; i++)
  {
    digitalWrite(BuzzerPin, HIGH);
    delayMicroseconds(i);
    digitalWrite(BuzzerPin, LOW);
    delayMicroseconds(i);
  }
}


/*------------------------------------------------------------*/
// IR Send Code 38kHz
void IR_Send_Code()
{
  for (int i16t = 0; i16t < 16; i16t++)
  {
    for (int i37k = 0; i37k < 125; i37k++)
    {
      digitalWrite(irEmitterPin, HIGH);
      delayMicroseconds(12);
      digitalWrite(irEmitterPin, LOW);
      delayMicroseconds(12);
    }
    delay(2);
  }
}


/*------------------------------------------------------------*/
// Read Left IR Distance
int Read_Left_IR_Distance(int times)
{
  for (int x = 0; x < times; x++)
  {
    // turning the IR LEDs off to read the IR coming from the ambient
    digitalWrite(irEmitterPin, LOW);
    delay(1);

    // storing IR coming from the ambient
    irLeftAmbient = analogRead(irLeftPin);

    // turning the IR LEDs on to read the IR coming from the obstacle
    digitalWrite(irEmitterPin, HIGH);
    delay(1);

    // storing IR coming from the obstacle
    irLeftObstacle = analogRead(irLeftPin);

    // calculating changes in IR values and storing it for future average
    irLeftValue[x] = irLeftObstacle - irLeftAmbient;
  }

  // turning the IR LEDs off
  digitalWrite(irEmitterPin, LOW);

  // calculating the average based on the "accuracy"
  for (int x = 0; x < times; x++)
  {
    irLeftDistance += irLeftValue[x];
  }

  // return the final value
  irLeftDistance = irLeftDistance / times;
}


/*------------------------------------------------------------*/
// Read Right IR Distance
int Read_Right_IR_Distance(int times)
{
  for (int x = 0; x < times; x++)
  {
    // turning the IR LEDs off to read the IR coming from the ambient
    digitalWrite(irEmitterPin, LOW);
    delay(1);

    // storing IR coming from the ambient
    irRightAmbient = analogRead(irRightPin);

    // turning the IR LEDs on to read the IR coming from the obstacle
    digitalWrite(irEmitterPin, HIGH);
    delay(1);

    // storing IR coming from the obstacle
    irRightObstacle = analogRead(irRightPin);

    // calculating changes in IR values and storing it for future average
    irRightValue[x] = irRightObstacle - irRightAmbient;
  }

  // turning the IR LEDs off
  digitalWrite(irEmitterPin, LOW);

  // calculating the average based on the "accuracy"
  for (int x = 0; x < times; x++)
  {
    irRightDistance += irRightValue[x];
  }

  // return the final value
  irRightDistance = irRightDistance / times;
}


/*------------------------------------------------------------*/
void Move_Forward()
{
  if (BatteryLow == 0)
  {
    digitalWrite(LegMotor_A, HIGH);
    digitalWrite(LegMotor_B, LOW);
  }
  else
  {
    Beep(fH, 150);
    delay(100);
    Beep(fH, 150);
  }
}

void Move_Backward()
{
  if (BatteryLow == 0)
  {
    digitalWrite(LegMotor_A, LOW);
    digitalWrite(LegMotor_B, HIGH);
  }
  else
  {
    Beep(fH, 150);
    delay(100);
    Beep(fH, 150);
  }
}

void Move_TurnLeft()
{
  if (BatteryLow == 0)
  {
    digitalWrite(TurnMotor_A, HIGH);
    digitalWrite(TurnMotor_B, LOW);
  }
  else
  {
    Beep(fH, 150);
    delay(100);
    Beep(fH, 150);
  }
}

void Move_TurnRight()
{
  if (BatteryLow == 0)
  {
    digitalWrite(TurnMotor_A, LOW);
    digitalWrite(TurnMotor_B, HIGH);
  }
  else
  {
    Beep(fH, 150);
    delay(100);
    Beep(fH, 150);
  }
}

void Motor_Break()
{
  digitalWrite(LegMotor_A, HIGH);
  digitalWrite(LegMotor_B, HIGH);
  digitalWrite(TurnMotor_A, HIGH);
  digitalWrite(TurnMotor_B, HIGH);
  delay(100);
  digitalWrite(LegMotor_A, LOW);
  digitalWrite(LegMotor_B, LOW);
  digitalWrite(TurnMotor_A, LOW);
  digitalWrite(TurnMotor_B, LOW);
}

void Move_Fire()
{
  if (BatteryLow == 0)
  {
    digitalWrite(LegMotor_A, LOW);
    digitalWrite(LegMotor_B, HIGH);
    delay(200);
    digitalWrite(LegMotor_A, HIGH);
    digitalWrite(LegMotor_B, LOW);
    delay(100);
    digitalWrite(LegMotor_A, LOW);
    digitalWrite(LegMotor_B, LOW);
  }
  else
  {
    Beep(fH, 150);
    delay(100);
    Beep(fH, 150);
  }
}

void Move_Damage()
{
  if (BatteryLow == 0)
  {
    Move_TurnLeft();
    delay(200);
    Move_TurnRight();
    delay(400);
    Move_TurnLeft();
    delay(200);
    Motor_Break();
  }
  else
  {
    Beep(fH, 150);
    delay(100);
    Beep(fH, 150);
  }
}


/*------------------------------------------------------------*/
// Start
void Start_UP()
{
  for (int i = 0; i < 255; i++)
  {
    Pixels.setPixelColor(0, Pixels.Color(i, i, i));
    Pixels.setPixelColor(1, Pixels.Color(i, i, i));
    Pixels.show();

    digitalWrite(BuzzerPin, HIGH);
    delayMicroseconds(i);
    digitalWrite(BuzzerPin, LOW);
    delayMicroseconds(i);

    delay(7);
  }

  Pixels.setPixelColor(0, Pixels.Color(0, 0, 0));
  Pixels.setPixelColor(1, Pixels.Color(0, 0, 0));
  Pixels.show();
  delay(200);
  Pixels.setPixelColor(0, Pixels.Color(150, 150, 150));
  Pixels.setPixelColor(1, Pixels.Color(150, 150, 150));
  Pixels.show();
  Beep(fH, 200);

  Pixels.setPixelColor(0, Pixels.Color(0, 0, 0));
  Pixels.setPixelColor(1, Pixels.Color(0, 0, 0));
  Pixels.show();
  delay(200);
  Pixels.setPixelColor(0, Pixels.Color(150, 150, 150));
  Pixels.setPixelColor(1, Pixels.Color(150, 150, 150));
  Pixels.show();
  Beep(fH, 200);
}


/*------------------------------------------------------------*/
void setup() {

  // Initialize serial communications at 9600 bps:
  Serial.begin(9600);


  // Set the data rate for the SoftwareSerial port
  mp3Serial.begin(9600);

  // Add for DFplayer
  mp3_set_serial (mp3Serial);  // set Serial for DFPlayer-mini mp3 module
  mp3_set_volume (Volume_Value);
  delay (10);
  pinMode(DFPlayerBusyPin, INPUT);


  // Buzzer
  pinMode(BuzzerPin, OUTPUT);

  // Motor PIN Set
  pinMode(LegMotor_A, OUTPUT);
  pinMode(LegMotor_B, OUTPUT);
  pinMode(TurnMotor_A, OUTPUT);
  pinMode(TurnMotor_B, OUTPUT);

  // Infrared
  pinMode(irLeftPin, INPUT);
  pinMode(irRightPin, INPUT);
  pinMode(irEmitterPin, OUTPUT);
  digitalWrite(irEmitterPin, LOW);

  // NeoPixel
  Pixels.setBrightness(255);
  Pixels.begin();
  Pixels.show();


  // Start Up !
  Start_UP();


  // Initialize BLE subsystem & get BLE address
  LBLE.begin();
  while (!LBLE.ready()) {
    delay(100);
  }

  Serial.print("Device Address = [");
  LBLEAddress ble_address;
  String probbie_address;

  ble_address = LBLE.getDeviceAddress();
  probbie_address = ble_address.toString();
  Serial.print(probbie_address);
  Serial.println("]");

  String probbie_name;
  probbie_name = "AI" + probbie_address.substring(0, 2) + probbie_address.substring(3, 5);

  // Setup the Remote Control's Name
  LRemote.setName(probbie_name);


  // Setup the Remote Control's UI canvas
  LRemote.setOrientation(RC_PORTRAIT);
  LRemote.setGrid(3, 6);

  // Add a text Battery Voltage label
  batterylabel.setText("0.0V");
  batterylabel.setPos(1, 3);
  batterylabel.setSize(1, 1);
  batterylabel.setColor(RC_GREY);
  LRemote.addControl(batterylabel);

  // Add a Forward button
  forwardbutton.setText("Forward");
  forwardbutton.setPos(1, 0);
  forwardbutton.setSize(1, 1);
  forwardbutton.setColor(RC_BLUE);
  LRemote.addControl(forwardbutton);

  // Add a Backward button
  backwardbutton.setText("Backward");
  backwardbutton.setPos(1, 2);
  backwardbutton.setSize(1, 1);
  backwardbutton.setColor(RC_BLUE);
  LRemote.addControl(backwardbutton);

  // Add a TurnLeft button
  turnleftbutton.setText("TurnLeft");
  turnleftbutton.setPos(0, 1);
  turnleftbutton.setSize(1, 1);
  turnleftbutton.setColor(RC_BLUE);
  LRemote.addControl(turnleftbutton);

  // Add a TurnRight button
  turnrightbutton.setText("TurnRight");
  turnrightbutton.setPos(2, 1);
  turnrightbutton.setSize(1, 1);
  turnrightbutton.setColor(RC_BLUE);
  LRemote.addControl(turnrightbutton);

  // Add a Buzzer button
  buzzerbutton.setText("Beep");
  buzzerbutton.setPos(1, 1);
  buzzerbutton.setSize(1, 1);
  buzzerbutton.setColor(RC_ORANGE);
  LRemote.addControl(buzzerbutton);

  // Add a Auto Demo Switch
  autoswitch.setText("Auto Demo");
  autoswitch.setPos(0, 3);
  autoswitch.setSize(1, 1);
  autoswitch.setColor(RC_YELLOW);
  LRemote.addControl(autoswitch);

  // Add a Battle Switch
  battleswitch.setText("TANK GO !");
  battleswitch.setPos(2, 3);
  battleswitch.setSize(1, 1);
  battleswitch.setColor(RC_YELLOW);
  LRemote.addControl(battleswitch);

  // Add a Music 1 Switch
  music1switch.setText("Marching");
  music1switch.setPos(0, 0);
  music1switch.setSize(1, 1);
  music1switch.setColor(RC_ORANGE);
  LRemote.addControl(music1switch);

  // Add a Music 2 Switch
  music2switch.setText("Katyusha");
  music2switch.setPos(2, 0);
  music2switch.setSize(1, 1);
  music2switch.setColor(RC_ORANGE);
  LRemote.addControl(music2switch);

  // Add a LED button
  ledWbutton.setText("WHITE");
  ledWbutton.setPos(0, 5);
  ledWbutton.setSize(1, 1);
  ledWbutton.setColor(RC_GREY);
  LRemote.addControl(ledWbutton);

  // Add a LED button
  ledRbutton.setText("RED");
  ledRbutton.setPos(0, 4);
  ledRbutton.setSize(1, 1);
  ledRbutton.setColor(RC_PINK);
  LRemote.addControl(ledRbutton);

  // Add a LED button
  ledGbutton.setText("GREEN");
  ledGbutton.setPos(1, 4);
  ledGbutton.setSize(1, 1);
  ledGbutton.setColor(RC_GREEN);
  LRemote.addControl(ledGbutton);

  // Add a LED button
  ledBbutton.setText("BLUE");
  ledBbutton.setPos(2, 4);
  ledBbutton.setSize(1, 1);
  ledBbutton.setColor(RC_BLUE);
  LRemote.addControl(ledBbutton);

  // Add a Turret slider
  setBrightnessslider.setText("LED Brightness");
  setBrightnessslider.setPos(1, 5);
  setBrightnessslider.setSize(2, 1);
  setBrightnessslider.setColor(RC_ORANGE);
  setBrightnessslider.setValueRange(1, 255, 255);
  LRemote.addControl(setBrightnessslider);


  // Start broadcasting our remote contoller
  LRemote.begin();
  Serial.println("LRemote begin ...");
}


/*------------------------------------------------------------*/
void loop() {

  // BLE central device, e.g. an mobile app
  if (!LRemote.connected())
  {
    if (BLE_connect == 1)
    {
      mp3_play (9009);
      delay (2500);
      mp3_stop ();

      //Beep(g, 100);
      //Beep(f, 100);
      //Beep(e, 100);

      BLE_connect = 0;
    }

    Serial.println("Waiting for connection ...");
    delay(10);
  }
  else
  {
    if (BLE_connect == 0)
    {
      mp3_play (9002);
      delay (2500);
      mp3_stop ();

      //Beep(e, 100);
      //Beep(f, 100);
      //Beep(g, 100);

      BLE_connect = 1;
    }

    delay(10);
  }

  // Process the incoming BLE write request
  LRemote.process();


  // Battery Voltage label
  BatteryVol = (BatteryVol + analogRead(BatteryPin) * 0.00285) / 2;

  if (BatteryCount >= 200)
  {
    BatteryCount = 0;
    //Serial.print("BatteryVol = ");
    //Serial.println(BatteryVol);

    if (BatteryVol < BatteryLowVol)
    {
      BatteryLow = 1;
      batterylabel.updateText("Low Battery");
    }
    else
    {
      BatteryLow = 0;
      batterylabel.updateText(String(BatteryVol, 2) + "V");
    }
  }
  else
  {
    BatteryCount++;
  }


  // Music ON/OFF
  if (music1switch.isValueChanged())
  {
    if (music1switch.getValue() == 1)
    {
      mp3_play (12);
    }
    else
    {
      mp3_stop ();
    }
  }

  if (music2switch.isValueChanged())
  {
    if (music2switch.getValue() == 1)
    {
      mp3_play (10);
    }
    else
    {
      mp3_stop ();
    }
  }


  // Motor
  if (forwardbutton.isValueChanged()) {
    if (forwardbutton.getValue() == 1) {
      Move_Forward();
    } else {
      Motor_Break();
    }
  }

  if (backwardbutton.isValueChanged()) {
    if (backwardbutton.getValue() == 1) {
      Move_Backward();
    } else {
      Motor_Break();
    }
  }

  if (turnleftbutton.isValueChanged()) {
    if (turnleftbutton.getValue() == 1) {
      Move_TurnLeft();
    } else {
      Motor_Break();
    }
  }

  if (turnrightbutton.isValueChanged()) {
    if (turnrightbutton.getValue() == 1) {
      Move_TurnRight();
    } else {
      Motor_Break();
    }
  }


  // Buzzer
  if (buzzerbutton.isValueChanged())
  {
    if (buzzerbutton.getValue() == 1)
    {
      if (Battle_OnOff == 1)
      {
        mp3_play (1);

        IR_Send_Code();
        Move_Fire();

        delay (700);
        mp3_stop ();
      }
      else
      {
        Ray_Gun_Sound();
      }
    }
  }


  // Auto Demo ON/OFF
  if (autoswitch.isValueChanged())
  {
    AutoDemo_OnOff = autoswitch.getValue();

    Beep(aH, 200);

    if (AutoDemo_OnOff == 1)
    {
      Pixels.setPixelColor(0, Pixels.Color(0, 0, 150));
      Pixels.show();
      Pixels.setPixelColor(1, Pixels.Color(0, 0, 150));
      Pixels.show();

      mp3_play (9004);

      // Prepare IR Proximity Sensor Value
      Read_Left_IR_Distance(5);
      Read_Right_IR_Distance(5);
      ProximityLeftValue = irLeftDistance;
      ProximityRightValue = irRightDistance;
      ProximityLmaxValue = irLeftDistance * irLeftDivide;
      ProximityRmaxValue = irRightDistance * irRightDivide;

      delay (2000);
      mp3_stop ();
    }
    else
    {
      Motor_Break();

      Pixels.setPixelColor(0, Pixels.Color(150, 150, 150));
      Pixels.show();
      Pixels.setPixelColor(1, Pixels.Color(150, 150, 150));
      Pixels.show();

      mp3_play (9008);
      delay (2000);
      mp3_stop ();
    }
  }


  // IR Proximity Sensor
  if (AutoDemo_OnOff == 1 && Battle_OnOff == 0)
  {
    ProximityValue = 0;

    Read_Left_IR_Distance(5);
    if (irLeftDistance > ProximityLmaxValue) {
      ProximityValue = ProximityValue + 1;
    }

    Read_Right_IR_Distance(5);
    if (irRightDistance > ProximityRmaxValue) {
      ProximityValue = ProximityValue + 2;
    }

    switch (ProximityValue)
    {
      case 0:
        Pixels.setPixelColor(0, Pixels.Color(0, 0, 150));
        Pixels.show();
        Pixels.setPixelColor(1, Pixels.Color(0, 0, 150));
        Pixels.show();

        Move_Forward();
        break;

      case 1:
        Pixels.setPixelColor(0, Pixels.Color(0, 0, 150));
        Pixels.show();
        Pixels.setPixelColor(1, Pixels.Color(150, 0, 0));
        Pixels.show();

        Move_TurnRight();
        delay(300);
        Motor_Break();
        break;

      case 2:
        Pixels.setPixelColor(0, Pixels.Color(150, 0, 0));
        Pixels.show();
        Pixels.setPixelColor(1, Pixels.Color(0, 0, 150));
        Pixels.show();

        Move_TurnLeft();
        delay(300);
        Motor_Break();
        break;

      case 3:
        Motor_Break();
        Pixels.setPixelColor(0, Pixels.Color(150, 0, 0));
        Pixels.show();
        Pixels.setPixelColor(1, Pixels.Color(150, 0, 0));
        Pixels.show();

        mp3_play (9011);
        delay (2000);
        mp3_stop ();

        Move_Backward();
        delay(1500);
        break;
    }

    //Serial.print("ProximityRmaxValue = ");
    //Serial.print(ProximityRmaxValue);
    //Serial.print("  , ProximityLmaxValue = ");
    //Serial.println(ProximityLmaxValue);

    //Serial.print("irRightDistance = ");
    //Serial.print(irRightDistance);
    //Serial.print("  , irLeftDistance = ");
    //Serial.println(irLeftDistance);
  }


  // Battle ON/OFF
  if (battleswitch.isValueChanged())
  {
    Battle_OnOff = battleswitch.getValue();

    Beep(aH, 200);

    if (Battle_OnOff == 1)
    {
      Pixels.setPixelColor(0, Pixels.Color(150, 0, 0));
      Pixels.show();
      Pixels.setPixelColor(1, Pixels.Color(150, 0, 0));
      Pixels.show();

      //mp3_play (9005);
      mp3_play (14);
      delay (2000);
      mp3_stop ();
    }
    else
    {
      Motor_Break();

      Pixels.setPixelColor(0, Pixels.Color(150, 150, 150));
      Pixels.show();
      Pixels.setPixelColor(1, Pixels.Color(150, 150, 150));
      Pixels.show();

      mp3_play (9010);
      delay (2000);
      mp3_stop ();
    }
  }


  // IR Battle System
  if (Battle_OnOff == 1 && AutoDemo_OnOff == 0)
  {
    IR_ADValue_L = analogRead(irLeftPin);
    IR_ADValue_R = analogRead(irRightPin);

    if ( (IR_ADValue_Lavg > IR_ADValue_L * 2.5) or (IR_ADValue_Ravg > IR_ADValue_R * 2.5) )
    {
      mp3_play (2);

      Move_Damage();

      delay (700);
      mp3_stop ();
    }

    IR_ADValue_Lavg = IR_ADValue_L;
    IR_ADValue_Ravg = IR_ADValue_R;

    //Serial.print("IR_ADValue_Lavg = ");
    //Serial.print(IR_ADValue_Lavg);
    //Serial.print(" , IR_ADValue_Ravg = ");
    //Serial.println(IR_ADValue_Ravg);
  }


  // LED Brightness
  if (setBrightnessslider.isValueChanged())
  {
    Pixels.setBrightness(setBrightnessslider.getValue());
    Pixels.show();
    delay(1);
  }


  // White LED
  if (ledWbutton.isValueChanged()) {
    if (ledWbutton.getValue() == 1) {
      Pixels.setPixelColor(0, Pixels.Color(150, 150, 150));
      Pixels.show();
      Pixels.setPixelColor(1, Pixels.Color(150, 150, 150));
      Pixels.show();
      Beep(a, 200);
    }
  }

  // Red LED
  if (ledRbutton.isValueChanged()) {
    if (ledRbutton.getValue() == 1) {
      Pixels.setPixelColor(0, Pixels.Color(150, 0, 0));
      Pixels.show();
      Pixels.setPixelColor(1, Pixels.Color(150, 0, 0));
      Pixels.show();
      Beep(a, 200);
    }
  }

  // Green LED
  if (ledGbutton.isValueChanged()) {
    if (ledGbutton.getValue() == 1) {
      Pixels.setPixelColor(0, Pixels.Color(0, 150, 0));
      Pixels.show();
      Pixels.setPixelColor(1, Pixels.Color(0, 150, 0));
      Pixels.show();
      Beep(a, 200);
    }
  }

  // Blue LED
  if (ledBbutton.isValueChanged()) {
    if (ledBbutton.getValue() == 1) {
      Pixels.setPixelColor(0, Pixels.Color(0, 0, 150));
      Pixels.show();
      Pixels.setPixelColor(1, Pixels.Color(0, 0, 150));
      Pixels.show();
      Beep(a, 200);
    }
  }


}
