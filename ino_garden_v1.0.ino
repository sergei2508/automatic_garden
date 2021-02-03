
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <virtuabotixRTC.h>

// relay pins
#define relay1      0
#define relay2      1
#define relay3      2
#define relay4      3

// define pins of RTC module
#define rtcSCLK     A3
#define rtcIO       A2
#define rtcCE       A1

#include <DHT.h>
 
// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 11
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11
 
// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

// creation of RTC object
virtuabotixRTC myRTC(rtcSCLK, rtcIO, rtcCE);

// keypad definitions
#define btnRIGHT    0
#define btnUP       1
#define btnDOWN     2
#define btnLEFT     3
#define btnSELECT   4
#define btnNONE     5

// mode definitions
#define modeSETUP   1
#define modeNORMAL  2

int mode = 1; //set mode

// EEPROM address
#define adr1ON    2
#define adr1OF    4
#define adr2ON    6
#define adr3ON    10
#define adr4ON    14
//variables ajuste timer
int eepromMin = 0;
int eepromHour = 0;
int eepromday =0;
int eepromliter =0;
int eepromTemp =0;
int eepromHumidity = 0;
//variables timer
int eepromHourON = 0;
int eepromHourOF = 0;
int eepromMinON = 0;
int eepromMinOF = 0;

//lcd
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int lcd_key     = 0;
int adc_key_in  = 0;

void setup() {
    for (int i = 0; i <= 9; i++) {
    pinMode(i, OUTPUT);
  }

	for (int i = 11; i <= 13; i++) {
		pinMode(i, OUTPUT);
	}

	// activar RTC
	pinMode(rtcSCLK, OUTPUT);
	pinMode(rtcIO,   OUTPUT);
	pinMode(rtcCE,   OUTPUT);

	delay(500);

	// lcd iniciar
	lcd.begin(16, 2);
 //rth iniciar
  dht.begin();
  lcd.setCursor(0, 0);
  lcd.print("MAY BACK");
  lcd.setCursor(0, 1);
  lcd.print("grow v1.0");
  delay(3000);
  lcd.clear();
}

void loop() {
  int hum = dht.readHumidity();
  // Leemos la temperatura en grados centígrados (por defecto)
  int tem = dht.readTemperature();
	lcd.setCursor(0, 0);
	//mostrar hora
	displayTime();
  lcd.print("    ");
  lcd.print(hum);
  lcd.print(" %");

//contenido pantalla
	lcd.setCursor(0, 1); 
	lcd.print("L");
	relayAction(adr1ON, adr1OF, 1, relay1);
	lcd.setCursor(2, 1); 
	lcd.print("I");
	relayActionRiego(adr2ON,3,relay2);
	lcd.setCursor(4, 1); 
	lcd.print("T");
	relayActionTemp(adr3ON, 5, relay3);
	lcd.setCursor(6, 1); 
	lcd.print("H");
	relayActionHumi(adr3ON, 7, relay4);
  lcd.print("    ");
  lcd.print(tem);
  lcd.print(" C");

	// jika user menekan tombol SELECT, masuk menu setting

	if (read_LCD_buttons() == btnSELECT) {
		while (read_LCD_buttons() == btnSELECT);
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("UP.TIME LT.T_&_H");
		lcd.setCursor(0, 1);
		lcd.print("DN.LAMP RT.WATER");
		while (read_LCD_buttons() == btnNONE);
		if (read_LCD_buttons() == btnUP) {
			while (read_LCD_buttons() == btnUP);
			setRTC();   // jika user menekan tombol UP, masuk menu setting RTC
		}
		else if (read_LCD_buttons() == btnDOWN) {
			while (read_LCD_buttons() == btnDOWN);
			setPin();   // jika user menekan tombol DOWN, masuk menu setting PIN
		}
    else if (read_LCD_buttons() == btnRIGHT){
      while (read_LCD_buttons() == btnRIGHT);
      setIrrigation();
      }
    else if (read_LCD_buttons() == btnLEFT){
      while (read_LCD_buttons() == btnLEFT);
      setTempHumi();
      }
	  }
}

void relayActionTemp(int adrON, int pos,int pin){
  int tempC=dht.readTemperature();
  int tempG=(EEPROM.read(adrON));
  lcd.setCursor(pos, 1);
  if((tempC + 2)>tempG){
              lcd.print("*");
    digitalWrite(pin, LOW);
  }
  else{
             lcd.print("-");
      digitalWrite(pin, HIGH); 
  }
}

void relayActionHumi(int adrON, int pos,int pin){
  int humiC=dht.readHumidity();
  int humiG=EEPROM.read(adrON + 1);
  lcd.setCursor(pos, 1);
  if((humiC - 3)<humiG){
              lcd.print("*");
    digitalWrite(pin, LOW);
  }
  else{
             lcd.print("-");
      digitalWrite(pin, HIGH); 
  } 
}



void relayActionRiego(int adrON,int pos,int pin){
  myRTC.updateTime();
  int d = myRTC.dayofweek;
  int h = myRTC.hours;
  int m = myRTC.minutes;
  int dayEprON = (EEPROM.read(adrON));
  int literEprOF = EEPROM.read(adrON + 1);
  lcd.setCursor(pos, 1);
  if(dayEprON==1){
    if((d==1||d==4||d==7)&& h==0 && m<literEprOF){
          lcd.print("*");
    digitalWrite(pin, LOW);
    }
    else{
          lcd.print("-");
      digitalWrite(pin, HIGH);
    }
  }
  else if(dayEprON==2){
    if((d==1||d==3||d==5||d==7)&& h==0 && m<literEprOF){
          lcd.print("*");
    digitalWrite(pin, LOW);
    }
    else{
          lcd.print("-");
      digitalWrite(pin, HIGH);
    }
  }
    else if(dayEprON==3){
    if(h==0 && m<literEprOF){
          lcd.print("*");
    digitalWrite(pin, LOW);
    }
    else{
          lcd.print("-");
      digitalWrite(pin, HIGH);
    }
  }  else {
        lcd.print(" ");
    digitalWrite(pin, HIGH);
  }
}
 //void relay timer
void relayAction(int adrON, int adrOF, int pos, int pin) {
	myRTC.updateTime();
	int MinToday = (myRTC.hours * 60) + myRTC.minutes;
	int MinEprON = (EEPROM.read(adrON) * 60) + EEPROM.read(adrON + 1);
	int MinEprOF = (EEPROM.read(adrOF) * 60) + EEPROM.read(adrOF + 1);

	lcd.setCursor(pos, 1);
	if (MinEprON == MinEprOF) {
		lcd.print("*");
		digitalWrite(pin, LOW);
	}
	else if (MinEprON < MinEprOF) {
		if ((MinEprON <= MinToday) && (MinEprOF > MinToday)) {
			lcd.print("*");
			digitalWrite(pin, LOW);
		}
		else {
			lcd.print("-");
			digitalWrite(pin, HIGH);
		}
	}
	else if (MinEprON > MinEprOF) {
		if ((MinEprON >= MinToday) || (MinEprOF < MinToday)) {
			lcd.print("*");
			digitalWrite(pin, LOW);
		}
		else {
			lcd.print(" ");
			digitalWrite(pin, HIGH);
		}
	}
}
void setTempHumi(){
  lcd.clear();
lcd.setCursor(0,0);
lcd.print("TEMP & HUMIDITY");
delay(1000);

lcd.clear();
lcd.setCursor(0,0);
lcd.print("TEMP    HUMIDITY");

lcd.setCursor(0,0);
while (read_LCD_buttons()== btnNONE);
setupChooseValueTempHumi(eepromTemp,eepromHumidity,adr3ON);
lcd.setCursor(0,0);
lcd.clear();
lcd.print("SETTINGS SAVED         ");
delay(1000);
lcd.clear();
}


//void ajuste riego
void setIrrigation(){
lcd.clear();
lcd.setCursor(0,0);
lcd.print("IRRGIATION");
delay(1000);

lcd.clear();
lcd.setCursor(0,0);
lcd.print("ON DAYS  MINUTES");

lcd.setCursor(0,0);
while (read_LCD_buttons()== btnNONE);
setupChooseValueRiego(eepromday,eepromliter,adr2ON);
lcd.setCursor(0,0);
lcd.clear();
lcd.print("SETTINGS SAVED        ");
delay(1000);
lcd.clear();
}
//void ajuste hora

void setRTC() {

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("TIME SETTING");
	delay(1000);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("CURRENT ");
	setupShowValue(myRTC.hours, myRTC.minutes, 0);

	lcd.setCursor(0, 1);
	lcd.print("NEW ");

	myRTC.updateTime();
	setupShowValue(myRTC.hours, myRTC.minutes, 1);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValueSetRTC(myRTC.hours, myRTC.minutes, 1);
	lcd.setCursor(0,0);
     lcd.clear();
	lcd.print("SETTINGS SAVED   ");
	delay(1000);
	lcd.clear();
}

//void ajuste timer
void setPin() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("PIN SETTING");
	delay(1000);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("LAMP 1 ON");
	eepromHour = EEPROM.read(adr1ON);
	eepromMin = EEPROM.read(adr1ON + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 0);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr1ON, 0);

	lcd.setCursor(0, 1);
	lcd.print("LAMP 1 OFF");
	eepromHour = EEPROM.read(adr1OF);
	eepromMin = EEPROM.read(adr1OF + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 1);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr1OF, 1);

	lcd.clear();
}

void setupChooseValueTempHumi(int tempNew,int humidity, byte Address){
 tempNew= (EEPROM.read(Address));
 humidity = EEPROM.read(Address + 1);
 if (tempNew <15){
        tempNew=15;
      }
 else       if(humidity <40){
        humidity=40;
      }
  while (read_LCD_buttons() != btnSELECT){
    if (read_LCD_buttons() == btnRIGHT) {
      if (tempNew <30){
        tempNew++;
      }
    }
    else if (read_LCD_buttons() ==btnLEFT) {
      if (tempNew >15){
        tempNew--;
      }
    }
    else if (read_LCD_buttons() == btnUP){
      if(humidity <70){
        humidity++;
      }
    }
    else if (read_LCD_buttons ()== btnDOWN){
      if(humidity >40){
        humidity--;
      }
    }
   setupShowTemp(tempNew,humidity);
   delay(150);
  }
    while (read_LCD_buttons() != btnNONE);  // tunggu sampai tombol rilis
  EEPROM.write(Address, tempNew);
  EEPROM.write(Address + 1, humidity);
  delay(150);
}

//voidajusteriego
void setupChooseValueRiego(int dayNew, int Liters, byte Address){
 dayNew= (EEPROM.read(Address));
 Liters = EEPROM.read(Address + 1);
  while (read_LCD_buttons() != btnSELECT){
    if (read_LCD_buttons() == btnRIGHT) {
      if (dayNew <3){
        dayNew++;
      }
    }
    else if (read_LCD_buttons() ==btnLEFT) {
      if (dayNew >0){
        dayNew--;
      }
    }
    else if (read_LCD_buttons() == btnUP){
      if(Liters <9){
        Liters++;
      }
    }
    else if (read_LCD_buttons ()== btnDOWN){
      if(Liters >0){
        Liters--;
      }
    }
   setupShowIrrigation(dayNew,Liters);
   delay(150);
  }
    while (read_LCD_buttons() != btnNONE);  // tunggu sampai tombol rilis
  EEPROM.write(Address, dayNew);
  EEPROM.write(Address + 1, Liters);
  delay(150);
}
//voidajustehora
void setupChooseValue(int HourNew, int MinNew, byte Address, byte Pos) {
	while (read_LCD_buttons() != btnSELECT) {
		if (read_LCD_buttons() == btnRIGHT) {
			if (HourNew < 23) {
				HourNew++;
			}
		}
		else if (read_LCD_buttons() == btnLEFT) {
			if (HourNew > 0) {
				HourNew--;
			}
		}
		else if (read_LCD_buttons() == btnUP) {
			if (MinNew < 59) {
				MinNew++;
			}
		}
		else if (read_LCD_buttons() == btnDOWN) {
			if (MinNew > 0) {
				MinNew--;
			}
		}
		setupShowValue(HourNew, MinNew, Pos);
		delay(150);
	}
	while (read_LCD_buttons() != btnNONE);  // tunggu sampai tombol rilis
	EEPROM.write(Address, HourNew);
	EEPROM.write(Address + 1, MinNew);
	delay(150);
}
//void ajuste hora 2
void setupChooseValueSetRTC(int HourNew, int MinNew, byte Pos) {
	while (read_LCD_buttons() != btnSELECT) {
		if (read_LCD_buttons() == btnRIGHT) {
			if (HourNew < 23) {
				HourNew++;
			}
		}
		else if (read_LCD_buttons() == btnLEFT) {
			if (HourNew > 0) {
				HourNew--;
			}
		}
		else if (read_LCD_buttons() == btnUP) {
			if (MinNew < 59) {
				MinNew++;
			}
		}
		else if (read_LCD_buttons() == btnDOWN) {
			if (MinNew > 0) {
				MinNew--;
			}
		}
		setupShowValue(HourNew, MinNew, Pos);
		delay(150);
	}
	while (read_LCD_buttons() != btnNONE);  // tunggu sampai tombol rilis
	myRTC.setDS1302Time(00, MinNew, HourNew, 1, 10, 1, 2014);
	delay(150);
}

void setupShowTemp(int temp, int humedad){
 lcd.setCursor(0,1);
 lcd.print(temp);
 lcd.print(" C     ");
 lcd.print(humedad); 
 lcd.print(" %");
}

void setupShowIrrigation(int day, int liter){
if(day == 1){
        lcd.setCursor(0,1);
    lcd.print("MON THU SUN    "); //1 4 7
    lcd.print(liter);
  } 
  else if(day == 2){
        lcd.setCursor(0,1);
    lcd.print("MON WED FRI SUN"); // 1 3 5 7
    lcd.print(liter);
  }
  else if(day == 3){
        lcd.setCursor(0,1);
    lcd.print("ALL DAYS       "); //1-7
    lcd.print(liter);
  }
  else{
    lcd.setCursor(0,1);
    lcd.print("NO IRRIGATION  ");
    lcd.print(liter);
  }       
}

void setupShowValue(int Hour, int Min, int Pos) {
	lcd.setCursor(11, Pos);
	print2digits(Hour);
	lcd.print(":");
	print2digits(Min);
}

//botones

int read_LCD_buttons(){
	adc_key_in = analogRead(0);       // read the value from the sensor

	if (adc_key_in > 1000) return btnNONE; 
	if (adc_key_in < 50)   return btnRIGHT;  
	if (adc_key_in < 195)  return btnUP; 
	if (adc_key_in < 380)  return btnDOWN; 
	if (adc_key_in < 555)  return btnLEFT; 
	if (adc_key_in < 790)  return btnSELECT;  
	return btnNONE;                // when all others fail, return this.
}

void eeprom_write_int(int p_address, int p_value) {
	byte lowByte = ((p_value >> 0) & 0xFF);
	byte highByte = ((p_value >> 8) & 0xFF);

	EEPROM.write(p_address, lowByte);
	EEPROM.write(p_address + 1, highByte);
}

unsigned int eeprom_read_int(int p_address) {
	byte lowByte = EEPROM.read(p_address);
	byte highByte = EEPROM.read(p_address + 1);

	return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}
 //void mostrar hora
void displayTime() {
	myRTC.updateTime();
	print2digits(myRTC.hours);
	lcd.print(":");
	print2digits(myRTC.minutes);
	lcd.print(":");
	print2digits(myRTC.seconds);
}
//void 2 digitos
void print2digits(int number) {
	if (number >= 0 && number < 10)
		lcd.print('0');
	lcd.print(number, DEC);
}
