// 해당 코드는 바코드리더기 GM95를 사용한 것이 아닌 MH-ET Live Scanner 3.0V을 이용한 코드입니다.
// <주의 사항>
// sd카드 모듈에 삽입할 sd카드를 고를때 16Gb 혹은 32Gb 제품만 쓰세요.
// 아두이노는 FAT16 과 FAT32만 인식하기에 만약 32GB이상의 sd카드를 삽입시 인식이 되지않습니다.
// 또한 SD카드 모듈은 반드시 5.5V에 전원을 꼽으셔야만 sd카드가 인식이 됩니다.

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include <SoftwareSerial.h>

// 설정
const int barcodeRxPin = 3; // 바코드 리더기 RX 핀
const int barcodeTxPin = 2; // 바코드 리더기 TX 핀
const int sdCardCSPin = 4;  // SD 카드 CS 핀

SoftwareSerial barcodeSerial(barcodeRxPin, barcodeTxPin); // 바코드 변수를 정한다
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD 설정 (주소, 열, 행)

void setup() {
  SPI.begin();        // sd카드를 사용하기 위해 SPI 통신을 초기화
  Serial.begin(9600); // 시리얼 모니터를 사용하기 위해 Serial통신 초기화
  Wire.begin();       // lcd_i2c 사용을 위해 wire 통신 초기화
  barcodeSerial.begin(9600); // 바코드와 통신하기 위해 바코드 함수 초기화
  lcd.init();         // lcd 화면을 초기화
  lcd.backlight();    // lcd의 백라이트를 킴
  lcd.begin(16, 2);   // lcd의 크기를 지정

  // 만약 sd카드 모듈에 sd카드가 인식이 안되면 lcd에 "can't find a SD card!"라는 문구를 띄움
  if (!SD.begin(sdCardCSPin)) { 
    lcd.setCursor(0, 0);
    lcd.print("can't find a");
    lcd.setCursor(0, 1);
    lcd.print("SD card!");
    while (1);
  }
  // sd카드가 정상적으로 인식되었다면 "system is ready"라는 문구를 2초간 띄움
  // sd카드 모듈은 무조건 5.5v에 연결되어어야 인식이 가능하다. 
  lcd.setCursor(0, 0);
  lcd.print("system is ready");
  delay(2000);
  lcd.clear(); // lcd화면상에 있는 모든 글자를 지운다
}


String barcode = ""; // 바코드의 숫자를 받기 위해 빈 변수를 생성
String productNames[] = {"Tape", "SD card"}; // 제품명을 기입하는 리스트이다
String productCodes[] = {"8801230400095", "8806092397552"}; // 바코드의 일련번호를 기입하는 리스트이다.
int productVale[] = {3000, 9000}; // 해당 제품의 가격을 기입하는 리스트이다.
int totalprice = 0;               // 최종 가격을 저장하는 변수이다.

void loop() {
  if (barcodeSerial.available()) {   // 만약 바코드리더기를 사용해서 수신된 입력이 있다면
    char ch = barcodeSerial.read();  // 바코드리더기로 읽은 데이터를 ch변수에 char형으로 저장
    Serial.print(ch); // 시리얼 모니터에 출력
    if (ch == '\n') { // 바코드 데이터 수신 완료
      Serial.print("sd card = ");    // 시리얼 모니터에 "sd card = "라는 문구를 출력
      Serial.println(barcode);       // 시리얼 모니터에 barcode변수에 저장된 값을 출력
      Serial.println(barcode.length()); // barcode변수에 저장된 값의 길이를 시리얼 모니터에 출력
      String data = barcode.substring(0, 13); // barcode변수에 저장되는 값의 길이를 13으로 제한 그 정보를 data변수에 저장
      Serial.println(data);                   // data변수에 저장된 값을 시리얼 모니터에 출력
      lcd.setCursor(0, 0);                    // lcd의 커서를 0번째칸에 0번째 줄로 설정
      lcd.println("                   ");     // lcd의 0번째 줄의 화면을 초기화
      lcd.setCursor(0, 1);                    // lcd의 커서를 0번째칸에 1번째 줄로 설정
      lcd.println("                   ");     // lcd의 1번째 줄의 화면을 초기화
      
      if (data.equals(productCodes[0]))       // 만약 productCodes의 0번째 값이 들어온다면
      {
        totalprice = totalprice + productVale[0];  // totalprice변수에 productVale의 0번째 값이 합산 됨 
        lcd.setCursor(0, 0);                       
        lcd.println("Goods:");                     // lcd에 "Goods:"라는 문구가 출력 됨
        lcd.println(productNames[0]);              // Goods 옆에 0번에 있는 제품의 이름이 출력됨
        lcd.setCursor(0, 1);              
        lcd.println("total:"); lcd.println(totalprice); // 바로 밑줄에 "total:"이라는 문구가 나오면서 totalprice에 저장된 값이 lcd에 출력된다.
      }
      
      if (data == productCodes[1])   // 만약에 data에 들어온 값이 productCodesdes의 1번의 값과 같으면
      {
        totalprice = totalprice + productVale[1];  // totalprice에 productVale의 1번째에 저장된 값을 totalprice에 합산
        lcd.setCursor(0, 0);                       
        lcd.println("Goods:");
        lcd.println(productNames[1]);
        lcd.setCursor(0, 1);
        lcd.println("total:");
        lcd.println(totalprice);
      }
      Serial.println("totalprice");
      writeToSDCard(barcode); // SD 카드에 저장
      //displayOnLCD(barcode);  // LCD에 출력
      barcode = "";           // 바코드 초기화
    }
    
    else {
      barcode += ch;  // barcode = barcode + ch, 그렇지 않으면 barcode값에 ch값을 추가, ch변수는 50번째 줄에 있음
    }
  }
}

void writeToSDCard(const String &barcode) {  // sd카드에 barcode의 값을 저장하기 위한 전역함수이다.
  File file = SD.open("barcodes.txt", FILE_WRITE); // sd카드의 파일을 열어 값을 저장하기 위한 준비를 한다.
  if (!file) {                                     // 만약 파일이 열리 않으면
    lcd.setCursor(0, 0);      
    lcd.print("faild often file");                 // lcd에 "faild often file"문구를 2초 동안 출력
    delay(2000);
    return;                                        // 다시 돌아감
  }

  file.println(barcode);                           // 파일에 barcode변수의 값이 쓰여짐

  file.close();                                    // 값이 쓰여진 파일을 닫음
}

void displayOnLCD(const String &barcode) {         // 바코드의 정보를 띄어주기 위해 만든 전역변수이다.
  lcd.clear();                                     // lcd의 화면을 초기화한다.
  //lcd.begin(16, 2);     
  lcd.setCursor(0, 0);        
  lcd.print("barcord:");                           // lcd의 0번째 칸의 0번째 줄에 "barcord:"라는 문구를 출력한다.
  lcd.setCursor(0, 1);
  lcd.print(barcode);                              // lcd의 0번째 칸의 1번째 줄에 barcode의 값을 출력한다.
  delay(5000); // 바코드 정보를 5초 동안 표시
}
