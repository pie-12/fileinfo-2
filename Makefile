# Tên trình biên dịch
CC = gcc

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

# Biến kiểm tra sự tồn tại của pokeget
POKEGET_CHECK = command -v pokeget >/dev/null 2>&1

# Target mặc định, được chạy khi gõ `make`
all: check_deps $(TARGET)

check_deps:
	@if ! $(POKEGET_CHECK); then \
		echo "Lỗi: 'pokeget' chưa được cài đặt."; \
		echo "Hãy chạy 'make install_deps' để cài đặt."; \
		exit 1; \
	fi

# Quy tắc để tạo file thực thi từ file mã nguồn
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Target để cài đặt các phụ thuộc
install_deps:
	@echo "Đang cài đặt pokeget..."
	@curl -sL https://raw.githubusercontent.com/talwat/pokeget/main/scripts/install.sh | bash

# Target để dọn dẹp
# Xóa file thực thi đã được tạo
clean:
	rm -f $(TARGET)

.PHONY: all clean install_deps check_deps
