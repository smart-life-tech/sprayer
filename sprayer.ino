/*
  Initialize the servo, LCD screen, joystick, and potentiometer.
  Read the current value of the time selection and the deploy counter from EEPROM.
  Use the potentiometer to select one of the three available time intervals (9, 18, or 36 seconds). Save the new time selection in EEPROM.
  Use the switch embedded in the joystick to reset the counter to 200 and the selected time to 9 seconds. Overwrite the values in EEPROM.
  Display the current time selection and the deploy counter on the LCD screen.
  Use a loop to deploy the servo every selected time interval. Within the loop, decrease the deploy counter and update the value in EEPROM after each deploy.
  Check if the deploy counter reaches 0, if so, stop the loop and display a message on the LCD screen indicating that the system is out of deploys.
*/

#include <Servo.h>
#include <EEPROM.h>
#include <Wire.h>
//#include <LiquidCrystal_I2C.h>

Servo myservo;
//LiquidCrystal_I2C lcd(0x27, 16, 2);
// include the library code:
#include <LiquidCrystal.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int joy_switch = 8;
int joy_pot = A0;
int deploy_counter_address = 0;
int time_selection_address = 1;
int deploy_counter = 200;
long time_selection = 9; // 9 sec
int times = 0;
unsigned long lastMillis = 0; // keeping track of non blocking time
void setup()
{
    myservo.attach(9);
    pinMode(joy_switch, INPUT_PULLUP);
   // lcd.init();
    //lcd.backlight();
    lcd.begin(16, 2);
    deploy_counter = EEPROM.read(deploy_counter_address);
    time_selection = EEPROM.read(time_selection_address);
    Serial.begin(9600);
    // handling first time issue
    if (time_selection == 255)
        time_selection = 9;
    if (deploy_counter == 255)
        deploy_counter = 200;
    display();
}

void loop()
{
    int joy_val = analogRead(joy_pot);
    int switch_val = digitalRead(joy_switch);
    // Serial.print("joystick pos : ");
    // Serial.println(joy_val);
    if (joy_val < 500)
    {
        times++;
        selection(times);
        delay(200);
        display();
    }
    if (joy_val > 600)
    {
        times--;
        selection(times);
        delay(200);
        display();
    }
    if (switch_val == LOW)
    {
        Serial.println("pressed");
        deploy_counter = 200;
        time_selection = 9; // 9 secs
        EEPROM.write(deploy_counter_address, deploy_counter);
        EEPROM.write(time_selection_address, time_selection);
        display();
    }

    if (millis() - lastMillis > (time_selection * 1000))
    {
        Serial.println(time_selection);
        lastMillis = millis();
        Serial.println("elapsed");
        myservo.write(90);
        delay(100);
        myservo.write(0);
        delay(100);
        deploy_counter--;
        EEPROM.write(deploy_counter_address, deploy_counter);
        display();
    }
}
void display()
{
    lcd.clear();
    lcd.print("Dep : ");
    lcd.print(deploy_counter);
    lcd.println("/200");
    lcd.setCursor(0, 1);
    lcd.print("T. Sel: ");
    lcd.print(time_selection);
    lcd.println(" secs");
}
void selection(int select)
{
    if (select >= 3)
        select, times = 3;
    else if (select <= 0)
        select, times = 0;
    switch (select)
    {
    case 1:
        time_selection = 9;
        Serial.println("now on 9 secs");
        break; // 9 secs
    case 2:
        time_selection = 18;
        Serial.println("now on 18 secs");
        break; // 18 secs
    case 3:
        time_selection = 36;
        Serial.println("now on 36 secs");
        break; // 36 secs
    }
    EEPROM.write(time_selection_address, time_selection);
}