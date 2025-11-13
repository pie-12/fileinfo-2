#include <ncurses.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <locale.h>

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
int get_dir_entries(const char *path, DirEntry **entries);
int compare_entries(const void *a, const void *b);
void free_entries(DirEntry *entries, int count);

// Các hàm vẽ giao diện
void draw_pane(WINDOW *win, const char *title, DirEntry *entries, int count, int highlight, int scroll_offset);
void draw_preview(WINDOW *win, const char *base_path, const char *entry_name);

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
    int scroll_offset = 0; // Để cuộn danh sách file
    int ch;
    bool running = true;

    while (running) {
        // 1. Lấy danh sách file cho cột giữa (thư mục hiện tại)
        DirEntry *middle_entries = NULL;
        int middle_count = get_dir_entries(current_path, &middle_entries);

        // 2. Lấy danh sách file cho cột trái (thư mục cha)
        char parent_path[PATH_MAX];
        snprintf(parent_path, sizeof(parent_path), "%s/..", current_path);
        DirEntry *left_entries = NULL;
        int left_count = get_dir_entries(parent_path, &left_entries);

        // Đảm bảo selection không vượt quá giới hạn
        if (current_selection >= middle_count) {
            current_selection = middle_count > 0 ? middle_count - 1 : 0;
        }
        if (current_selection < 0) {
            current_selection = 0;
        }
        
        // Logic cuộn
        int pane_height = screen_height - 2; // Chiều cao có thể vẽ bên trong box
        if (current_selection < scroll_offset) {
            scroll_offset = current_selection;
        }
        if (current_selection >= scroll_offset + pane_height) {
            scroll_offset = current_selection - pane_height + 1;
        }


        // 3. Vẽ các cột
        draw_pane(left_pane, "Parent", left_entries, left_count, -1, 0); // Không highlight cột trái
        draw_pane(middle_pane, current_path, middle_entries, middle_count, current_selection, scroll_offset);
        if (middle_count > 0) {
            draw_preview(right_pane, current_path, middle_entries[current_selection].name);
        } else {
            werase(right_pane);
            box(right_pane, 0, 0);
            mvwprintw(right_pane, 1, 2, "Empty directory");
            wrefresh(right_pane);
        }

        ch = getch();

        switch (ch) {
            case 'q':
                running = false;
                break;
            case 'j':
            case KEY_DOWN:
                if (current_selection < middle_count - 1) {
                    current_selection++;
                }
                break;
            case 'k':
            case KEY_UP:
                if (current_selection > 0) {
                    current_selection--;
                }
                break;
            case 'h':
            case KEY_LEFT:
                // Di chuyển lên thư mục cha
                snprintf(current_path, sizeof(current_path), "%s/..", current_path);
                char resolved_path[PATH_MAX];
                if (realpath(current_path, resolved_path)) {
                    strcpy(current_path, resolved_path);
                }
                current_selection = 0;
                scroll_offset = 0;
                break;
            case 'l':
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

int get_dir_entries(const char *path, DirEntry **entries) {
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

void draw_pane(WINDOW *win, const char *title, DirEntry *entries, int count, int highlight, int scroll_offset) {
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

void draw_preview(WINDOW *win, const char *base_path, const char *entry_name) {
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
    mvwprintw(win, 4, 2, "Type: %s", S_ISDIR(st.st_mode) ? "Directory" : "Regular File");
    mvwprintw(win, 5, 2, "Size: %lld bytes", (long long)st.st_size);
    
    char time_str[100];
    strftime(time_str, sizeof(time_str), "%b %d %H:%M %Y", localtime(&st.st_mtime));
    mvwprintw(win, 7, 2, "Modified: %s", time_str);

    wrefresh(win);
}