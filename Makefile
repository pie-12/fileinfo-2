# Tên trình biên dịch
CC = gcc

SHELL = /bin/bash

# Cờ cho trình biên dịch
# -Wall: Bật tất cả các cảnh báo để code an toàn hơn
# -g: Thêm thông tin gỡ lỗi (debug) vào file thực thi
# -DPOKEGET_CMD=...: Truyền đường dẫn pokeget cục bộ vào chương trình C
CFLAGS = -Wall -g
LDFLAGS = -lncurses

# --- Cấu hình cài đặt cục bộ ---
LOCAL_DIR = $(CURDIR)/.local
POKEGET_CMD = $(LOCAL_DIR)/bin/pokeget
CFLAGS += -DPOKEGET_CMD='"$(POKEGET_CMD)"'

# Tên file thực thi mục tiêu
TARGET = fileinfo

# Tên file mã nguồn
SRCS = main.c

# Target mặc định, được chạy khi gõ `make`
all: $(TARGET)

# Quy tắc để tạo file thực thi, phụ thuộc vào việc cài đặt pokeget trước
$(TARGET): $(SRCS) $(POKEGET_CMD)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Quy tắc để cài đặt pokeget nếu nó chưa tồn tại
$(POKEGET_CMD):
	@echo "Phụ thuộc 'pokeget' chưa được cài đặt vào thư mục cục bộ."
	@echo "Đang tự động cài đặt thủ công..."
	@# 1. Tạo thư mục cục bộ
	@mkdir -p $(LOCAL_DIR)/bin
	@# 2. Tải trực tiếp file script pokeget vào thư mục bin cục bộ
	@curl -sL https://raw.githubusercontent.com/talwat/pokeget/main/pokeget -o $(POKEGET_CMD)
	@# 3. Cấp quyền thực thi cho nó
	@chmod +x $(POKEGET_CMD)
	@echo "pokeget đã được cài đặt cục bộ vào $(POKEGET_CMD)"

# Target để dọn dẹp
# Xóa file thực thi và thư mục cài đặt cục bộ
clean:
	rm -f $(TARGET)
	rm -rf $(LOCAL_DIR)

.PHONY: all clean
