#include <LiquidCrystal_I2C.h>
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);

LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <EEPROM.h>


const int btnChange = 2;
const int btnOK = 3;
const int btnMode = 4;
const int buzzer = 5;

int alarmHour = 12;
int alarmMinutes = 0;
boolean hourMode = 0;
boolean alarmOn = 1;
int M = 1;
int D = 1;
int Y = 2023;
int day = 7;
int h = 23;
int m = 59;

int addrH[] = { 0, 1, 2, 3, 4 };
int addrM[] = { 5, 6, 7, 8, 9 };
int addrActive[] = { 10, 11, 12, 13, 14 };

int mode = 1;
int submode = 1;
int slot = 0;

byte clockChar[] = { B00000, B01110, B10101, B10101, B10111, B10001, B01110, B00000 };
byte alarmChar[] = { B00100, B01110, B01110, B01110, B11111, B00000, B00100, B00000 };
byte dateChar[] = { B11111, B00000, B01100, B01010, B01010, B01100, B00000, B11111 };

void setup() {

  pinMode(btnChange, INPUT_PULLUP);
  pinMode(btnOK, INPUT_PULLUP);
  pinMode(btnMode, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);


  lcd.init();       // initialize the LCD
  lcd.backlight();  // Turn on the blacklight
  lcd.createChar(0, clockChar);
  lcd.createChar(1, alarmChar);
  lcd.createChar(2, dateChar);
  Rtc.Begin();
  Serial.begin(9600);
}

///////////////////////////////////////////////////////////////////   void LOOP  ///////////////////////////////////////////

void loop() {
  RtcDateTime now = Rtc.GetDateTime();
  if (digitalRead(btnMode) == LOW) {
    mode++;
    Serial.println("btnMode press");
    if (mode == 4) {
      mode = 1;
    }
    submode = 0;
    lcd.clear();
    delay(200);
  }

  if (mode == 1) {
    clockDisplay();
    // Serial.println(mode);
    delay(200);
  }

  if (mode == 2) {
    alarmMode();
    // Serial.println(mode);
    delay(200);
  }

  if (mode == 3) {
    updateRTC();
    // Serial.println(mode);
    delay(200);
  }
  if (mode == 0) {
    alarm();
    // Serial.println(mode);
    EEPROM.write(addrActive, 0);
    delay(200);
  }
  for (int x = 0; x < 5; x++) {
    if (EEPROM.read(addrActive[x]) == 1) {
      Serial.print("alarm slot active: ");
      if (now.Hour() == EEPROM.read(addrH[x]) && now.Minute() == EEPROM.read(addrM[x])) {
        EEPROM.write(addrActive[x], 0);
        mode = 0;
      }
    }
  }
}

void alarm() {
  clockDisplay();
  lcd.noBacklight();
  digitalWrite(buzzer, HIGH);
  delay(300);
  lcd.backlight();
  digitalWrite(buzzer, LOW);
  delay(200);
}


///////////////////////////////////////////////////////////////////   CLOCK DISPLAY  ///////////////////////////////////////////

void clockDisplay() {
  RtcDateTime now = Rtc.GetDateTime();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(0);

  lcd.setCursor(0, 1);
  switch (now.Month()) {
    case 1: lcd.print("Jan"); break;
    case 2: lcd.print("Feb"); break;
    case 3: lcd.print("Mar"); break;
    case 4: lcd.print("Apr"); break;
    case 5: lcd.print("May"); break;
    case 6: lcd.print("Jun"); break;
    case 7: lcd.print("Jul"); break;
    case 8: lcd.print("Aug"); break;
    case 9: lcd.print("Sep"); break;
    case 10: lcd.print("Oct"); break;
    case 11: lcd.print("Nov"); break;
    case 12: lcd.print("Dec"); break;
  }
  lcd.print(" ");
  if (now.Day() < 10) {
    lcd.print("0");
  }

  lcd.print(now.Day());

  lcd.print(",");
  lcd.print(now.Year());

  lcd.setCursor(13, 1);
  switch (now.DayOfWeek()) {
    case 0: lcd.print("Sun"); break;
    case 1: lcd.print("Mon"); break;
    case 2: lcd.print("Tue"); break;
    case 3: lcd.print("Wed"); break;
    case 4: lcd.print("Thu"); break;
    case 5: lcd.print("Fri"); break;
    case 6: lcd.print("Sat"); break;
  }
  lcd.setCursor(4, 0);
  if ((now.Hour() > 0 && now.Hour() < 10)) {
    lcd.print("0");
  }

  lcd.print(now.Hour());
  lcd.print(":");
  if (now.Minute() < 10) {
    lcd.print("0");
  }
  lcd.print(now.Minute());
  lcd.print(":");
  if (now.Second() < 10) {
    lcd.print("0");
  }
  lcd.print(now.Second());
}

///////////////////////////////////////////////////////////////////   ALARM FUNCTION  ///////////////////////////////////////////
void alarmMode() {
  if (submode == 0) {
    lcd.setCursor(0, 0);
    lcd.write(1);
    lcd.setCursor(2, 0);
    lcd.print("Select:");

    if (digitalRead(btnChange) == LOW) {
      slot++;
      if (slot > 4) {
        slot = 0;
      }
      delay(200);
    }
    lcd.setCursor(10, 0);
    lcd.print("Slot#");
    lcd.print(slot + 1);
    lcd.setCursor(0, 1);
    if (EEPROM.read(addrH[slot]) > 23) {
      EEPROM.write(addrH[slot], 0);
    }
    if (EEPROM.read(addrM[slot]) > 59) {
      EEPROM.write(addrM[slot], 0);
    }
    if (EEPROM.read(addrActive[slot]) > 1) {
      EEPROM.write(addrActive[slot], 0);
    }
    if ((EEPROM.read(addrH[slot]) > 12 && EEPROM.read(addrH[slot]) < 22) || (EEPROM.read(addrH[slot]) > 0 && EEPROM.read(addrH[slot]) < 10)) {
      lcd.print("0");
    }
    if (EEPROM.read(addrH[slot]) == 0) {
      lcd.print("12");
      alarmHour = 12;
    } else if (EEPROM.read(addrH[slot]) <= 12) {
      lcd.print(EEPROM.read(addrH[slot]));
      alarmHour = EEPROM.read(addrH[slot]);
    } else {
      lcd.print(EEPROM.read(addrH[slot]) - 12);
      alarmHour = EEPROM.read(addrH[slot]) - 12;
    }
    lcd.print(":");
    if (EEPROM.read(addrM[slot]) < 10) {
      lcd.print("0");
    }
    lcd.print(EEPROM.read(addrM[slot]));
    alarmMinutes = EEPROM.read(addrM[slot]);

    lcd.print("   ");
    if (EEPROM.read(addrActive[slot]) == 1) {
      lcd.print("ON ");
    } else {
      lcd.print("OFF");
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.write(1);
    lcd.setCursor(2, 0);
    lcd.print("Set ||");
    lcd.setCursor(0, 1);
    lcd.print("Alarm ||");
    lcd.setCursor(11, 1);
    if (alarmHour < 10) {
      lcd.print("0");
      lcd.setCursor(12, 1);
    }
    lcd.print(alarmHour);
    lcd.setCursor(13, 1);
    lcd.print(":");
    lcd.setCursor(14, 1);
    if (alarmMinutes < 10) {
      lcd.print("0");
      lcd.setCursor(15, 1);
    }
    lcd.print(alarmMinutes);
  }

  if (digitalRead(btnOK) == LOW) {
    submode++;
    if (submode > 3) {
      submode = 0;
    }
    delay(200);
    lcd.clear();
  }
  if (submode == 1) {
    lcd.setCursor(11, 0);
    lcd.write(1);
    lcd.write(1);
    if (digitalRead(btnChange) == LOW) {
      alarmHour++;
      if (alarmHour > 23) {
        alarmHour = 0;
      }
      delay(500);
    }
  }
  if (submode == 2) {
    lcd.setCursor(14, 0);
    lcd.write(1);
    lcd.write(1);
    if (digitalRead(btnChange) == LOW) {
      alarmMinutes++;
      if (alarmMinutes >= 60) {
        alarmMinutes = 0;
      }
      delay(500);
    }
  }

  while (submode == 3) {
    lcd.setCursor(0, 0);
    lcd.print("Activate Alarm?");
    lcd.setCursor(0, 1);
    lcd.print("Slot #");
    lcd.print(slot + 1);
    lcd.print("   ");
    if (alarmOn == 1) {
      lcd.print("ON ");
    } else {
      lcd.print("OFF");
    }
    if (digitalRead(btnChange) == LOW) {
      alarmOn = !alarmOn;
      delay(200);
    }

    if (digitalRead(btnOK) == LOW && alarmOn == 1) {
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Saving.");
      delay(200);
      lcd.print(".");
      delay(200);
      lcd.print(".");
      delay(200);
      lcd.print(".");
      delay(200);
      lcd.print(".");
      delay(200);
      lcd.print(".");
      delay(200);
      lcd.print(".");
      delay(200);
      lcd.print(".");
      delay(200);
      lcd.print(".");
      if (hourMode == 1 && alarmHour != 12) {
        EEPROM.write(addrH[slot], alarmHour + 12);
      } else if (hourMode == 0 && alarmHour == 12) {
        EEPROM.write(addrH[slot], 0);
      } else {
        EEPROM.write(addrH[slot], alarmHour);
      }

      EEPROM.write(addrM[slot], alarmMinutes);
      EEPROM.write(addrActive[slot], 1);
      submode = 0;
      mode = 1;
      lcd.clear();
    }
    if (digitalRead(btnOK) == LOW && alarmOn == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alarm Slot#");
      lcd.print(slot + 1);
      lcd.setCursor(0, 1);
      lcd.print(" OFF      ");

      EEPROM.write(addrActive[slot], 0);
      submode = 0;
      mode = 1;

      delay(2000);
      lcd.clear();
    }
  }
}

///////////////////////////////////////////////////////////////////   SETTING DATE & TIME FUNCTION  ///////////////////////////////////////////

void updateRTC() {
  lcd.setCursor(0, 0);
  lcd.write(2);
  if (digitalRead(btnOK) == LOW) {
    submode++;
    if (submode > 7) {
      submode = 0;
    }
    delay(200);
    lcd.clear();
  }

  if (submode == 0) {
    lcd.setCursor(2, 0);
    lcd.print(" Set Date/Time?");
    lcd.setCursor(0, 1);
    lcd.print("Press OK button");
  }
  if (submode == 1) {
    lcd.setCursor(2, 0);
    lcd.print("Set Month");
    lcd.setCursor(0, 1);
    lcd.print("--> ");
    if (digitalRead(btnChange) == LOW) {
      M++;
      if (M == 13) {
        M = 1;
      }
      delay(200);
    }
    switch (M) {
      case 1:
        lcd.print("January     ");
        break;
      case 2:
        lcd.print("February    ");
        break;
      case 3:
        lcd.print("March       ");
        break;
      case 4:
        lcd.print("April       ");
        break;
      case 5:
        lcd.print("May         ");
        break;
      case 6:
        lcd.print("June        ");
        break;
      case 7:
        lcd.print("July        ");
        break;
      case 8:
        lcd.print("August      ");
        break;
      case 9:
        lcd.print("September   ");
        break;
      case 10:
        lcd.print("October     ");
        break;
      case 11:
        lcd.print("November    ");
        break;
      case 12:
        lcd.print("December    ");
        break;
    }
  }
  if (submode == 2) {
    lcd.setCursor(2, 0);
    lcd.print("Set Day");
    lcd.setCursor(0, 1);
    lcd.print("--> ");
    if (digitalRead(btnChange) == LOW) {
      D++;
      if (D == 32) {
        D = 1;
      }
      delay(200);
    }
    lcd.print(D);
    lcd.print(" ");
  }

  if (submode == 3) {
    lcd.setCursor(2, 0);
    lcd.print("Set Year");
    lcd.setCursor(0, 1);
    lcd.print("--> ");
    if (digitalRead(btnChange) == LOW) {
      Y++;
      if (Y == 2099) {
        Y = 2000;
      }
      delay(200);
    }
    lcd.print(Y);
  }

  if (submode == 4) {
    lcd.setCursor(2, 0);
    lcd.print("Set Hour");
    lcd.setCursor(0, 1);
    lcd.print("--> ");
    if (digitalRead(btnChange) == LOW) {
      h++;
      if (h == 24) {
        h = 0;
      }
      delay(200);
    }
    lcd.print(h);
    lcd.print(" ");
  }
  if (submode == 5) {
    lcd.setCursor(2, 0);
    lcd.print("Set Minutes");
    lcd.setCursor(0, 1);
    lcd.print("--> ");
    if (digitalRead(btnChange) == LOW) {
      m++;
      if (m == 60) {
        m = 0;
      }
      delay(200);
    }
    lcd.print(m);
    lcd.print(" ");
  }
  if (submode == 6) {
    lcd.setCursor(2, 0);
    lcd.print("Date & Time");
    lcd.setCursor(0, 1);
    lcd.print("Updating");
    delay(200);
    lcd.print(".");
    delay(200);
    lcd.print(".");
    delay(200);
    lcd.print(".");
    delay(200);
    lcd.print(".");
    delay(200);
    lcd.print(".");
    delay(200);
    lcd.print(".");
    delay(200);
    lcd.print(".");
    delay(200);
    lcd.print(".");


    String newMonth = "";
    String newDate = "";

    switch (M) {
      case 1: newMonth = "Jan"; break;
      case 2: newMonth = "Feb"; break;
      case 3: newMonth = "Mar"; break;
      case 4: newMonth = "Apr"; break;
      case 5: newMonth = "May"; break;
      case 6: newMonth = "Jun"; break;
      case 7: newMonth = "Jul"; break;
      case 8: newMonth = "Aug"; break;
      case 9: newMonth = "Sep"; break;
      case 10: newMonth = "Oct"; break;
      case 11: newMonth = "Nov"; break;
      case 12: newMonth = "Dec"; break;
    }

    newDate = newMonth + ' ' + D + ' ' + Y;

    char userDate[11];
    userDate[0] = newDate[0];
    userDate[1] = newDate[1];
    userDate[2] = newDate[2];
    userDate[3] = newDate[3];
    userDate[4] = newDate[4];
    userDate[5] = newDate[5];
    userDate[6] = newDate[6];
    userDate[7] = newDate[7];
    userDate[8] = newDate[8];
    userDate[9] = newDate[9];
    userDate[10] = newDate[10];
    char userTime[8];
    userTime[0] = h / 10 + '0';
    userTime[1] = h % 10 + '0';
    userTime[2] = ':';
    userTime[3] = m / 10 + '0';
    userTime[4] = m % 10 + '0';
    userTime[5] = ':';
    userTime[6] = '0';
    userTime[7] = '0';

    RtcDateTime manual = RtcDateTime(userDate, userTime);
    Rtc.SetDateTime(manual);
    delay(200);
    lcd.clear();
    mode = 1;
  }
}
