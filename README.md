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
