/*
 * 
 * 全部代码按顺序分为三个部分：
 * 1.引用库，定义各种数据
 * 2。setup以及loop函数
 * 3.函数的集中定义
 * 
 * 我看别人的代码，解释的都一知半解，让我一个初学者烦恼了半个月，我清晰的做了标注，当然，一些理念需要自己参透，我尽力使代码看起来简单了
 * 
 * 
 */
//这里引入的库前两个是获取当前时间的，第三个是连接wifi的，第四、五个是控制OLED，最后一个发送和接收HTTP包要用
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <ArduinoJson.h>

 
//以下三个定义为调试定义，有一部分代码是我改的CSDN前辈的，这里我就不改了，因为后面实在太多，一改都逮改，说白了就是把Serial的几个函数换个说法
#define DebugBegin(baud_rate)    Serial.begin(baud_rate)
#define DebugPrintln(message)    Serial.println(message)
#define DebugPrint(message)    Serial.print(message)

 //这里是一些需要定义的信息
const char* ssid     = "PRO20";         // 这是我的wifi名称
const char* password = "2020beiyou";         // 这是wifi密码
const char* host = "api.heclouds.com";     //这是api的地址
const char* APIKEY = "7cn4RQ2nAnRluXyDXSarRtD=buA=";        //这是API KEY

const unsigned long BAUD_RATE = 115200;                   // 设置串口
const unsigned long HTTP_TIMEOUT = 5000;               // 超时时间
const size_t MAX_CONTENT_SIZE = 1000;                   // 返回内容最大长度

//这里定义一个整型数，是为了防止震动模块一直响
int s = 10;
 
// 在这里定义一个结构，以便存储缓存区数据
struct Jishiben {
  char date[20];//日期
  char thing[20];//事件
};
Jishiben jishiben;

//这里是请求当前时间的一个东西，具体我没再深入发掘，这是一个库里的函数
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp1.aliyun.com",60*60*8, 30*60*1000);

//这个是个简化wificlient
WiFiClient client;

//定义两个数组，第一个是接受回复信息的，第二个后面跳过HTTP开头的函数会用，就是找到两个换行符才开始读数据
char response[MAX_CONTENT_SIZE];
char endOfHeaders[] = "\r\n\r\n";

//这里定义OLED打印的接口
U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ D5, /* data=*/ D6, /* reset=*/ U8X8_PIN_NONE); 


//正式开始
void setup() {
  //按钮的接口
  pinMode(D8, INPUT);
  
  //以下是对wifi的一些调试
  WiFi.mode(WIFI_STA);     //设置esp8266 工作模式
  DebugBegin(BAUD_RATE);
  DebugPrint("Connecting to ");//写几句提示，哈哈
  DebugPrintln(ssid);
  WiFi.begin(ssid, password);   //连接wifi
  WiFi.setAutoConnect(true);
  
  //这个函数是wifi连接状态，返回wifi链接状态
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DebugPrint(".");
  }
  DebugPrintln("");
  DebugPrintln("WiFi connected");
  delay(500);
  DebugPrintln("IP address: ");
  //WiFi.localIP()返回8266获得的ip地址
  DebugPrintln(WiFi.localIP());
  client.setTimeout(HTTP_TIMEOUT);
  
  //这里是请求当前时间的一个东西，具体我没再深入发掘，这是一个库里的函数
  timeClient.begin();
  
  //定义震动模块接口
  pinMode(D3, OUTPUT);
  
  //打印开始
   u8g2.begin();
}


 //重头戏
void loop() {
  //控制s的值
  if(s>0){
    s = s - 1;
    }
  
  //这里是请求当前时间的一个东西，具体我没再深入发掘，这是一个库里的函数
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
  String T = timeClient.getFormattedTime();
  
  //在OLED上打印
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawFrame(3,5,120,55);
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.setCursor(23, 40);
    u8g2.print(T);
  } while ( u8g2.nextPage() );
    
  //判断tcp client是否处于连接状态，不是就建立连接
  while (!client.connected()){
     if (!client.connect(host, 80)){
         DebugPrintln("connection....");
         delay(200);
     }
  }
  
  //发送http请求 并且跳过响应头 直接获取响应body，这几个函数在下面写好了，这里加一个按钮，想接收的时候按按钮，并显示一下数据，就不让他一直发了
  //另外要说一句的是我这里只请求了两组数据，想要更多可以复制几次，但是就项目考虑，为使它简洁易懂一点，我就不再重复那么多次了，还要再一次分配空间，再一次存数据，重复工作，想来蛮没意思
  //项目焊接好以后可以再上传，可以很便捷的优化，更改，考虑到这么几点，就这样吧
   if(digitalRead(D8) == HIGH) {
     if (sendRequest(host, APIKEY) && skipResponseHeaders()) {
      //清除缓冲
      clrEsp8266ResponseBuffer();
      //读取响应数据
      readReponseContent(response, sizeof(response));
      if (parseUserData(response, &jishiben)) {
       printUserData(&jishiben);
     }
   }
   //从屏幕打印一下
   u8g2.clearDisplay();
   u8g2.firstPage();
   do {
     u8g2.setFont(u8g2_font_ncenB10_tr);
     u8g2.drawFrame(3,5,120,55);
     u8g2.setFont(u8g2_font_ncenB14_tr);
     u8g2.drawStr(23, 40,jishiben.thing);
   } while ( u8g2.nextPage() );
   delay(4000);
  }

  
  //判断控制震动并打印事件，中文库太大，NodeMCU下载不下来，就用英文吧，刚好练一练大家的英语水平
  if(jishiben.date[0]==T[0]&&jishiben.date[1]==T[1]&&jishiben.date[3]==T[3]&&jishiben.date[4]==T[4]&&s==0){
  zhendong();
  u8g2.clearDisplay();
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawFrame(3,5,120,55);
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(23, 40,jishiben.thing);
  } while ( u8g2.nextPage() );
  s = 100;
  delay(5000);
  }
  delay(500);
}



/*
 * 
 * 
 * 下面是函数的集中定义，我都做了标注，阅读起来不会很费劲，这里特别感谢CSDN里的前辈，借鉴了一些相似的代码，我也做了我自己的理解
 * 
 * 
 */


 
//控制震动的函数，D3输出
void zhendong(){
  digitalWrite(D3, HIGH);  
  delay(1000);                       
  digitalWrite(D3, LOW);   
  delay(1000); 
  digitalWrite(D3, HIGH);  
  delay(1000);                       
  digitalWrite(D3, LOW);   
  delay(1000);    
  digitalWrite(D3, HIGH);  
  delay(1000);                       
  digitalWrite(D3, LOW);   
  delay(1000);                
  }


 
//发送HTTP请求的函数，重中之重，具体报文怎么写需要看文档，但是HTTP协议的大致就这样，注意一下，我下面这个设备ID直接写这了，本来该在上面定义的
bool sendRequest(const char* host, const char* apiKey) {
  
  //这里写一下URL
  String GetUrl = "/devices/652113157/datastreams";
  
  //在这里发送给服务器请求的指令，过一会就能在串口看到返回信息了
  client.print(String("GET ") + GetUrl + " HTTP/1.1\r\n" +
               "api-key: " + apiKey + "\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
               
  //这里打印一下发送的东西
  DebugPrintln("create a request:");
  DebugPrintln(String("GET ") + GetUrl + " HTTP/1.1\r\n" +
               "api-key " + apiKey + "\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n");
  delay(100);
  return true;
}
  



//跳过 HTTP 头，使我们在响应正文的开头，就是找到换行符
bool skipResponseHeaders() {
  bool ok = client.find(endOfHeaders);
  if (!ok) {
    DebugPrintln("No response or invalid response!");
  }
  return ok;
}



  
 //从HTTP服务器响应中读取正文，这个基本不用变
void readReponseContent(char* content, size_t maxSize) {
  size_t length = client.readBytes(content, maxSize);
  delay(100);
  DebugPrintln("Get the data from Internet!");
  content[length] = 0;
  DebugPrintln(content);
  DebugPrintln("Read data Over!");
  
  //清除一下缓冲
  client.flush();
}


  
//这里需要调整一下缓冲区的大小，以便完整的接收到数据，注意ArduinoJson.h必须是V5（我吃了大亏，不要试着改为V6，我已踩坑）
bool parseUserData(char* content, struct Jishiben* jishiben) {
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(content);
   
  if (!root.success()) {
    DebugPrintln("JSON parsing failed!");
    return false;
  }
    
  //将有用信息存入结构体中
  strcpy(jishiben->date, root["data"][0]["current_value"]);
  strcpy(jishiben->thing, root["data"][1]["current_value"]);
  
  return true;
}
   
// 从串口打印从JSON中提取的数据，比较简单，不解释
void printUserData(const struct Jishiben* jishiben) {
  DebugPrintln("Print parsed data :");
  DebugPrint("data : ");
  DebugPrint(jishiben->date);
  DebugPrint("\n");
  DebugPrint("thing : ");
  DebugPrint(jishiben->thing);
  DebugPrint("\n");
}
   
// 关闭与HTTP服务器连接
void stopConnect() {
  DebugPrintln("Disconnect");
  client.stop();
}
  
void clrEsp8266ResponseBuffer(void){
  //清空
    memset(response, 0, MAX_CONTENT_SIZE);      
}
