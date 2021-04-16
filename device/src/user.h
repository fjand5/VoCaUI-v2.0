#pragma once
#define USER_FILE "user.txt"

#include <LittleFS.h>
#include <map>

// tên, (mật khẩu, quyền)
std::map < String, String > UserContent;

void setupConfig() {
  if (!LittleFS.begin()) {
    return;
  }
  File usr_file = LittleFS.open(USER_FILE, "r");
  if (usr_file) {
    String tmp = usr_file.readString();
    loadFileIntoUser(tmp);
  }
  usr_file.close();
}

// User viết HOA chữ đầu mà master
// Cấu trúc dòng [user]:[pass]
// permision càng thấp thì quyên càng cao, cao nhất là 0
void loadFileIntoUser(String content) {
  while (content.indexOf("\n") >= 0) {
    String curLine = content.substring(0, content.indexOf("\n"));
    String user = curLine.substring(0, content.indexOf(":"));
    String pass = curLine.substring(content.indexOf(":") + 1);
    UserContent[user] = pass;
    content.remove(0, curLine.length() + 1);
  }
}
bool saveUserContent() {
  // lưu vào file
  File usr_file = LittleFS.open(USER_FILE, "w");
  if (!usr_file) {
    usr_file.close();
    return false;
  }

  for (std::pair < String, String > e : UserContent) {
    String k = e.first;
    String v = e.second;
    usr_file.print(k + ":" + v + "\n");
  }
  usr_file.close();
  return true;
}
bool hasMaster() {
  for (auto const& x : UserContent)
  {
    // nếu có bất cứ user nào là master thì return true luôn
    if (isUpperCase(x.first[0]))
      return true;

  }
  return false;
}

// Lấy thông tin master
std::pair < String, String > getMaster() {
  std::pair < String, String > ret;
  for (std::pair < String, String > e : UserContent) {
    String k = e.first;
    if (isUpperCase(e.first[0])) {
      ret = e;
      break;
    }
  }
  return ret;
}
// xác thực
bool authUser(ESP8266WebServer* server) {
  for (std::pair < String, String > x : UserContent)
  {
    if (server->authenticate(x.first.c_str(), x.second.c_str()))
      return true;

  }
  return false;
}

// Đăng ký người dùng
bool registerUser(String user, String pass) {

  // Nếu user đã tồn tại thì return false
  if (UserContent.find(user) != UserContent.end())
    return false;

  // Nếu đăng ký user master mà đã có user master thì riểm tra có phải đang muốn đăng ký master không
  // bằng cách kiểm tra ký tự đầu của user có phải viết hoa không
  if (isUpperCase(user[0]) // Nếu muốn đăng ký master
      && hasMaster()) {   // và đã có master
    return false;
  }

  // Đến đây thì có thể lưu vào bộ nhớ
  // lưu vào ram
  UserContent[user] = pass;
  return saveUserContent();
}


// Thay đổi thông tin người dùng
/*
  Bài test:
  - Dùng tài khoản Master để thay đổi tất cả => đã test với chính Master và 1 guest: OK
  - Dùng tài khoản guest để thay đổi chính nó =>  đã test : OK
  - Dùng tài khoản guest để thay đổi tất cả =>  chưa test (hạn chế ở font-end => làm biếng)
  - Dùng tài khoản guest để thay đổi Master => chưa test (hạn chế ở font-end => làm biếng)
*/
bool editUser(String curUser, String curPass, String distUser, String newPass) {
  // Kiểm tra curent có phải là master hay không
  // Nếu là master thì được đổi mật khẩu tất cả

  std::pair < String, String > master = getMaster();
  if (master.first == curUser
      && master.second == curPass ) {
    for (std::pair < String, String > e : UserContent) {

      if (e.first == distUser) {
        UserContent[distUser] = newPass;
        return saveUserContent();
      }
    }
    // Nếu không tìm thấy người dùng trong dữ liệu thì return false.
    return false;
  }


  // Ngược lại (không phải master) thì chỉ được thay đổi mật khẩu chính mình
  for (std::pair < String, String > e : UserContent) {
    if (e.first == curUser
        && e.second == curPass
        && curUser == distUser) {
      UserContent[distUser] = newPass;
      return saveUserContent();
    }
  }
  return false;

}
String listUser() {
  String ret;
  for (std::pair < String, String > e : UserContent) {
    ret+=e.first +"\n";
  }
  return ret;
}
////////////////////////////////////////////////////////////////////////////////////
