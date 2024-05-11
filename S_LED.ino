#include <Wire.h>

/*Dia chi DS3231*/
const byte DS3231 = 0x68;
/*So byte du lieu duoc doc tu DS3231*/
const byte NumberOfFields = 3;

/*Khai bao bien thoi gian*/
int second=10, minute=0, hour=0;                     //Bien luu thoi gian cai dat cho RTC
int second_tmp, minute_tmp, hour_tmp;         //Bien tam dung de luu gia tri thoi gian doc tu RTC
int led = 10, led_mode_0 = 12, led_mode_1 = 11;       //Den hien thi che do Auto = 11, che do hen gio = 12
unsigned char led_str[30];                    //Bien tam luu chuoi
unsigned char ch;
int i =0;                                     //Bien dem 
int mode=0;
int sensor=0;
/*-------------------------------*/
void readDS3231()
{ Wire.beginTransmission(DS3231);       //Truyen dia chi của RTC
  Wire.write((byte)0x00);               //Dinh dia chi thanh ghi bat dau doc thoi gian 0x00
  Wire.endTransmission();
  Wire.requestFrom(DS3231,NumberOfFields);

  second_tmp = bcd2dec(Wire.read() & 0x7f);     //Doc giay
  minute_tmp = bcd2dec(Wire.read() );           //Doc phut
  hour_tmp = bcd2dec(Wire.read() & 0x3f);       //Doc gio che do 24h
}
/*------------------------------*/
/*Chuyen tu BCD sang DEC*/
int bcd2dec(byte num)
{ 
  return  ((num/16*10) + (num%16));
}
/*Chuyen tu DEC sang BCD*/
int dec2bcd(byte num)
{
  return ((num/10*16)+(num%10));  
}
/*------------------------------*/
/*Cai dat thoi gian cho DS3231*/
void setTime(byte second, byte minute, byte hour)
{
  Wire.beginTransmission(DS3231);
  Wire.write(byte(0x00));             //Dat lai Pointer
  Wire.write(dec2bcd(second));
  Wire.write(dec2bcd(minute));
  Wire.write(dec2bcd(hour));
  Wire.endTransmission();
}
/*-------------------------------*/
//Doc gia tri thoi gian tu chuoi va chuyen tu string sang int
int get_time(char mystring[40])
{  char time_str[6];
  int i, n = 0, time_int ;
  for (i=0;i<strlen(mystring);i++)
  { if ((mystring[i]>='0') && (mystring[i]<='9'))
    { time_str[n] = mystring[i];
      n++;
    }
  }
  if (n == 0) return 0;
  else
  { time_int = atoi(time_str);
    return time_int;
  }
}
/*----------------------------------*/
//Doc che do cua den
void get_mode(char mystring[30])
{  char mode_str[7];
  strncpy(mode_str,mystring+7,7);     //Tach 3 ki tu dau tien khoi chuoi
  mode_str[7] = '\0';
  if ((strcmp(mode_str,"tu dong")) ==0){
    mode = 0;
   //Auto mode = 0, Hen gio = 1
  }
  else if ((strcmp(mode_str,"hen gio")) == 0){
    mode = 1;
  }
  else{
    mode = mode;  
  }
}
/*------------------------------*/
//Doc trang thai cua den
int get_state(char mystring[30])
{   
  String state =  "123";

  for(int i=0; i<3;i++)
  {
    state[i] = char(mystring[i]);
   }
   Serial.print("state: ");
   if(state == "mo "){
    Serial.println(1);
    return 1;
   }else if(state == "tat"){
    Serial.println(0);
      return 0;
   }
}
/*--------------------------------*/
/*Xoa du lieu trong mang*/
void clr_str (char string[30])
{
  for (int i = 0; i<30;i++)
  {
    string[i] = 0x00;
  }
}
/*-----------------------------------*/
/*Tinh toan thoi gian cai dat*/
void time_cal(int time_tmp)
{    int time_div=0;
     second = time_tmp%60;     //Doi sang giay
     time_div = time_tmp/60;
     hour = time_div/60;     //Doi sang gio
     minute = time_div%60;    //Doi sang phut
}
void setup() {
  // put your setup code here, to run once:
    Wire.begin();             //Khoi dong I2C
    /*Cai dat thoi gian cho RTC*/
    Serial.begin(9600);

    pinMode(led, OUTPUT);       //Dat chan 13 lam OUTPUT
    digitalWrite(led, HIGH);         //Tat den
    pinMode(2,INPUT_PULLUP);   
    pinMode(11, OUTPUT); 
    pinMode(12, OUTPUT);    
}
/*---------------------------------*/
/*Reset thoi gian*/
void reset_Time()
{
  second=0;
  minute=0;
  hour=0;
}
void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available())
  {
    ch = Serial.read();               //Doc du lieu tu Buffer
    if (ch!='\n')
    {  led_str[i]=ch;
       i++;
    }
    
    else 
    {
      
      i = 0;
      Serial.print("get mode: ");
      get_mode(led_str);
      Serial.println(mode);
      
      int state_ = get_state(led_str); 
      if ( (state_ == 1) && (mode==1) )
      {
        Serial.print("doc trang thai: ");
        
        int time_tmp=0;
        time_tmp = get_time(led_str);       //Doc thoi gian tu chuoi
        Serial.println(time_tmp);
        time_cal(time_tmp);                         //Tinh toan thoi gian cai dat
        setTime(0,0,0);                     //Cai dat thoi gian cho RTC
        digitalWrite(led,HIGH);             //Bat den
        delay(1);                           //Delay 1ms
        readDS3231();                       //Doc thoi gian tu RTC lan dau
        clr_str(led_str);
      }
     else if (state_ == 0) 
     {
      clr_str(led_str);
      Serial.println("tat den");
      digitalWrite(led,LOW);              //Tat den
                       //Xoa chuoi
     }
  
   }
 }
  readDS3231(); //Doc thoi gian tu RTC lan dau
//   Serial.print("Mode:");
//   Serial.println(mode);
  if ( (second_tmp==second) && (minute_tmp==minute) && (hour_tmp==hour) && ((second!=0)|(minute!=0)|(hour!=0)) ) //Da du thoi gian, tat den
     {  digitalWrite(led, LOW);         //Tat den
        setTime(0,0,0);                  //Dat lai thoi gian cho RTC
        reset_Time();                    //Reset lai thoi gian cai dat
     }
  if (mode==0)            //Che do Auto
  { 
    digitalWrite(led_mode_1, HIGH);
    digitalWrite(led_mode_0, LOW);
    sensor = digitalRead(2);
//    Serial.println("che do auto");
    if (sensor==1) digitalWrite(led,HIGH);
    else digitalWrite(led,LOW); 
//     Serial.println(sensor);
  }
  else if (mode==1)
  {
    digitalWrite(led_mode_0, HIGH);
    digitalWrite(led_mode_1, LOW);
  }
}
