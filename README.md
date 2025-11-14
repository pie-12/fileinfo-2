# FileInfo - Trình quản lý file TUI đơn giản

Một trình quản lý file giao diện dòng lệnh (Text User Interface - TUI) đơn giản được viết bằng C và thư viện ncurses, lấy cảm hứng từ `ranger`.

## Giao diện

Chương trình có giao diện 3 cột quen thuộc:

```
+------------------------+------------------------+----------------------------+
| Parent                 | /home/user/repo        | Preview                    |
|                        |                        |                            |
| ..                     | > .git/                  | File: .git                 |
| project-1/             |   .gitignore           |                            |
| project-2/             |   README.md            | Permissions: drwxr-xr-x   |
|                        |   Makefile             | Type:       Directory      |
|                        |   main.c               | Size:       4096 bytes     |
|                        |                        |                            |
|                        |                        | Modified:   Nov 13 20:10   |
|                        |                        |                            |
+------------------------+------------------------+----------------------------+
```

## Tính năng

- Giao diện 3 cột: Thư mục cha, thư mục hiện tại, và xem trước thông tin.
- Điều hướng bằng các phím mũi tên.
- Xem trước thông tin chi tiết của file/thư mục (quyền, kích thước, ngày sửa đổi).
- Hỗ trợ bật/tắt hiển thị các file và thư mục ẩn.

## Cơ sở lý thuyết

Dự án này được xây dựng dựa trên một số khái niệm và thư viện lập trình cơ bản, tạo nên một ứng dụng giao diện người dùng văn bản (TUI) hiệu quả và nhẹ.

1.  **Giao diện người dùng văn bản (TUI - Text-based User Interface):**
    *   Ứng dụng được thiết kế như một TUI, cung cấp trải nghiệm tương tác trực quan ngay trong môi trường terminal. Cách tiếp cận này không yêu cầu môi trường đồ họa (GUI), giúp chương trình chạy nhanh, tiêu thụ ít tài nguyên và có tính tương thích cao trên nhiều hệ thống khác nhau.

2.  **Thư viện `ncurses`:**
    *   Đây là nền tảng chính cho việc xây dựng giao diện. `ncurses` là một thư viện lập trình cho phép điều khiển màn hình terminal một cách độc lập với loại terminal cụ thể. Nó cung cấp các hàm để vẽ cửa sổ (panes), di chuyển con trỏ, sử dụng màu sắc và xử lý dữ liệu nhập từ bàn phím (`getch`). Toàn bộ giao diện ba cột của chương trình được quản lý bởi `ncurses`.

3.  **API Hệ thống tệp của C/POSIX:**
    *   Để tương tác với hệ thống tệp, dự án sử dụng các hàm thư viện C tiêu chuẩn:
        *   **`dirent.h`**: Dùng để đọc nội dung của các thư mục (`opendir`, `readdir`).
        *   **`sys/stat.h`**: Dùng để lấy thông tin chi tiết (metadata) của tệp và thư mục, chẳng hạn như kích thước, ngày sửa đổi và loại tệp (`stat`).

4.  **Lập trình hướng sự kiện (Event-Driven Programming):**
    *   Chương trình hoạt động theo một vòng lặp vô tận (`while(1)`), chờ đợi sự kiện từ người dùng (nhấn phím). Khi một phím được nhấn, chương trình sẽ xử lý sự kiện đó (ví dụ: di chuyển lựa chọn, đổi thư mục) và cập nhật lại giao diện. Đây là một mô hình lập trình phổ biến cho các ứng dụng tương tác.

5.  **Kiến trúc giao diện ba cột:**
    *   Lấy cảm hứng từ trình quản lý tệp `ranger`, giao diện được chia thành ba cột riêng biệt (thư mục cha, thư mục hiện tại, xem trước tệp). Kiến trúc này cho phép người dùng điều hướng hệ thống tệp phân cấp một cách trực quan và hiệu quả.

## Yêu cầu

- Một trình biên dịch C (ví dụ: `gcc`).
- Thư viện `ncurses` (bao gồm cả các gói phát triển).

Trên các hệ thống Debian/Ubuntu, bạn có thể cài đặt các gói cần thiết bằng lệnh:
```bash
sudo apt-get update
sudo apt-get install build-essential libncurses5-dev libncursesw5-dev
```

## Hướng dẫn biên dịch và chạy

1.  **Clone repository:**
    ```bash
    git clone https://github.com/pie-12/fileinfo-2.git
    cd fileinfo-2
    ```

2.  **Biên dịch:**
    Sử dụng `make` để tự động biên dịch chương trình.
    ```bash
    make
    ```

3.  **Chạy chương trình:**
    ```bash
    ./fileinfo
    ```

4.  **Dọn dẹp (tùy chọn):**
    Để xóa file thực thi đã tạo, chạy lệnh:
    ```bash
    make clean
    ```

## Các phím tắt

| Phím              | Chức năng                               |
| ----------------- | --------------------------------------- |
| `↑` (Mũi tên lên) | Di chuyển lên trên trong danh sách.     |
| `↓` (Mũi tên xuống) | Di chuyển xuống dưới trong danh sách.   |
| `←` (Mũi tên trái) | Di chuyển đến thư mục cha.             |
| `→` hoặc `Enter`  | Đi vào thư mục con đang được chọn.     |
| `a`               | Bật/Tắt hiển thị các file/thư mục ẩn. |
| `q`               | Thoát chương trình.                     |

## Cơ sở lý thuyết

Dự án `FileInfo` được xây dựng dựa trên một số khái niệm và công nghệ nền tảng trong lập trình hệ thống trên môi trường Unix/Linux.

### 1. Giao diện người dùng dạng văn bản (TUI - Text-based User Interface)

TUI là một loại giao diện người dùng cho phép tương tác với chương trình thông qua môi trường dòng lệnh (terminal). Không giống như giao diện đồ họa (GUI), TUI sử dụng các ký tự văn bản, màu sắc và vị trí con trỏ để tạo ra các cửa sổ, panel và các thành phần tương tác.

- **Ưu điểm:** Nhẹ, tiêu thụ ít tài nguyên, hoạt động hiệu quả qua các kết nối mạng chậm (ví dụ: SSH), và mang lại trải nghiệm tập trung, không bị phân tâm cho người dùng chuyên nghiệp.
- **Vai trò trong dự án:** `FileInfo` sử dụng mô hình TUI để cung cấp một trình quản lý file nhanh và hiệu quả ngay trong terminal.

### 2. Thư viện `ncurses`

`Ncurses` (new curses) là một thư viện lập trình cung cấp một API cho phép các nhà phát triển viết các ứng dụng TUI một cách độc lập với loại terminal. Nó xử lý các chi tiết cấp thấp phức tạp như:

- Di chuyển con trỏ đến các vị trí cụ thể trên màn hình.
- Quản lý màu sắc cho văn bản và nền.
- Tạo và quản lý các "cửa sổ" (windows) hoặc "panel" trên màn hình.
- Xử lý input từ bàn phím một cách không đệm (unbuffered), cho phép chương trình phản ứng ngay lập tức với mỗi lần nhấn phím.

Trong `FileInfo`, `ncurses` là công cụ chính để vẽ giao diện 3 cột, hiển thị danh sách file, và xử lý điều hướng từ người dùng.

### 3. Tương tác với Hệ thống File trong C

Để đọc thông tin về file và thư mục, chương trình sử dụng các API hệ thống tiêu chuẩn của C trên các hệ điều hành tương tự Unix:

- **`<dirent.h>`:** Cung cấp các hàm để làm việc với các luồng thư mục (directory streams). Các hàm như `opendir()`, `readdir()`, và `closedir()` được dùng để liệt kê nội dung của một thư mục.
- **`<sys/stat.h>`:** Cung cấp hàm `stat()` hoặc `lstat()`, cho phép lấy thông tin chi tiết (metadata) của một file hoặc thư mục, bao gồm:
    - Quyền truy cập (permissions).
    - Kích thước file (size).
    - Loại (file thông thường, thư mục, symbolic link,...).
    - Thời gian sửa đổi cuối cùng (last modification time).

Đây là nền tảng cho tính năng xem trước (preview) và hiển thị thông tin file của `FileInfo`.

### 4. Cấu trúc dữ liệu

Để quản lý danh sách các file và thư mục, chương trình sử dụng các cấu trúc dữ liệu cơ bản như mảng động (dynamic arrays). Cấu trúc này cho phép lưu trữ một số lượng mục không xác định trước, sắp xếp chúng (ví dụ: theo tên), và truy cập hiệu quả để hiển thị lên giao diện.
