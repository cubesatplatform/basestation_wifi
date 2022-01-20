#include <arduino.h>
#include "cbasestation.h"
#include <ArduinoJson.h>

unsigned char CBaseStation::h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

std::string CBaseStation::urldecode(std::string str)
{
    
    std::string encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str.length(); i++){
        c=str[i];
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str[i];
        i++;
        code1=str[i];
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
        
        encodedString+=c;  
      }
      
      yield();
    }
    
   return encodedString;
}

std::string CBaseStation::urlencode(std::string str)
{
    std::string encodedString="";
    std::string encodedString1="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str[i];
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    
    for(auto c:encodedString){      
      if((c=='{')||(c=='}')){}
      else
        encodedString1+=c;
    }
    
    return encodedString1;
    
}

std::string convertStringToCommand(std::string str){
  StaticJsonDocument<500> doc;
  std::string cmdstr;

    DeserializationError error = deserializeJson(doc, str);

  // Test if parsing succeeds.
  if (error)
    {
      writeconsole("deserializeJson() failed: ");
      writeconsoleln(error.f_str());
    }  
  else
    {
      const char* cidstr=doc["cid"];
      const char* bidstr=doc["bid"];
      const char* datastr=doc["data"];
      cmdstr=datastr+std::string("~CID:")+cidstr;
    }
    return(cmdstr);
}

void CBaseStation::init(){
  radio.setACK(false);
  writeconsoleln("Starting Basestation: ");


  simpleCMDs[std::string("normal")]=std::string("SYS:SAT~ACT:NORMAL~SAT:ADR2~ACK:1");
  simpleCMDs[std::string("lowpower")]=std::string("SYS:SAT~ACT:LOWPOWER~ACK:1");
  simpleCMDs[std::string("deploy")]=std::string("SYS:SAT~ACT:DEPLOY~ACK:1");
  
  simpleCMDs[std::string("adcs")]=std::string("SYS:SAT~ACT:ADCS~SAT:ADR2~ACK:1");
  simpleCMDs[std::string("detumble")]=std::string("SYS:SAT~ACT:DETUMBLE~ACK:1");
  simpleCMDs[std::string("phonestate")]=std::string("SYS:SAT~ACT:PHONE~ACK:1");
  
  simpleCMDs[std::string("gps")]=std::string("SYS:GPS~ACT:START~ACK:0");
  simpleCMDs[std::string("reset")]=std::string("SYS:SAT~ACT:RESET~ACK:0");


  simpleCMDs[std::string("TRANSMITDATA")]=std::string("SYS:SAT~ACT:TRANSMITDATA~ACK:1");
  simpleCMDs[std::string("DATALISTCLEAR")]=std::string("SYS:SAT~ACT:DATALISTCLEAR~ACK:1"); 
  simpleCMDs[std::string("MESSAGESLISTCLEAR")]=std::string("SYS:SAT~ACT:MESSAGESLISTCLEAR~ACK:1");
  simpleCMDs[std::string("TRANSMITLISTCLEAR")]=std::string("SYS:SAT~ACT:TRANSMITLISTCLEAR~ACK:1");
  simpleCMDs[std::string("TRANSMITTEDLISTCLEAR")]=std::string("SYS:SAT~ACT:TRANSMITTEDLISTCLEAR~ACK:1");

  simpleCMDs[std::string("datatest")]=std::string("SYS:SAT~ACT:OUTPUT~DATA:000000000000000000000000111111111111111111111111111111111222222222222222222222222222223333333333333333333333333333333444444444444444444444444444444444444555555555555555555555555555555555556666666666666666666666666666677777777777777777777777777777777777778888888888888888888888888888888888999999999999999999999999999999999000000000000000000000000111111111111111111111111111111111222222222222222222222222222223333333333333333333333333333333444444444444444444444444444444444444555555555555555555555555555555555556666666666666666666666666666677777777777777777777777777777777777778888888888888888888888888888888888999999999999999999999999999999999000000000000000000000000111111111111111111111111111111111222222222222222222222222222223333333333333333333333333333333444444444444444444444444444444444444555555555555555555555555555555555556666666666666666666666666666677777777777777777777777777777777777778888888888888888888888888888888888999999999999999999999999999999999000000000000000000000000111111111111111111111111111111111222222222222222222222222222223333333333333333333333333333333444444444444444444444444444444444444555555555555555555555555555555555556666666666666666666666666666677777777777777777777777777777777777778888888888888888888888888888888888999999999999999999999999999999999000000000000000000000000111111111111111111111111111111111222222222222222222222222222223333333333333333333333333333333444444444444444444444444444444444444555555555555555555555555555555555556666666666666666666666666666677777777777777777777777777777777777778888888888888888888888888888888888999999999999999999999999999999999000000000000000000000000111111111111111111111111111111111222222222222222222222222222223333333333333333333333333333333444444444444444444444444444444444444555555555555555555555555555555555556666666666666666666666666666677777777777777777777777777777777777778888888888888888888888888888888888999999999999999999999999999999999000000000000000000000000111111111111111111111111111111111222222222222222222222222222223333333333333333333333333333333444444444444444444444444444444444444555555555555555555555555555555555556666666666666666666666666666677777777777777777777777777777777777778888888888888888888888888888888888999999999999999999999999999999999000000000000000000000000111111111111111111111111111111111222222222222222222222222222223333333333333333333333333333333444444444444444444444444444444444444555555555555555555555555555555555556666666666666666666666666666677777777777777777777777777777777777778888888888888888888888888888888888999999999999999999999999999999999000000000000000000000000111111111111111111111111111111111222222222222222222222222222223333333333333333333333333333333444444444444444444444444444444444444555555555555555555555555555555555556666666666666666666666666666677777777777777777777777777777777777778888888888888888888888888888888888999999999999999999999999999999999000000000000000000000000111111111111111111111111111111111222222222222222222222222222223333333333333333333333333333333444444444444444444444444444444444444555555555555555555555555555555555556666666666666666666666666666677777777777777777777777777777777777778888888888888888888888888888888888999999999999999999999999999999999000000000000000000000000111111111111111111111111111111111222222222222222222222222222223333333333333333333333333333333444444444444444444444444444444444444555555555555555555555555555555555556666666666666666666666666666677777777777777777777777777777777777778888888888888888888888888888888888999999999999999999999999999999999");   
 
  simpleCMDs[std::string("phonegps")]=std::string("SYS:PHONE~ACT:CGPS~ACK:1");
  simpleCMDs[std::string("phonetime")]=std::string("SYS:PHONE~ACT:CTIME~ACK:1");
  simpleCMDs[std::string("photo")]=std::string("SYS:PHONE~ACT:CPHOTO(B,50%,100,100,AUTO)");
  simpleCMDs[std::string("imu")]=std::string("SYS:IMU~ACT:START");  
  simpleCMDs[std::string("light")]=std::string("SYS:LIGHT~ACT:START");  
  simpleCMDs[std::string("acktest")]=std::string("SYS:SAT~ACT:OUTPUT~ACK:0~DATA:000000000000000000000000111111111111111111111111111111111222222222222222222222222222223333333333333333333333333333333444444444444444444444444444444444444555555555555555555555555555555555556666666666666666666666666666677777777777777777777777777777777777778888888888888888888888888888888888999999999999999999999999999999999"); 
 
  simpleCMDs[std::string("irx")]=std::string("SYS:IRX1~ACT:START");  
  simpleCMDs[std::string("irxout")]=std::string("SYS:IRX1~ACT:OUTPUT~TYPE:A");  
  simpleCMDs[std::string("irxsend")]=std::string("SYS:MGR~ACT:ADD~_SYS:IRX1~_ACT:OUTPUT~TYPE:A~_INTERVAL:60000~_START:0~_STOP:1000000");  
  simpleCMDs[std::string("forwardx")]=std::string("SYS:MOTORX~ACT:SPEED~SPEED:0.2~DURATION:2000~ACK:1");  
  simpleCMDs[std::string("backwardx")]=std::string("SYS:MOTORX~ACT:SPEED~SPEED:0.2~DURATION:2000~ACK:1");  
  simpleCMDs[std::string("forwardy")]=std::string("SYS:MOTORY~ACT:SPEED~SPEED:0.2~DURATION:2000~ACK:1");  
  simpleCMDs[std::string("backwardy")]=std::string("SYS:MOTORY~ACT:SPEED~SPEED:0.2~DURATION:2000~ACK:1");  
  simpleCMDs[std::string("forwardz")]=std::string("SYS:MOTORZ~ACT:SPEED~SPEED:0.2~DURATION:2000~ACK:1");  
  simpleCMDs[std::string("backwardz")]=std::string("SYS:MOTORZ~ACT:SPEED~SPEED:0.2~DURATION:2000~ACK:1");  
  
 
  simpleCMDs[std::string("dur1")]=std::string("SYS:MOTORX~ACT:SPEED~SPEED:0.2~DURATION:2000~DATA:000000000000000000000000001111111111111111111111111111112222222222222222222222222333333333333333333333344444444444"); 

  simpleCMDs["RADIO"]=std::string("SYS:MGR~ACT:RADIO");
  simpleCMDs["MAG"]=std::string("SYS:MGR~ACT:MAG");
  simpleCMDs["MAGX"]=std::string("SYS:MGR~ACT:MAGX");
  simpleCMDs["MAGY"]=std::string("SYS:MGR~ACT:MAGY");
  simpleCMDs["MAGZ"]=std::string("SYS:MGR~ACT:MAGZ");
  simpleCMDs["PINSON"]=std::string("SYS:MGR~ACT:PINSON");
  simpleCMDs["PINSOFF"]=std::string("SYS:MGR~ACT:PINSOFF");
  simpleCMDs["PHONEON"]=std::string("SYS:MGR~ACT:PHONEON");
  simpleCMDs["PHONEOFF"]=std::string("SYS:MGR~ACT:PHONEOFF");
  simpleCMDs["BURN"]=std::string("SYS:MGR~ACT:BURN");
  simpleCMDs["IMUSPI"]=std::string("SYS:MGR~ACT:IMUSPI");
  simpleCMDs["IMU0"]=std::string("SYS:MGR~ACT:IMU0");
  simpleCMDs["IMU1"]=std::string("SYS:MGR~ACT:IMU1");
  simpleCMDs["IMU2"]=std::string("SYS:MGR~ACT:IMU2");
  simpleCMDs["I2C0"]=std::string("SYS:MGR~ACT:I2C0");
  simpleCMDs["I2C1"]=std::string("SYS:MGR~ACT:I2C1");
  simpleCMDs["I2C2"]=std::string("SYS:MGR~ACT:I2C2");
  simpleCMDs["H"]=std::string("SYS:MGR~ACT:H4");
  simpleCMDs["L"]=std::string("SYS:MGR~ACT:L4");
  simpleCMDs["W"]=std::string("SYS:MGR~ACT:PWM4");
  displayCMDs();
}

void CBaseStation::displayCMDs(){
  
  writeconsoleln();
  writeconsoleln("Commands: ");
  writeconsoleln("----------------------------------------------------------------");
  for (auto it = simpleCMDs.begin(); it != simpleCMDs.end(); it++) {    
    std::string tmpstr = "   "+it->first + ": " + it->second;
    writeconsoleln(tmpstr.c_str());
  }
  writeconsoleln("----------------------------------------------------------------");
}


std::string CBaseStation::kbloop() {
  std::string kbstring;
  if(Serial.available()){
        input = Serial.read();
        if(input =='?'){
          displayCMDs();    
          cmdstr="";
          input=' '; 
          return kbstring;
        }
        if((input !='\n')&&(input !=' ')) cmdstr+=input;              
    }

  if(input =='\n'){
    writeconsoleln(cmdstr.c_str());
    //SendCmd(cmdstr);   
    kbstring=cmdstr;
    cmdstr="";
    input=' ';
    return kbstring;
  }
  return kbstring;
}


void CBaseStation::SendCmd(std::string str){
  
    std::string cmd=simpleCMDs[str];
    if(cmd.size()>1){
      CMsg m(cmd);
      
      writeconsoleln(m.serialize());
      radio.addTransmitList(m);         
    }
    else{
      if(str.size()>1){
        CMsg m(str);
        
        radio.addTransmitList(m);                 
      }
    }      
}    

void CBaseStation::chkIRArray(){

  if(MSG->ReceivedList.size()){
  std::string d,d1,d2,d3,d4;
  CMsg b = MSG->ReceivedList.back();
  if((b.getTABLE()=="IR")&&(b.getOFFSET()=="4")){
    
    while(MSG->ReceivedList.size()){
      CMsg m=MSG->ReceivedList.front();
      MSG->ReceivedList.pop_front();
      if(m.getTABLE()=="IR"){
        if(m.getOFFSET()=="4")
          d4=m.getDATA();
        if(m.getOFFSET()=="3")
          d3=m.getDATA();
        if(m.getOFFSET()=="2")
          d2=m.getDATA();
        if(m.getOFFSET()=="1")
          d1=m.getDATA();
        if(m.getOFFSET()=="0")
          d=m.getDATA();
         }                     
    }

    
    d+=d1;
    d+=d2;
    d+=d3;    
    d+=d4;    
    MSG->ReceivedList.clear();
    int count=0;
    for(auto a:d){
      writeconsole(a);
      writeconsole(" ");
      count++;
      if(count%32==0)
        writeconsoleln("");      
      
    }
    writeconsoleln("");
   }   
  }    
}
