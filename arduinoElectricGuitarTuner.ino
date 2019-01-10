//Electric Guitar Tuner
//Free for use and modification
//Written by: Noah St. Pierre and Ethan Gibson
//Credit for frequency detection code to:
//https://github.com/akellyirl/Arduino-Guitar-Tuner


// include the library code:
#include <LiquidCrystal.h>

#define LENGTH 512

byte rawData[LENGTH];
int count;
char noteName;

// Sample Frequency in kHz
const float sample_freq = 8919;

int len = sizeof(rawData);
int i,k;
long sum, sum_old;
int thresh = 0;
float freq_per = 0;
byte pd_state = 0;

//Base 0 octave frequencies
//float freq = 415;  // dont neet this 
float Ffreq;
float Note;

char testput;
String out = "b----[[  ]]----#";
int octave_counter;
float C = 16.35;
float D = 18.35;
float E = 20.60;
float F = 21.83;
float G = 24.50;
float A = 27.50; 
float B = 30.87;

// initialize the library with the numbers of the interface pins
//Pin 12 = RS
//Pin 11 = E
//Pin 5 = DB4
//Pin 4 = DB5
//Pin 3 = DB6
//Pin 2 = DB7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  Serial.begin(115200);

  analogReference(EXTERNAL);   // Connect to 3.3V
  analogRead(A0);
  
  //string output = 0;
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.setCursor(0,0); lcd.print(" EE421: Signals");
  lcd.setCursor(0,1); lcd.print("  Guitar Tuner");
  delay(3000);
  lcd.clear();
  lcd.setCursor(2,0); lcd.print("[");
  lcd.setCursor(13,0); lcd.print("]");
  // Print a message to the LCD.
   
  count = 0;
}
float freq;
void loop() {
  if (count < LENGTH) {
    count++;
    rawData[count] = analogRead(A0)>>2;
  }
  else {
    sum = 0;
    pd_state = 0;
    int period = 0;
    for(i=0; i < len; i++)
    {
      // Autocorrelation
      sum_old = sum;
      sum = 0;
      for(k=0; k < len-i; k++) sum += (rawData[k]-128)*(rawData[k+i]-128)/256;
      // Serial.println(sum);
      
      // Peak Detect State Machine
      if (pd_state == 2 && (sum-sum_old) <=0) 
      {
        period = i;
        pd_state = 3;
      }
      if (pd_state == 1 && (sum > thresh) && (sum-sum_old) > 0) pd_state = 2;
      if (!i) {
        thresh = sum * 0.5;
        pd_state = 1;
      }
    }
    // for(i=0; i < len; i++) Serial.println(rawData[i]);
    // Frequency identified in Hz
    if (thresh >100) {
      freq_per = sample_freq/period;
      //Serial.println(freq_per);

      //Filter out frequencies that are too high to matter
      if(freq_per < 400)
      {
        freq = freq_per;  
      }
      else
      {
        freq = -1;  
      }
    
    }
    count = 0;
    Serial.println(freq);
    displayToLCD(freq);
    delay(400);
  }  
}

void displayToLCD(float freq){
  if(freq == -1)
  {
    return;
  }
  
  if(freq >= 15.89){ // check if above minimum C;
    octave_counter = -1;
    Ffreq=getFfreq(freq);

    if((15.89<=Ffreq) & (Ffreq<=17.34)){ 
      Note = C;
      noteName = 'C';
    }
    else if((17.35<=Ffreq) & (Ffreq<19.475)){ 
      Note = D;
      noteName = 'D';
    }
    else if((19.475<=Ffreq) & (Ffreq<21.215)){
      Note = E;
      noteName = 'E';
    }
    else if((21.215<=Ffreq) & (Ffreq<23.185)){
      Note = F;
      noteName = 'F';
    }
    else if((23.185<=Ffreq) & (Ffreq<26.00)){
      Note = G;
      noteName = 'G';
    }
    else if((26.00<=Ffreq) & (Ffreq<29.185)){
      Note = A;
      noteName = 'A';
    }
    else if((29.185<=Ffreq) & (Ffreq<31.785)){
      Note = B;
      noteName = 'B';
    }
    
    float closeness0 = (Ffreq/Note);
    int cl1 = 0;
    cl1 = int((closeness0-1)*100); // round to nearest whole number
    if(Ffreq==Note){out = "b----[[  ]]----#";}
    else if(cl1==-1) out = "b----[[  ]]----#"; else if(cl1==1) out = "b----[[  ]]----#";
    else if(cl1==-2) out = "b---<<<  >-----#"; else if(cl1==2) out = "b-----<  >>>---#";
    else if(cl1==-3) out = "b--<<<<  >-----#"; else if(cl1==3) out = "b-----<  >>>>--#";
    else if(cl1==-4) out = "b-<<<<<  >-----#"; else if(cl1==4) out = "b-----<  >>>>>-#";
    else if(cl1==-5) out = "b<<<<<<  >-----#"; else if(cl1==5) out = "b-----<  >>>>>>#"; 
  }
  else{
    Ffreq = -1;
  } // -1 one for too low
   // count++;
   
    lcd.setCursor(3,0);
    lcd.print(freq);
    lcd.setCursor(11,0);
    lcd.print("Hz");
    lcd.setCursor(0,1);
    lcd.print(out);  
    lcd.setCursor(7,1);
    lcd.print(noteName);
    lcd.setCursor(8,1);
    lcd.print(octave_counter);
  }

float getFfreq(float freq){
  octave_counter++;
  if(freq > B){ return getFfreq(freq/2);}
  else return freq;
}
