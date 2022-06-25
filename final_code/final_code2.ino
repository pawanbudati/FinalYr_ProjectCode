
// Fill-in information from your Blynk Template here
//#define BLYNK_TEMPLATE_ID           "TMPLxxxxxx"
//#define BLYNK_DEVICE_NAME           "Device"


/*
1.fan speed pins 19,21,22,23 done
2.relay control pins 18,5,4,2 done
3.gas pin 35 and gas alert to blynk app done
4.Mail alert-gas sensor done
5.panic Button read pin 13 
  Buzzer write pin 15 done
  Buzzer time = 10sec modify in line 37
6.motion sensor mechanism  0,2  

V2 pin is to be removed from blynk
*/

#define BLYNK_TEMPLATE_ID "TMPL0dCtcWMo"
#define BLYNK_DEVICE_NAME "Final year Project"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_WROVER_BOARD
//#define USE_TTGO_T7

#include "BlynkEdgent.h"


////pin declarations
int gas_value=0,gas_value_flag=0,slider_value,gas_pin = 34;
int fs1=19,fs2=21,fs3=22,fs4=23 ,flag = 0;                      //fan pins
int R2=18,R3=5,R4=16,R5=17;                       //relay pins
int fR1=0,fR2=0,fR3=0,fR4=0,fR5=0;                      //extra flags
int gas_limit = 3000;                                   // enter gas limit in range(0,4095)
int buzzer_pin = 35, buzzer_write_pin = 15,buzzer_alert_time = 5;
int sw2,sw3,sw4,sw5,Sw_sensor_manual;
int person_count = 0,person_count_flag =0; //Persons count in room
int IR1 = 32,IR2 = 33; // Proximity sensor pins


BLYNK_WRITE(V0){
  slider_value = param.asInt();
}
BLYNK_WRITE(V2){
  sw2 = param.asInt();
}
BLYNK_WRITE(V3){
  sw3 = param.asInt();
}
BLYNK_WRITE(V4){
  sw4 = param.asInt();
}
BLYNK_WRITE(V5){
  sw5 = param.asInt();
}
BLYNK_WRITE(V6){
  Sw_sensor_manual = param.asInt();
}

void setup()
{   
    Blynk.virtualWrite(V7,person_count);
    pinMode(fs1, OUTPUT);
    pinMode(fs2, OUTPUT);
    pinMode(fs3, OUTPUT);
    pinMode(fs4, OUTPUT);
    pinMode(R2, OUTPUT);
    pinMode(R3, OUTPUT);
    pinMode(R4, OUTPUT);
    pinMode(R5, OUTPUT);
    
    digitalWrite(fs1,HIGH);
    digitalWrite(fs2,HIGH);
    digitalWrite(fs3,HIGH);
    digitalWrite(fs4, HIGH);
    digitalWrite(R2, HIGH);
    digitalWrite(R3, HIGH);
    digitalWrite(R4, HIGH);
    digitalWrite(R5, HIGH);
    
    pinMode(gas_pin,INPUT);
    pinMode(buzzer_pin,INPUT);
    pinMode(buzzer_write_pin, OUTPUT);
    digitalWrite(buzzer_write_pin, HIGH);
    pinMode(IR1, INPUT);
    pinMode(IR2, INPUT);
    

  Serial.begin(115200);
  BlynkEdgent.begin();
}

void loop() {
  BlynkEdgent.run();
  applianceControl();
  gas_monitor();
  buzzerCheck();
  countpeople();    

  if(gas_value != gas_value_flag){
        Serial.println(gas_value);
        gas_value_flag = gas_value;
        }
  if(person_count != person_count_flag){
            Blynk.virtualWrite(V7,person_count);
            Serial.println(person_count);
            person_count_flag = person_count;
        }
}

void countpeople(void){
    if( digitalRead(IR1)==LOW ){
        delay(1000);
        personCount++;
    }
    if( digitalRead(IR2)==LOW){
        delay(1000);
        if(personCount != 0)
            personCount--;
    }
}


void buzzerCheck(void){
  int v = analogRead(buzzer_pin);
  Serial.print("B");
   Serial.println(analogRead(buzzer_pin));
  if(v == 4095) {
    playbuzzer();
    alertMail("Panic Buzzer Triggered...",0);}
}
void playbuzzer(void){
  for(int i=0; i<=buzzer_alert_time*10; i++){
    digitalWrite(buzzer_write_pin,LOW);
    delay(50);
    digitalWrite(buzzer_write_pin,HIGH);
    delay(50);
  }
}
void playbuzzer2(void){
  for(int i=0; i<=buzzer_alert_time*5; i++){
    digitalWrite(buzzer_write_pin,LOW);
    delay(100);
    digitalWrite(buzzer_write_pin,HIGH);
    delay(100);
  }
}
void buzzerClick(void){
    digitalWrite(buzzer_write_pin,LOW);
    delay(50);
    digitalWrite(buzzer_write_pin,HIGH);
}

void gas_monitor(void){
  gas_value = analogRead(gas_pin);
  if (gas_value >= 2000 ) Blynk.virtualWrite(V1,gas_value);
  else Blynk.virtualWrite(V1,0);
  if (gas_value >= gas_limit) {
    alertMail("Gas leakage detected...",gas_value);
    playbuzzer2();
  }
}

// void alertGasMail(int value){
//   String sss = "https://maker.ifttt.com/trigger/gas_detected/with/key/bzYOuJ1YkEFUpYNkwpPYn-?value1="+String(value); //+"&value2="+String(v2) ;
//   runHttp(sss);
// }
void alertMail(String msg,int val2){
  String sss = "https://maker.ifttt.com/trigger/gas_detected/with/key/bzYOuJ1YkEFUpYNkwpPYn-?value1="+String(msg);+"&value2="+String(val2) ;
  runHttp(sss);
}

void runHttp(String url){
    HTTPClient http;
    http.begin(url); //Specify the URL
    int httpCode = http.GET();//Make the request
    if (httpCode > 0) 
    { 
      //Check for the returning code
        String payload = http.getString();
        Serial.println(httpCode + payload);
    }
    else 
    {
      Serial.println("Error on HTTP request");
    }
    http.end(); //Free the resources 
}

void applianceControl(void){
    if(flag != slider_value){
    if(slider_value == 0){
      digitalWrite(fs1,HIGH);
      digitalWrite(fs2,HIGH);
      digitalWrite(fs3,HIGH);
      digitalWrite(fs4,HIGH);
      flag = slider_value;
      buzzerClick();//plays click
      Serial.println("Fan OFF");
      }
    if(slider_value == 1){
      digitalWrite(fs2,HIGH);
      digitalWrite(fs3,HIGH);
      digitalWrite(fs4,HIGH);
      digitalWrite(fs1,LOW);
      flag = slider_value;
      buzzerClick();//plays click
      Serial.println("Fan Speed 1");
      }
    if(slider_value == 2){
      digitalWrite(fs1,HIGH);
      digitalWrite(fs3,HIGH);
      digitalWrite(fs4,HIGH);
      digitalWrite(fs2,LOW);
      flag = slider_value;
      buzzerClick();//plays click
      Serial.println("Fan Speed 2");
      }
    if(slider_value == 3){
      digitalWrite(fs1,HIGH);
      digitalWrite(fs2,HIGH);
      digitalWrite(fs4,HIGH);
      digitalWrite(fs3,LOW);
      flag = slider_value;
      buzzerClick();//plays click
      Serial.println("Fan Speed 3");
      }
    if(slider_value == 4){
      digitalWrite(fs1,HIGH);
      digitalWrite(fs2,HIGH);
      digitalWrite(fs3,HIGH);
      digitalWrite(fs4,LOW);
      flag = slider_value;
      buzzerClick();//plays click
      Serial.println("Fan Speed 4");
      }
    }

    if(Sw_sensor_manual == 1){
        if(fR2!=sw2){
        fR2 = sw2;
          if (sw2==0){
              digitalWrite(R2,HIGH);
              buzzerClick();//plays click 
              Serial.println("Relay 2 OFF");
          }
          else{
              digitalWrite(R2,LOW);
              buzzerClick();//plays click 
              Serial.println("Relay 2 ON");
          }   

      }
      if(fR3!=sw3){
        fR3 = sw3;
          if (sw3==0){
              digitalWrite(R3,HIGH);
              buzzerClick();//plays click 
              Serial.println("Relay 3 OFF");
          }
          else{
              digitalWrite(R3,LOW);
              buzzerClick();//plays click 
              Serial.println("Relay 3 ON");
          }
        }
    }

    else{
      if(person_count > 0){
        fR2 = 1;
        digitalWrite(R2,LOW);
        // buzzerClick();//plays click 
        Serial.println("Relay 2 ON");
        Blynk.virtualWrite(V2,1);
        fR3 = 1;
        digitalWrite(R3,LOW);
        // buzzerClick();//plays click 
        Serial.println("Relay 3 ON");
        Blynk.virtualWrite(V3,1);
      }
      else{
        fR2 = 0;
        digitalWrite(R2,HIGH);
        // buzzerClick();//plays click 
        Serial.println("Relay 2 OFF");
        Blynk.virtualWrite(V2,0);
        fR3 = 0;
        digitalWrite(R3,HIGH);
        // buzzerClick();//plays click 
        Serial.println("Relay 3 OFF");
        Blynk.virtualWrite(V3,0);
      }
    }
    

    if(fR4!=sw4){
      fR4 = sw4;
        if (sw4==0){
            digitalWrite(R4,HIGH);
            buzzerClick();//plays click 
            Serial.println("Relay 4 OFF");
        }
        else{
            digitalWrite(R4,LOW);
            buzzerClick();//plays click 
            Serial.println("Relay 4 ON");
        }
    }
    if(fR5!=sw5){
      fR5 = sw5;
        if (sw5==0){
            digitalWrite(R5,HIGH);
            buzzerClick();//plays click 
            Serial.println("Relay 5 OFF");
        }
        else{
            digitalWrite(R5,LOW);
            buzzerClick();//plays click 
            Serial.println("Relay 5 ON");
        }
    }
}
