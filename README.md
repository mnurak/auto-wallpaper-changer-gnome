# Wallpaper Changer Daemon

A simple Linux daemon that periodically changes the desktop wallpaper by selecting a random image from a specified directory. The daemon runs in the background, scans a directory for supported image formats, and sets a new wallpaper at regular intervals.

## Features

- Daemonized process running in the background.
- Randomly selects a wallpaper from a specified directory.
- Supports image formats like `.jpg` and `.png`.
- Configurable time interval for wallpaper change.
- Uses GNOME's `gsettings` to change wallpapers.

## Requirements

- Linux environment with GNOME desktop.
- The `gsettings` command should be available.
- C compiler (e.g., GCC) to build the project.

## Installation

1. **Clone the repository:**

   ```bash
   git clone https://github.com/yourusername/wallpaper-changer-daemon.git
   cd wallpaper-changer-daemon
   ```

2. **Compile the code:**

   ```bash
   gcc -o wallpaper_daemon wallpaper_daemon.c
   ```

## Configuration

The daemon can be configured using command-line arguments or environment variables.

### Environment Variable

- `WALLPAPER_DIR`: The directory containing the wallpapers.

### Command-Line Arguments

- `--dir` or `-d`: Specify the directory where your wallpapers are stored.
  - Example: `./wallpaper_daemon --dir /path/to/wallpapers`
- `--time` or `-t`: Set the interval (in seconds) between wallpaper changes. Default is 300 seconds (5 minutes).
  - Example: `./wallpaper_daemon --time 600` (sets the interval to 10 minutes)

## Example Usage

Run the daemon with a custom wallpaper directory and interval:

```bash
./wallpaper_daemon --dir /path/to/wallpapers --time 600
```

Or run the daemon using the environment variable:

```bash
export WALLPAPER_DIR=/path/to/wallpapers
./wallpaper_daemon
```

This will automatically use the `WALLPAPER_DIR` environment variable to find the wallpaper directory.

## How It Works

1. The daemon is forked into the background using the daemonize function.
2. It continuously scans the specified wallpaper directory for image files (`.jpg`, `.png`).
3. A random wallpaper is selected from the available files.
4. The wallpaper is set using GNOME's `gsettings` command.
5. This process repeats indefinitely with a delay defined by the interval parameter.

## Example Workflow

1. **Daemon Initialization:** Reads configuration from command-line arguments or environment variables.
2. **Wallpaper Scan:** Scans the specified directory for valid image files.
3. **Random Selection:** Selects a random wallpaper from the list of valid images.
4. **Wallpaper Update:** Applies the selected wallpaper to the system.
5. **Interval Repeat:** Repeats the process after the specified time interval.

## Logging and Signals

- **Log Output:** Logs are written to `/tmp/wallpaper_daemon.log` by default. If the log file cannot be created, logs are redirected to `/dev/null`.
- **Signals:** The daemon listens for `SIGINT` and `SIGTERM` signals to gracefully exit. Upon receiving a signal, the daemon cleans up resources and shuts down.

## Troubleshooting

- **No Wallpapers Found:** If the specified directory contains no supported wallpapers, the daemon logs a message and waits for the next interval.
- **Failed to Set Wallpaper:** If the wallpaper cannot be set (e.g., due to an incorrect file format or missing dependencies), the daemon logs an error.

## Author
Karun M