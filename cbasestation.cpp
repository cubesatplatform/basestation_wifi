#include "cbasestation.h"


bool CBaseStation::isCmdTime(){
     if(millis() - lastCmd >= (unsigned long)CMD_INTERVAL * (unsigned long)1000)  {
         lastCmd=millis();
         return true;
     }
     return false;
}

std::string CBaseStation::getField(){
  long ct=millis();
  std::string str;
  
  while(millis()<ct+20000){
    str=kbloop();
    if(str.size())
      break;
  }

  return str;
}

void CBaseStation::addWifi(){
  std::string ssid;
  std::string pwd;
  
  writeconsoleln("______________________________________");
  writeconsole("Please enter Network: ");
  ssid=getField();
  writeconsoleln(ssid);
  writeconsole("Please enter Password: ");
  pwd=getField();
  writeconsoleln(pwd);
  writeconsoleln("______________________________________");

  if(ssid.size()&&pwd.size())
    wifiMulti.addAP(ssid.c_str(),pwd.c_str());
  
}

void CBaseStation::connectWifi() {
  Serial.begin(serialBaudRate);

  writeconsole("Starting BaseStation                          -----------------             Connecting to wifi: ");
  // You can add the id and password for several WiFi networks
  // it will connect to the first one it sees.
  wifiMulti.addAP("Truffle1", "Alexander1");
  wifiMulti.addAP("Cabana", "alexander");

  while (wifiMulti.run() != WL_CONNECTED) {
    delay(delayWaitingForWifiConnection);
    writeconsole(".");
  }
  writeconsoleln("");
  writeconsole("WiFi connected to ");
  writeconsole(WiFi.SSID());
  writeconsole(" with IP ");
  writeconsoleln(String(WiFi.localIP()));
}


void CBaseStation::insertSatData(CMsg &msg){

  std::string str=msg.serialize();
  str=urlencode(str);
  writeconsoleln("---------------------------------------------------");  writeconsole("Inserting Data:<");  writeconsole(str.c_str());  writeconsoleln(">");  writeconsoleln("---------------------------------------------------");
  client.setTimeout(10000);
  if (!client.connect(host, httpsPort)) {
    writeconsoleln("Connection failed");
  }
  else{
    std::string str1=std::string("GET ")+logpath+str+" HTTP/1.1\r\n" +"Host: " + host + "\r\n" +"User-Agent: SeaKing\r\n" +"Connection: close\r\n\r\n";
    client.print(str1.c_str());    
    
    writeconsoleln(str1.c_str());
    client.stop();
  }
}


void CBaseStation::updateAllReceivedCommands(){
if (!MSG->ReceivedList.size()) return;
int count=0;
writeconsoleln("updateAllReceivedCommands()  {");
while(MSG->ReceivedList.size()){
    CMsg msg=MSG->ReceivedList.front();
    if (msg.getACK()=="1") {
        updateAckCmd(msg);
      }
      else{ 
       insertSatData(msg);          
      }
            
    MSG->ReceivedList.pop_front();
    count++;
    if(count>10) break;
  }
 writeconsoleln("}END    updateAllReceivedCommands()");  
}


void CBaseStation::updateAllTransmittedCommands(){
int count=0;
while(MSG->TransmittedList.size()){
  CMsg msg=MSG->TransmittedList.front();

  
  updateTransmittedCmd(msg);

  MSG->TransmittedList.pop_front();
  count++;
  if(count>10) break;
  }
}

void CBaseStation::updateTransmittedCmd(CMsg &msg){
  std::string cid=msg.getCID();
 
  writeconsole("Transmit Update: ");
  writeconsoleln(cid.c_str());

  
  client.setTimeout(10000);
  if (!client.connect(host, httpsPort)) {
    writeconsoleln("Connection failed");
  }
  else{
    std::string str1=std::string("GET ")+updatecmdpath+cid+" HTTP/1.1\r\n" +"Host: " + host + "\r\n" +"User-Agent: SeaKing\r\n" +"Connection: close\r\n\r\n";
    client.print(str1.c_str());

      // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    writeconsole("Unexpected response: ");
    writeconsoleln(status);
    return;
  }  
  client.stop();
  }
}


void CBaseStation::updateAckCmd(CMsg &msg){
  if(msg.getACK()!="1")
    return;
  
  std::string cid=msg.getCID();  
  if (cid.length()<1) return;

  writeconsoleln();
  writeconsole("Transmit Update: ");
  writeconsoleln(cid.c_str());

  client.setTimeout(10000);
  if (!client.connect(host, httpsPort)) {
    writeconsoleln("Connection failed");
  }
  else{
    std::string str1=std::string("GET ")+updateackpath+cid.c_str()+" HTTP/1.1\r\n" +"Host: " + host + "\r\n" +"User-Agent: SeaKing\r\n" +"Connection: close\r\n\r\n";
    client.print(str1.c_str());

      // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    writeconsole("Unexpected response: ");
    writeconsoleln(status);
    return;
  }
  client.stop();
  }
}


void CBaseStation::getEchoData(const char * strdata=""){
 std::string str=strdata;
 str=urlencode(str);
  client.setTimeout(10000);
  if (!client.connect(host, httpsPort)) {
    writeconsoleln("Connection failed");
  }
  else{
    std::string str1=std::string("GET ")+consolelogpath+str+" HTTP/1.1\r\n" +"Host: " + host + "\r\n" +"User-Agent: SeaKing\r\n" +"Connection: close\r\n\r\n";
    client.print(str1.c_str());  
    client.stop();
  }
}

void CBaseStation::getLastCommand() {
  long counter=0,endt=0,startt=millis();
  StaticJsonDocument<500> doc;
  client.setTimeout(10000);
  client.setInsecure();
  if (!client.connect(host, httpsPort)) {
    writeconsoleln("Connection failed");
    writeconsoleln(host);
    writeconsoleln(httpsPort);
    delay(2000);
    writeconsoleln("Restarting");
    connectWifi();
  }
  else{
    std::string str1=std::string("GET ")+cmdpath+" HTTP/1.1\r\n" +"Host: " + host + "\r\n" +"User-Agent: SeaKing\r\n" +"Connection: close\r\n\r\n";
    writeconsoleln(str1.c_str());
    client.print(str1.c_str());

    while (client.connected()) {
      counter++;       
      char endOfHeaders[] = "\r\n\r\n";  // Skip HTTP headers
      if (!client.find(endOfHeaders)) {  //May return nothing
        return;
      }
      String Stmp= client.readStringUntil('\n');
      std::string line=Stmp.c_str();
      if(line.length()<3) {
        break;
      }
      line=line.substr(1,line.length()-1);  //Gets rid of []   This JSon doesnt like it
      
      DeserializationError error = deserializeJson(doc, line);
    if (error)
      {
        writeconsole("deserializeJson() failed: ");
        writeconsoleln(error.f_str());
        return;
      }  
    else
      {    
        if(line.length()>2)  {
          writeconsoleln(line.c_str());
          std::string datastr=doc["data"];
          
          CMsg msg(datastr);
          msg.setCID(doc["cid"]);
          writeconsoleln(msg.serialize().c_str());
          radio.addTransmitList(msg);   
        }     
      }
      if (line == "\r") {
       // break;
      }
    }
  endt=millis();
  writeconsoleln();  writeconsole(counter);  writeconsole("Get Time: ");  writeconsoleln((float)((endt-startt)/1000.0));
  client.stop();
  } 
}
