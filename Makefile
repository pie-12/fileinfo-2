# Tên trình biên dịch
CC = gcc

SHELL = /bin/bash

# Cờ cho trình biên dịch
# -Wall: Bật tất cả các cảnh báo để code an toàn hơn
# -g: Thêm thông tin gỡ lỗi (debug) vào file thực thi
CFLAGS = -Wall -g

# Cờ cho trình liên kết (linker)
LDFLAGS = -lncurses

# Tên file thực thi mục tiêu
TARGET = fileinfo

# Tên file mã nguồn
SRCS = main.c

# Target mặc định, được chạy khi gõ `make`
all: $(TARGET)

# Quy tắc để tạo file thực thi từ file mã nguồn
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Target để dọn dẹp
# Xóa file thực thi đã được tạo
clean:
	rm -f $(TARGET)

.PHONY: all clean
