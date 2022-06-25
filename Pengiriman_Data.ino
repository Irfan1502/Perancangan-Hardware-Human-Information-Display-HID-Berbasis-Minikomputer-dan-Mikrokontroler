#include <ESP8266WiFi.h>                       //menggunakan library wifi
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>                  //menggunakan library web
#include <ESP8266HTTPClient.h>
#include <SPI.h>                               //define library  SPI
#include <MFRC522.h>                           //define library  RFID
 
//wifi init
const char *ssid = "HID";                              
const char *password = "12345678";                       
const String Domain = "http://192.168.185.219/TA/";  // alamat server
const String ruangweb = "elektro";
const String passwordweb = "HID_elektro";
 
#define SS_PIN D1                              // inisiasi pin yang digunakan
#define RST_PIN D0
int buzzer = D8;
int led_red = D2;
int led_green = D3;
int led_blue = D4; 
 
MFRC522 mfrc522(SS_PIN, RST_PIN);              // inisisasi RFID port
MFRC522::MIFARE_Key key;          
MFRC522::StatusCode status;
 
int blok_pemilik = 1;                          // nomor blok yang digunakan
int blok_kode = 2;
String UID,Data[3];
byte readBlockData[18]= {0};
byte bufferLen = sizeof(readBlockData);
 
bool data_terisi = false;
String kode_terdaftar[35];                     // array data tersimpan
int state_users[35];
 
void setup() {                      
  Serial.begin(115200);
  
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(buzzer, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_blue, OUTPUT);
 
  WiFi.mode(WIFI_OFF); //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA); //This line hides the viewing of ESP as wifi hotspot
  
  connect2wifi();
  inisiasi_kosong();
 
                                     
  do {                 // pengulangan jika data belum terambil
    nyala_led(219, 3, 252,0); // indikasi mengambil data
    getdata();
  }while (data_terisi == false);
  nyala_led(0,255,0,500);
    
  
  for(int i=0; i<35; i++){    // print data 
    Serial.print("data ke-"+ String(i)+" :");
    Serial.print(kode_terdaftar[i]);
    Serial.println(" " + String(state_users[i]));}
}
 
void loop(){
  if ( ! mfrc522.PICC_IsNewCardPresent()) {return;}       // cek kehadiran kartu
  if ( ! mfrc522.PICC_ReadCardSerial())   {return;}       // cek data pada kartu
  Serial.println("*Card Detected*");
 
  Baca_UID();                                             // membaca UID kartu
  Baca_Blok("pemilik",blok_pemilik);                      // membaca blok kartu
  Baca_Blok("kode",blok_kode);                        
 
  fungsi();                                                 
  
  mfrc522.PICC_HaltA();                                   // menghentikan proses enkripsi RFID
  mfrc522.PCD_StopCrypto1();
 
}
 
void connect2wifi(){
  WiFi.begin(ssid, password);
  Serial.print("Connecting");                             // menunggu koneksi
  while (WiFi.status() != WL_CONNECTED) {
   nyala_led(252, 227, 3,500);                            // nyala led kuning
   nyala_led(0, 0, 0,500);}
 
  Serial.println("");                                     // connected
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  nyala_led(0, 225, 0,2000);                                // nyala led hijau
  
   
}
 
void inisiasi_kosong(){                                     // inisiasi semua array dalam keadaan siap
  
  //inisiasi array kosong
  for(int i=0; i<=35; i++){
    kode_terdaftar[i]= "HID";
    state_users[i]= 0;
    }
}
 
void getdata(){
  HTTPClient http;                                         // deklarasi class HTTPClient
 
  http.begin(Domain +"arduino_codelist.php");              // mulai request ke alamat
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
 
  String post = "ruang=" + ruangweb + "&password=" + passwordweb ;
  int httpCode = http.POST(post);
  String payload = http.getString();                       // mengambil respon
    
  process_rawdata(payload);
  http.end();                                              // tutup koneksi
}
 
void process_rawdata(String payload){
  int pjgstr = payload.length()+1;         // inisiasi data sementara fungsi
  char chararray[pjgstr];
  String verifstr="";
  int dataterisis = 0;
  bool ambildata= false;
  bool ambildata_kode= false;
 
  payload.toCharArray(chararray,pjgstr);   // mengubah ke char array
   
  for(int i=0;i<3;i++){
   verifstr += chararray[i];}
   
  if(verifstr =="HID"){                    // mengambil data untuk verifikasi
   Serial.println(payload);
   Serial.println("data dimuat dari database");
   data_terisi = true;
   
   for(int i=0; i<=pjgstr; i++){
    
    if(chararray[i]== '"'&& ambildata==false && ambildata_kode==false){         
    ambildata= true;
    ambildata_kode=true;
    kode_terdaftar[dataterisis] = "";
    continue;}
 
    else if(chararray[i]== '"'&& ambildata==false && ambildata_kode==true){     // petik pertama untuk ambil state
    ambildata= true;
    ambildata_kode=false;
    state_users[dataterisis] = 0;
    continue;}
 
    else if(chararray[i]== '"'&& ambildata==true){  // petik kedua unutk menutup
    ambildata= false;
    }
 
    if(ambildata && ambildata_kode== true){
     kode_terdaftar[dataterisis] += chararray[i];
      }
    else if(ambildata && ambildata_kode== false){
     state_users[dataterisis] = int(chararray[i])-48;
     kode_terdaftar[dataterisis].toUpperCase();
     kode_terdaftar[dataterisis].trim();
      dataterisis += 1;}
    }
  }
  else
  data_terisi = false;
}
 
void Baca_UID(){                                    // membaca UID kartu
  UID= "";
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++){
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    UID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    UID.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
  UID.toUpperCase();
  UID.trim();
}
 
void Baca_Blok(String jenis, int blockNum){        // membaca blok kartu
  Data[blockNum] = "";                             // menyatakan string kosong
  bufferLen = sizeof(readBlockData);
  
  for (byte i = 0; i < 6; i++){                    // set kunci aktivasi
    key.keyByte[i] = 0xFF;
    }
                                                   // autentikasi enkripsi blok
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
 
  if (status != MFRC522::STATUS_OK){
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;}
  else{
//Serial.println("Authentication success");
    }
 
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen); // membaca data pada blok
  if (status != MFRC522::STATUS_OK){
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    }
  else{
    //Serial.println("Block was read successfully"); 
 
   /* Print the data read from block */
   Serial.print("\n");
   Serial.print("data pada blok:");
   Serial.print(blockNum);
   Serial.print(", "+jenis+" --> ");
   for (int j=0 ; j<16 ; j++){        
     //Serial.write(readBlockData[j]);
     Data[blockNum] += char(readBlockData[j]);            // menghimpun data
    }
    Data[blockNum].toUpperCase();
    Data[blockNum].trim();
    Serial.print(Data[blockNum]);  
  }
}
 
void fungsi(){
  bool state_terdaftar= false;
  int nmbr=0;
  Serial.print("\n");
  for(int i=0 ;i<35; i++){
    if (kode_terdaftar[i].compareTo(Data[blok_kode])){  // komperasi menggunaknan ASCII
        state_terdaftar= false; }                       // jika di kompare nilainya lebih dari 0 ma terdapat perbedaan
        
    else{
      state_terdaftar= true;
      nmbr = i;
      Data[blok_kode]=kode_terdaftar[i];                // mengubah data ke data yang sama dengan tipe data string
      break;}
  }
  output_data(state_terdaftar,nmbr,Data[blok_kode]);
}
 
void output_data(bool state_terdaftar,int nmbr, String kode){
  if(state_terdaftar){
      Serial.println("terdaftar");
      if(state_users[nmbr] ==0){
        Serial.println("masuk");                         // keadaan masuk
        state_users[nmbr] =1;
        push_data2base(1, kode);
        nyala_buzzer("masuk");
        
        }
      else if(state_users[nmbr] >= 1){
        Serial.println("keluar");                       // keadaan keluar
        state_users[nmbr] =0;
        push_data2base(0, kode);
        nyala_buzzer("keluar");
        
        }
  }
  else{
    Serial.println("tidak terdaftar");
    nyala_buzzer("salah");
  }
  Serial.print("\n");
}
 
void push_data2base(int state, String kode){
  HTTPClient http;    
  
  http.begin(Domain +"arduino_update.php");              
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");        
 
  String post = "ruang=elektro&kode="+ kode + "&state="+ String(state) ;
  //Serial.println(post);
  int httpCode = http.POST(post);
  String payload = http.getString();    
  
  Serial.println(payload);    
  http.end();  
}
 
void nyala_buzzer(String stat){
  if (stat=="masuk"){
    digitalWrite(buzzer, HIGH);
    delay(130);
    digitalWrite(buzzer, LOW);
    nyala_led(0, 255, 0,1000);
  }
  else if (stat=="keluar"){
    digitalWrite(buzzer, HIGH);
    delay(80);
    digitalWrite(buzzer, LOW);
    delay(50);
    digitalWrite(buzzer, HIGH);
    delay(80);
    digitalWrite(buzzer, LOW);
    nyala_led(0, 0, 255,1000);
    ;
  }
  else if (stat=="salah"){
    digitalWrite(buzzer, HIGH);
    nyala_led(255, 0, 0,0);
    delay(850);
    digitalWrite(buzzer, LOW);
    nyala_led(0, 0, 0,0);
  }
}
 
void nyala_led(int r, int g, int b, int tunggu){
  analogWrite(led_red,r);
  analogWrite(led_green,g);
  analogWrite(led_blue,b);
  if (tunggu != 0){
    delay(tunggu);
    analogWrite(led_red,0);
    analogWrite(led_green,0);
    analogWrite(led_blue,0);
  } 
}
 
void error(int i){
  
}
