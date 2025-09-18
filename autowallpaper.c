#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <stdarg.h>
#include <fcntl.h>

#define MAX_PATH_LEN 1024

volatile sig_atomic_t cleanup_needed = 0;

// Structure to hold wallpaper file paths
typedef struct
{
    char **paths;
    size_t count;
} WallpaperList;

// Function Prototypes
void daemonize(void);
void wallpaper_daemon_main(int argc, char *argv[]);
int load_config(char **wallpaper_dir);
int scan_wallpapers(const char *dir, WallpaperList *list);
const char *select_random_wallpaper(const WallpaperList *list);
int set_wallpaper(const char *filepath);
void setup_signal_handlers(void);
void signal_handler(int signum);
void log_message(const char *format, ...);
void main_loop(char *wallpaper_dir, unsigned short interval);
void free_wallpaper_list(WallpaperList *list);

// Load config from env vars or config file
int load_config(char **wallpaper_dir)
{
    const char *env_dir = getenv("WALLPAPER_DIR");
    if (env_dir)
    {
        *wallpaper_dir = malloc(strlen(env_dir) + 1);
        if (*wallpaper_dir == NULL)
        {
            fprintf(stderr, "Memory allocation failed for wallpaper_dir.\n");
            return -1;
        }
        strcpy(*wallpaper_dir, env_dir);
    }
    else
    {
        fprintf(stderr, "Environment variable WALLPAPER_DIR not set.\n");
        return -1;
    }

    return 0;
}

// Scan wallpaper directory and populate WallpaperList
int scan_wallpapers(const char *dir, WallpaperList *list)
{
    DIR *d = opendir(dir);
    if (!d)
    {
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    struct stat file_stat;
    list->count = 0;
    list->paths = NULL;

    while ((entry = readdir(d)))
    {
        char filepath[MAX_PATH_LEN];
        snprintf(filepath, MAX_PATH_LEN, "%s/%s", dir, entry->d_name);

        if (stat(filepath, &file_stat) == 0 && S_ISREG(file_stat.st_mode))
        {
            if (strstr(entry->d_name, ".jpg") || strstr(entry->d_name, ".jpg") || strstr(entry->d_name, ".png"))
            {
                list->paths = realloc(list->paths, (list->count + 1) * sizeof(char *));
                if (!list->paths)
                {
                    perror("realloc");
                    closedir(d);
                    return -1;
                }
                list->paths[list->count] = malloc(strlen(filepath) + 1);
                if (!list->paths[list->count])
                {
                    perror("malloc");
                    closedir(d);
                    return -1;
                }

                strcpy(list->paths[list->count], filepath);
                list->count++;
            }
        }
    }

    closedir(d);
    return 0;
}

// Select a random wallpaper path from WallpaperList
const char *select_random_wallpaper(const WallpaperList *list)
{
    if (list->count == 0)
    {
        return NULL;
    }

    srand(time(NULL));
    int random_index = rand() % list->count;
    return list->paths[random_index];
}

// Set wallpaper using system call or GNOME API
int set_wallpaper(const char *filepath)
{
    char command[MAX_PATH_LEN + 100];
    snprintf(command, sizeof(command), "gsettings set org.gnome.desktop.background picture-uri-dark 'file://%s'", filepath);
    int ret = system(command);
    return (ret == 0) ? 0 : -1;
}

// Setup signal handling
void signal_handler(int signum)
{
    printf("\nSignal %d received. Exiting...\n", signum);
    cleanup_needed = 1;
}

void setup_signal_handlers()
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
}

// Log helper (optional)
void log_message(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

// Main loop: scan → select → set → sleep → repeat
void main_loop(char *wallpaper_dir, unsigned short interval)
{
    WallpaperList list;

    while (1)
    {
        if (cleanup_needed == 1)
        {
            free_wallpaper_list(&list);
            return;
        }

        if (scan_wallpapers(wallpaper_dir, &list) != 0)
        {
            log_message("Error scanning wallpapers.\n");
            free_wallpaper_list(&list);
            sleep(interval);
            continue;
        }

        if (list.count == 0)
        {
            log_message("No wallpapers found.\n");
            free_wallpaper_list(&list);
            sleep(interval);
            continue;
        }

        const char *wallpaper = select_random_wallpaper(&list);
        if (wallpaper)
        {
            if (set_wallpaper(wallpaper) != 0)
            {
                log_message("Failed to set wallpaper: %s\n", wallpaper);
            }
        }
        free_wallpaper_list(&list);

        sleep(interval);
    }
}

void free_wallpaper_list(WallpaperList *list)
{
    if (list->paths == NULL)
        return;

    for (size_t i = 0; i < list->count; i++)
        free(list->paths[i]);
    free(list->paths);
    list->paths = NULL;
    list->count = 0;
}

// Deamon creation so the process can run individually
void daemonize(void)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("Fork failed");
        exit(1);
    }

    if (pid > 0)
    {
        // Parent process exits
        exit(0);
    }

    // Child process becomes the daemon

    if (setsid() < 0)
    {
        perror("setsid failed");
        exit(1);
    }

    if (chdir("/") < 0)
    {
        perror("chdir failed");
        exit(1);
    }

    FILE *pid_file = fopen("/tmp/wallpaper_changer.pid", "w");
    if (pid_file)
    {
        fprintf(pid_file, "%d\n", getpid());
        fflush(pid_file);        // flush stdio buffer
        fsync(fileno(pid_file)); // flush OS buffer
        fclose(pid_file);
        printf("PID %d written\n", getpid());
    }
    else
    {
        perror("Failed to open PID file for writing");
        return;
    }

    // Close file descriptors (stdin, stdout, stderr)
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Redirect output to log file
    int log_fd = open("/tmp/wallpaper_daemon.log", O_RDWR | O_CREAT | O_APPEND, 0640);
    if (log_fd != -1)
    {
        dup2(log_fd, STDOUT_FILENO);
        dup2(log_fd, STDERR_FILENO);
    }
    else
    {
        // Fall back to /dev/null if log file can't be opened
        int null_fd = open("/dev/null", O_RDWR);
        dup2(null_fd, STDOUT_FILENO);
        dup2(null_fd, STDERR_FILENO);
    }
}

// actual wallpaper function that takes care of all the working
void wallpaper_daemon_main(int argc, char *argv[])
{

    char *wallpaper_dir = NULL;
    unsigned short interval = 300;

    printf("Starting wallpaper daemon with PID %d\n", getpid());

    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "--dir") == 0 || strcmp(argv[i], "--d") == 0) && i + 1 < argc)
        {
            printf("%d : Wallpaper directory: %s\n", i, argv[i + 1]);
            wallpaper_dir = malloc(strlen(argv[i + 1]) + 1);
            strcpy(wallpaper_dir, argv[i + 1]);
            i++;
        }
        else if ((strcmp(argv[i], "--time") == 0 || strcmp(argv[i], "--t") == 0) && i + 1 < argc)
        {
            interval = atoi(argv[i + 1]);
            printf("%d : Setting interval to %d\n", i, interval);
            if (interval >= 65535)
            {
                printf("The inteval must not exceed 65,534s reverting back to default 300s\n");
                interval = 300;
            }
            i++;
        }
    }

    if (wallpaper_dir == NULL && load_config(&wallpaper_dir) != 0)
    {
        return;
    }

    setup_signal_handlers();

    main_loop(wallpaper_dir, interval);
    remove("/tmp/wallpaper_changer.pid");
    free(wallpaper_dir);
}

// Main function to create the deamon and run the wallpaper changer
int main(int argc, char *argv[])
{
    daemonize();

    wallpaper_daemon_main(argc, argv);

    return 0;
}