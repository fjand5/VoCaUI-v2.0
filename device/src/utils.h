#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
// Cấu trúc: [tên biến]:[x0];[y0][tab][x1];[y1][tab]...[xn];[yn][tab]
// Thêm giá trị vào data
String IpAddress2String(const IPAddress& ipAddress)
{
    return String(ipAddress[0]) + String(".") +
           String(ipAddress[1]) + String(".") +
           String(ipAddress[2]) + String(".") +
           String(ipAddress[3]);
}
String addData(String data, float val, int maxSize = 30){
  String ret;

  // Lấy số lượng dữ liệu
  int lastIndex=0;
  int count=0;
   while ((lastIndex=data.indexOf("\t",lastIndex+1)) >= 0) {
    count++;
  }
  // Nếu dữ liệu nhiều hơn maxSize thì xóa dữ liệu đầu
  if(count>=maxSize){
    data = data.substring(data.indexOf("\t")+1);
  }
  // Trừ tất cả tọa độ X đi 1 đơn vị
  
  // Thêm dữ liệu vào cuối
    // Lấy chỉ số cuối (tọa độ y)
  int lastY = data.substring(data.lastIndexOf("\t",data.length()-2)).toInt();
  lastY++;
  data = data+lastY+";"+val+"\t";
  return data;
}
// Stream dữ liệu từ bộ nhớ
// class ProgMemVar: public Stream {
//   private:
//     char* _addrMem;
//     int _len = 0;
//     int _availableLen = 0;
//   public:
//     ProgMemVar(char* addrMem) {
//       _addrMem = addrMem;

//       while (pgm_read_byte_near(_addrMem + _len)) {
//         _len++;
//       }
//       _availableLen = _len;
//     };
//     uint32_t length() {
//       return _len;
//     };
//     int available(void)
//     {
//       return _availableLen;
//     };
//     int read() {
//       int ret;
//       if (_availableLen > 0)
//         ret = pgm_read_byte_near(_addrMem + _len - _availableLen--);
//       else
//         ret = -1;

//       return ret;
//     };
//     int peek() {
//       int ret;
//       if (_availableLen > 0)
//         ret = pgm_read_byte_near(_addrMem + _len - _availableLen);
//       else
//         ret = -1;

//       return ret;
//     };
//     String readString() {
//       String ret;
//       for (int i = (_len - _availableLen); i < _len; i++) {
//         ret += char(pgm_read_byte_near(_addrMem + i));
//       }

//       return ret;
//     };
//     size_t readBytes(char *buffer, size_t length) {
//       size_t got = 0;

//       for (int i = (_len - _availableLen); i < (_len - _availableLen + length); i++) {
//         if (i > _len)
//           return got;
//         got++;
//         buffer[i] = char(pgm_read_byte_near(_addrMem + i));
//       }
//       return got;
//     };
//     size_t write(const uint8_t *buffer, size_t size) {
//       return 0;
//     };
//     size_t write(uint8_t data) {
//       return 0;
//     };
//     void flush() {};
// };
