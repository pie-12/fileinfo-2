#include <ncurses.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <locale.h>
#include <time.h>

// Cấu trúc để lưu một mục trong thư mục
typedef struct {
    char name[256];
    bool is_dir;
} DirEntry;

// --- Khai báo các hàm ---

// Khởi tạo và dọn dẹp ncurses
void init_ncurses();
void cleanup_ncurses();

// Các hàm xử lý thư mục
int get_dir_entries(const char *path, DirEntry **entries, bool show_hidden);
int compare_entries(const void *a, const void *b);
void free_entries(DirEntry *entries, int count);

// Các hàm vẽ giao diện
void draw_pane(WINDOW *win, const char *title, DirEntry *entries, int count, int highlight, int scroll_offset, int pane_width);
void draw_preview(WINDOW *win, const char *base_path, const char *entry_name, bool show_hidden);

// --- Chương trình chính ---

int main() {
    init_ncurses();

    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);

    // Tạo 3 cửa sổ (pane)
    int pane_width = screen_width / 3;
    WINDOW *left_pane = newwin(screen_height, pane_width, 0, 0);
    WINDOW *middle_pane = newwin(screen_height, pane_width, 0, pane_width);
    WINDOW *right_pane = newwin(screen_height, screen_width - 2 * pane_width, 0, 2 * pane_width);

    char current_path[PATH_MAX];
    if (getcwd(current_path, sizeof(current_path)) == NULL) {
        perror("getcwd() error");
        cleanup_ncurses();
        return 1;
    }

    int current_selection = 0;
    int scroll_offset = 0;
    bool show_hidden = false;

    // --- Vòng lặp chính ---
    bool running = true;
    while (running) {
        // 1. Lấy dữ liệu cho trạng thái hiện tại
        DirEntry *middle_entries = NULL;
        int middle_count = get_dir_entries(current_path, &middle_entries, show_hidden);

        char parent_path[PATH_MAX];
        snprintf(parent_path, sizeof(parent_path), "%s/..", current_path);
        DirEntry *left_entries = NULL;
        int left_count = get_dir_entries(parent_path, &left_entries, show_hidden);

        // Đảm bảo selection không vượt quá giới hạn
        if (current_selection >= middle_count) {
            current_selection = middle_count > 0 ? middle_count - 1 : 0;
        }
        if (current_selection < 0) {
            current_selection = 0;
        }
        
        // Logic cuộn
        int pane_height = screen_height - 2;
        if (current_selection < scroll_offset) {
            scroll_offset = current_selection;
        }
        if (current_selection >= scroll_offset + pane_height) {
            scroll_offset = current_selection - pane_height + 1;
        }

        // 2. Vẽ giao diện
        draw_pane(left_pane, "Parent", left_entries, left_count, -1, 0, pane_width);
        draw_pane(middle_pane, current_path, middle_entries, middle_count, current_selection, scroll_offset, pane_width);
        if (middle_count > 0) {
            draw_preview(right_pane, current_path, middle_entries[current_selection].name, show_hidden);
        } else {
            werase(right_pane);
            box(right_pane, 0, 0);
            mvwprintw(right_pane, 1, 2, "Empty directory");
            // Cũng hiển thị trạng thái file ẩn ở đây
            mvwprintw(right_pane, 3, 2, "Hidden Files: %s", show_hidden ? "On" : "Off");
            wrefresh(right_pane);
        }

        // 3. Chờ người dùng nhập phím
        int ch = getch();

        // 4. Xử lý phím bấm
        switch (ch) {
            case 'q':
                running = false;
                break;
            case 'a':
                show_hidden = !show_hidden;
                // Đặt lại lựa chọn về 0 khi thay đổi chế độ xem
                current_selection = 0;
                scroll_offset = 0;
                break;
            case KEY_DOWN:
                if (current_selection < middle_count - 1) {
                    current_selection++;
                }
                break;
            case KEY_UP:
                if (current_selection > 0) {
                    current_selection--;
                }
                break;
            case KEY_LEFT:
                // Di chuyển lên thư mục cha một cách an toàn
                if (strcmp(current_path, "/") != 0) { // Không đi lên từ thư mục gốc
                    char *last_slash = strrchr(current_path, '/');
                    if (last_slash != NULL) {
                        if (last_slash == current_path) {
                            *(last_slash + 1) = '\0';
                        } else {
                            *last_slash = '\0';
                        }
                        current_selection = 0;
                        scroll_offset = 0;
                    }
                }
                break;
            case KEY_RIGHT:
            case '\n': // Phím Enter
                if (middle_count > 0 && middle_entries[current_selection].is_dir) {
                    char next_path[PATH_MAX];
                    snprintf(next_path, sizeof(next_path), "%s/%s", current_path, middle_entries[current_selection].name);
                     char resolved_path_enter[PATH_MAX];
                    if (realpath(next_path, resolved_path_enter)) {
                        strcpy(current_path, resolved_path_enter);
                    }
                    current_selection = 0;
                    scroll_offset = 0;
                }
                break;
        }

        // Giải phóng bộ nhớ đã cấp phát trong vòng lặp
        free_entries(middle_entries, middle_count);
        free_entries(left_entries, left_count);
    }

    // Dọn dẹp
    delwin(left_pane);
    delwin(middle_pane);
    delwin(right_pane);
    cleanup_ncurses();

    return 0;
}

// --- Định nghĩa các hàm ---

void init_ncurses() {
    setlocale(LC_ALL, ""); // Hỗ trợ ký tự Unicode
    initscr();             // Khởi tạo màn hình ncurses
    cbreak();              // Tắt chế độ line buffering
    noecho();              // Không hiển thị phím bấm ra màn hình
    curs_set(0);           // Ẩn con trỏ
    keypad(stdscr, TRUE);  // Bật các phím chức năng (mũi tên, F1, v.v.)
}

void cleanup_ncurses() {
    endwin(); // Khôi phục lại terminal
}

int compare_entries(const void *a, const void *b) {
    DirEntry *entry_a = (DirEntry *)a;
    DirEntry *entry_b = (DirEntry *)b;

    // Ưu tiên thư mục lên đầu
    if (entry_a->is_dir != entry_b->is_dir) {
        return entry_a->is_dir ? -1 : 1;
    }
    // Sắp xếp theo tên
    return strcmp(entry_a->name, entry_b->name);
}

int get_dir_entries(const char *path, DirEntry **entries, bool show_hidden) {
    DIR *d = opendir(path);
    if (!d) return 0;

    struct dirent *dir;
    int count = 0;
    int capacity = 10;
    *entries = malloc(capacity * sizeof(DirEntry));

    while ((dir = readdir(d)) != NULL) {
        // Bỏ qua "." và ".."
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
            continue;
        }

        // Lọc file ẩn nếu cần
        if (!show_hidden && dir->d_name[0] == '.') {
            continue;
        }

        if (count >= capacity) {
            capacity *= 2;
            *entries = realloc(*entries, capacity * sizeof(DirEntry));
        }

        DirEntry *entry = &(*entries)[count];
        strncpy(entry->name, dir->d_name, sizeof(entry->name) - 1);

        struct stat st;
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, dir->d_name);
        if (stat(full_path, &st) == 0) {
            entry->is_dir = S_ISDIR(st.st_mode);
        } else {
            entry->is_dir = false;
        }
        count++;
    }
    closedir(d);

    // Sắp xếp các mục
    qsort(*entries, count, sizeof(DirEntry), compare_entries);

    return count;
}

void free_entries(DirEntry *entries, int count) {
    if (entries) {
        free(entries);
    }
}

void draw_pane(WINDOW *win, const char *title, DirEntry *entries, int count, int highlight, int scroll_offset, int pane_width) {
    werase(win);
    box(win, 0, 0);
    
    // In tiêu đề
    wattron(win, A_BOLD);
    mvwprintw(win, 0, 2, " %s ", title);
    wattroff(win, A_BOLD);

    int max_y, max_x;
    getmaxyx(win, max_y, max_x);
    (void)max_x; // Bỏ cảnh báo không sử dụng biến

    int pane_height = max_y - 2;

    for (int i = 0; i < pane_height && (i + scroll_offset) < count; ++i) {
        int current_index = i + scroll_offset;
        if (current_index == highlight) {
            wattron(win, A_REVERSE); // Bật chế độ highlight
        }

        char display_name[256];
        snprintf(display_name, sizeof(display_name), "%s%s", entries[current_index].name, entries[current_index].is_dir ? "/" : "");
        
        // Cắt bớt tên nếu quá dài
        display_name[pane_width - 3] = '\0';

        mvwprintw(win, i + 1, 2, "%s", display_name);

        if (current_index == highlight) {
            wattroff(win, A_REVERSE); // Tắt highlight
        }
    }
    wrefresh(win);
}

/**
 * @brief Định dạng trường mode thành chuỗi 10 ký tự (ví dụ: -rwxr-xr-x).
 * 
 * @param mode Trường st_mode từ struct stat.
 * @param perm_str Chuỗi để lưu kết quả (phải có ít nhất 11 byte).
 */
void format_permissions(mode_t mode, char *perm_str) {
    // Ký tự đầu tiên cho loại file
    if (S_ISDIR(mode)) perm_str[0] = 'd';
    else if (S_ISLNK(mode)) perm_str[0] = 'l';
    else if (S_ISCHR(mode)) perm_str[0] = 'c';
    else if (S_ISBLK(mode)) perm_str[0] = 'b';
    else perm_str[0] = '-'; // Regular file và các loại khác

    // Quyền của Owner
    perm_str[1] = (mode & S_IRUSR) ? 'r' : '-';
    perm_str[2] = (mode & S_IWUSR) ? 'w' : '-';
    perm_str[3] = (mode & S_IXUSR) ? 'x' : '-';

    // Quyền của Group
    perm_str[4] = (mode & S_IRGRP) ? 'r' : '-';
    perm_str[5] = (mode & S_IWGRP) ? 'w' : '-';
    perm_str[6] = (mode & S_IXGRP) ? 'x' : '-';

    // Quyền của Others
    perm_str[7] = (mode & S_IROTH) ? 'r' : '-';
    perm_str[8] = (mode & S_IWOTH) ? 'w' : '-';
    perm_str[9] = (mode & S_IXOTH) ? 'x' : '-';

    // Ký tự kết thúc chuỗi
    perm_str[10] = '\0';
}

void draw_preview(WINDOW *win, const char *base_path, const char *entry_name, bool show_hidden) {
    werase(win);
    box(win, 0, 0);
    
    mvwprintw(win, 0, 2, " Preview ");

    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", base_path, entry_name);

    struct stat st;
    if (stat(full_path, &st) == -1) {
        mvwprintw(win, 2, 2, "Cannot read info");
        wrefresh(win);
        return;
    }

    mvwprintw(win, 2, 2, "File: %s", entry_name);

    char perm_str[11];
    format_permissions(st.st_mode, perm_str);
    mvwprintw(win, 4, 2, "Permissions: %s", perm_str);

    // Diễn giải quyền cho người dùng
    char access_str[100];
    strcpy(access_str, "Access: ");
    bool first_perm = true;
    if (st.st_mode & S_IRUSR) { strcat(access_str, "Read"); first_perm = false; }
    if (st.st_mode & S_IWUSR) { if (!first_perm) strcat(access_str, ", "); strcat(access_str, "Write"); first_perm = false; }
    if (st.st_mode & S_IXUSR) { if (!first_perm) strcat(access_str, ", "); strcat(access_str, "Execute"); }
    mvwprintw(win, 5, 2, "%s", access_str);

    mvwprintw(win, 7, 2, "Type: %s", S_ISDIR(st.st_mode) ? "Directory" : "Regular File");
    mvwprintw(win, 8, 2, "Size: %lld bytes", (long long)st.st_size);
    
    char time_str[100];
    strftime(time_str, sizeof(time_str), "%b %d %H:%M %Y", localtime(&st.st_mtime));
    mvwprintw(win, 10, 2, "Modified: %s", time_str);

    // Hiển thị trạng thái file ẩn
    mvwprintw(win, 12, 2, "Hidden Files: %s", show_hidden ? "On" : "Off");

    wrefresh(win);
}