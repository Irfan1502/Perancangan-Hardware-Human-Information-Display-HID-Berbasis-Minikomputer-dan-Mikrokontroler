#include <SPI.h>                           //define library dan port
#include <MFRC522.h>
 
#define SS_PIN D1  
#define RST_PIN D0  
 
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;          
MFRC522::StatusCode status;
 
byte data_pemilik [16] = {"SYAMSYARIF B"}; //isikan data max 16 char
byte data_kode [16] = {"HID802720516582"};
 
int blok_pemilik = 1;                      // init penggunaan
int blok_kode = 2;
String UID,Data[3];
 
byte readBlockData[18]= {0};
byte bufferLen = sizeof(readBlockData);
 
 
void setup() {                             // setup
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
 
  for (byte i = 0; i < 6; i++){            // set kunci aktivasi
    key.keyByte[i] = 0xFF;
    }
  Serial.println("Tap Untuk Menulis");
}
 
void loop(){
  if ( ! mfrc522.PICC_IsNewCardPresent()) {return;}   // cek kehadiran kartu
  if ( ! mfrc522.PICC_ReadCardSerial())   {return;}   // cek data pada kartu
  Serial.println("*Card Detected*");
 
  Tulis_Blok(data_pemilik,blok_pemilik);
  Tulis_Blok(data_kode,blok_kode);
  Baca_UID();                                         // membaca UID kartu
  Baca_Blok("pemilik",blok_pemilik);                  // membaca blok kartu
  Baca_Blok("kode",blok_kode);                        // membaca blok kartu
 
  finish();
}
 
void Tulis_Blok(byte blockData[],int blockNum) 
{
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK){
    Serial.print("Authentication failed for Write: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
    }
  else{
    //Serial.println("Authentication success");
    }
 
  
  /* Write data to the block */
  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK){
    Serial.print("Writing to Block failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else{
    Serial.println("penulisan data berhasil pada blok :"+ String(blockNum) );
    }
  
}
 
void Baca_UID(){                                        // membaca UID kartu
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
 
void Baca_Blok(String jenis, int blockNum)                // membaca blok kartu
{
  Data[blockNum] = "";                          // menyatakan string kosong
  bufferLen = sizeof(readBlockData);
 
                                                // autentikasi enkripsi blok
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
 
  if (status != MFRC522::STATUS_OK){
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;
    }
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
     Data[blockNum] += (char)readBlockData[j];
    }
    Data[blockNum].trim();
    Serial.print(Data[blockNum]);  
  }
}
 
void finish(){                                      // menghentikan enkripsi
  // Halt PICC
    mfrc522.PICC_HaltA();
   // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}
