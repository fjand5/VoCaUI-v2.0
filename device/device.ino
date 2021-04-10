/*
  Đây là ví dụ cơ bản có chức năng đơn giản là điều khiển bật, tắt led trên mạch.
  Ví dụ nâng cao hơn và tài liệu hướng dẫn sẽ được bổ sung sau.
*/
#include "example.h"
/*
  Các bước thực hiện
  1. Nạp chương trình vào mạch (ESP8266 node)
    *Lưu ý: phải mở bộ nhớ flash cho hệ thống. Vào Tools/Flash SiZe, chọn 4MB(FS:2MB OTA:~1019KB)
    *Nên chọn Upload Speed là 921600 để nạp chương trình nhanh hơn.
  2. Dùng điện thoại hoặc máy tính kết nối vào wifi do mạch phát ra (id: vocaui2, password: 12345678)
    *Lưu ý: tắt kết nối 3G/4G nếu dùng điện thoại.
  3. Dùng trình duyệt web đến địa chỉ 192.168.4.1.
  4. Đến Tab SYSTEM, điền thông tin wifi vào mục Station wifi và password. 
    *Lưu ý: điền xong mục nào thì nhấn nút OK ở mục đó.
  6. Bấm vào nút Thử kết nối để kết nối đến trạm phát wifi.
  Đợi đến khi địa chỉ IP xuất hiện ở góc phải bên trên.
  Nếu sau 30s mà chưa xuất hiện địa chỉ IP, kiểm tra lại thông tin wifi và mật khẩu.
  Địa chỉ này do trạm phát cấp;
  bạn có thể dùng địa chỉ này để truy cập vào mạch trong mạng của trạm phát.
Sản phẩm đang trong quá trình thử nghiệm, mọi góp ý về giao diện và tính năng vui lòng góp ý tại trang FaceBook Ngôi Nhà IoT. Cảm ơn bạn đã sử dụng.
*/
