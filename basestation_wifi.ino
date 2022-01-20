#include <string> 
#include <consoleio.h>
#include "cbasestation.h"
#include <messages.h>
#include <boards.h>
#include <radio.h>

#include <esp_task_wdt.h>
#define WDT_TIMEOUT 13

unsigned long t=0;
std::string CSystemObject::_sat="ADR1";
std::map<std::string,CSystemObject *> SysMap;

CMessages* MSG=new CMessages();
CMessages* getMessages() { return MSG; }
CRadio radio;

CBaseStation bs;

void setup() {     
  Serial.begin(115200); 
  while(!Serial){};

  #ifdef TTGO1 
   initBoard();
  #endif
       
  radio.setup();
  bs.addWifi();
  bs.connectWifi();

  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  delay(1000);
}


void(* resetFunc) (void) = 0; //declare reset function @ address 0

void loop(){
  if(millis()>t+5000){
    t=millis();
    writeconsoleln();writeconsole("Loop "); writeconsoleln((long)ESP.getFreeHeap());writeconsoleln(" -------------------------------------------   ");
  }
  std::string cmdstr;
  esp_task_wdt_reset();

  if(bs.isCmdTime())  {    
    bs.getLastCommand();   //Restarts when connecting to service has some problem
  }
        
  cmdstr=bs.kbloop();
  if(cmdstr.size())
    bs.SendCmd(cmdstr);   
  radio.loop(); 
  bs.updateAllTransmittedCommands();
  bs.updateAllReceivedCommands();  
  
  bs.chkIRArray();
}
