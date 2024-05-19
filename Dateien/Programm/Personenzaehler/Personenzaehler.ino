int maximaleAnzahlPersonen = 2;
int aktuelleAnzahlPersonen = 0; 

float zeitMesser = 0.0;         
int zeitSprung = 20;            
int verzoegerung = 1000;

String raumNummer = "B11";     
String Status = "";             
char anzahlAenderung = ' ';     

int personenZahlGesamt;         
int personenZahlRein;           
int personenZahlRaus;          

#include <Wire.h>                                       
#include <LiquidCrystal_I2C.h>                          
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); 

#include "SPI.h"                  
#include "SD.h"                   
int modulSDPin = 5;               
File myFile;                      
String Dateiname = "zaehl3.txt";  //konfigurierbar

int pinSensorRein = 7;      
int pinSensorMitte = 8;     
int pinSensorRaus = 9;      

void setup() {

  Serial.begin(9600);

  pinMode(pinSensorRein, INPUT);    
  pinMode(pinSensorMitte, INPUT);   
  pinMode(pinSensorRaus, INPUT);    
  
  lcd.init();       
  lcd.backlight();                 

  pinMode(modulSDPin, OUTPUT);  
  
  if(!SD.begin(modulSDPin))     
  {
    Serial.print("SD Fail");    
    lcd.setCursor(0,3);         
    lcd.print("SD Fail");       
    while(1);                   
  }
  
  SD.begin(modulSDPin);                     
  myFile = SD.open(Dateiname, FILE_WRITE);  
  myFile.print("Raumnummer: ;");            
  myFile.print(raumNummer);                 
  myFile.println(";");                      
  myFile.println("Zeitstempel [s]; Maximal erlaubte anzahl Personen; anzahl Personen im Raum; Anzahlaenderung; Personenzahl Gesamt; Personenzahl Gesamt raus; Personenzahl Gesamt rein; Status;");  
  myFile.close();                           
}

void loop() {                

      lcd.setCursor(0, 0);    
      lcd.print("Raum: ");   
      lcd.setCursor(10, 0);   
      lcd.print(raumNummer);  

      lcd.setCursor(0,1);               
      lcd.print("Personen: ");          
      lcd.setCursor(10,1);              
      lcd.print("  ");                 
      lcd.setCursor(10,1);              
      lcd.print(aktuelleAnzahlPersonen);
      lcd.setCursor(12,1);              
      lcd.print("/");                   
      lcd.setCursor(13,1);              
      lcd.print(maximaleAnzahlPersonen);
      
      lcd.setCursor(0,3);     
      lcd.print("Status: ");  

      if(digitalRead(pinSensorMitte) !=1)     		
      {
        while(digitalRead(pinSensorMitte) !=1)		
        {
          lcd.setCursor(0,2);                   
          lcd.print("beweg dich");              
          zeitMesserErhoehung();                   
          Status = "Durchgang wird blockiert";  
          anzahlAenderung = '0';                
          Speichern();                          
          delay(zeitSprung);                    
        }
        
        lcd.setCursor(0,2);                 
        lcd.print("          ");            
        Status = "Durchgang wieder frei";   
        anzahlAenderung = '0';              
        Speichern();                        
      }       
        if(aktuelleAnzahlPersonen>=maximaleAnzahlPersonen)  
        {
          Status = "voll";                                  
        }
        if(aktuelleAnzahlPersonen<maximaleAnzahlPersonen)   
        {
          Status = "nicht voll";                            
        }
      lcd.setCursor(10,3);      
      lcd.print("          ");  
      lcd.setCursor(10,3);      
      lcd.print(Status);        

    if(digitalRead(pinSensorRaus) !=1)        
    {
      int fertig = 0;                         
      while(fertig == 0)                      	
      {
        if(digitalRead(pinSensorMitte) != 1)  
        {
          fertig = 1;                        
          if(fertig == 1)                     
          {
            personenZahlGesamt++;             
            if(aktuelleAnzahlPersonen>0)      
            {
              personenZahlRaus++;             
              aktuelleAnzahlPersonen--;      
              anzahlAenderung = '-';
              Speichern();
            }
          }
        }
        if(digitalRead(pinSensorRein) != 1)
        {
          fertig = 1;                      
        }
        delay(zeitSprung);                 
        zeitMesserErhoehung();              
      }
      
    }

    if(digitalRead(pinSensorRein) !=1)        
    {
      int fertig = 0;                         
      while(fertig == 0)                      
      {
        if(digitalRead(pinSensorMitte) != 1)  
        {
          fertig = 1;                         
          if(fertig == 1)                     
          {
            personenZahlGesamt++;             
            personenZahlRein++;               
            aktuelleAnzahlPersonen++;         
            anzahlAenderung = '+';            
            Speichern();                      
          }
        }
        if(digitalRead(pinSensorRaus) != 1)
        {
          fertig = 1;                      
        }
        delay(zeitSprung);                 
        zeitMesserErhoehung();              
      }
    }

    delay(zeitSprung);      
    zeitMesserErhoehung();  
}
void zeitMesserErhoehung() 
{
  zeitMesser = zeitMesser+((float)zeitSprung/1000);
}

void Speichern()                              
{
  myFile = SD.open(Dateiname, FILE_WRITE);    
  myFile.print(zeitMesser, 3);                
  myFile.print(";");                          
  myFile.print(maximaleAnzahlPersonen);       
  myFile.print(";");                         
  myFile.print(aktuelleAnzahlPersonen);       
  myFile.print(";");                          
  myFile.print(anzahlAenderung);              
  myFile.print(";");                          
  myFile.print(personenZahlGesamt);          
  myFile.print(";");                          
  myFile.print(personenZahlRaus);             
  myFile.print(";");                          
  myFile.print(personenZahlRein);             
  myFile.print(";");                         
  myFile.print(Status);                       
  myFile.println(";");                       
  myFile.close();                            
}
