# Auto Wallpaper Changer for GNOME

A professional Linux utility that periodically changes the GNOME desktop wallpaper by selecting a random image from a specified directory. It runs as a user-level daemon, supports autostart via a `.desktop` file, logs activity, and allows configurable intervals.

---

## Features

- Runs as a background daemon
- Randomly selects a wallpaper from a specified directory
- Supports `.jpg`, `.jpeg` and `.png` images
- Configurable interval for wallpaper changes (default: 5 minutes)
- Uses GNOME's `gsettings` to change wallpapers
- User-friendly installation via a Makefile
- Autostart support with a `.desktop` file
- Logs activities to `/opt/autowallpaper/logs/wallpaper_daemon.log`
- Clean uninstall removes all installed files

---

## Requirements

- Linux environment with GNOME desktop
- `gsettings` command available
- C compiler (e.g., GCC) to build the project
- `sudo` privileges for installation to `/opt/autowallpaper`

---

## Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/mnurak/auto-wallpaper-changer-gnome.git
   cd auto-wallpaper-changer-gnome
   ```

2. **Build the project using Makefile:**
   ```bash
   make
   ```

3. **Install the project (requires sudo):**
   ```bash
   sudo make install
   ```
   - Installs binary to `/opt/autowallpaper/bin/`
   - Creates directories `/opt/autowallpaper/logs/` and `/opt/autowallpaper/config/`
   - Copies autostart `.desktop` file to `~/.config/autostart/`

4. **Clean build files (optional):**
   ```bash
   make clean
   ```

5. **Uninstall (removes all installed files):**
   ```bash
   sudo make uninstall
   ```

---

## Configuration

### Command-Line Arguments

- `--dir` or `-d`: Directory containing wallpapers  
  Example:
  ```bash
  /opt/autowallpaper/bin/autowallpaper --dir /path/to/wallpapers
  ```

- `--time` or `-t`: Interval in seconds between wallpaper changes (default: 300)  
  Example:
  ```bash
  /opt/autowallpaper/bin/autowallpaper --time 600
  ```

> If both command-line arguments and environment variables are set, command-line arguments take precedence.

### Environment Variable

- `WALLPAPER_DIR`: Specify the wallpaper directory  
  Example:
  ```bash
  export WALLPAPER_DIR=/path/to/wallpapers
  /opt/autowallpaper/bin/autowallpaper
  ```

---

## Usage

- **Run manually for testing:**
  ```bash
  /opt/autowallpaper/bin/autowallpaper --dir /path/to/wallpapers --time 120 &
  ```

- **Check log output:**
  ```bash
  tail -f /opt/autowallpaper/logs/wallpaper_daemon.log
  ```

> After installation, the daemon starts automatically at user login via the `.desktop` file.

---

## How It Works

1. **Initialization:** Reads configuration from command-line arguments or environment variables
2. **Scan Wallpapers:** Scans the directory for supported images
3. **Random Selection:** Chooses a wallpaper randomly
4. **Apply Wallpaper:** Sets wallpaper using `gsettings`
5. **Repeat:** Waits for the specified interval and repeats
6. **Logging:** Writes messages to `/opt/autowallpaper/logs/wallpaper_daemon.log`
7. **Graceful Exit:** Responds to `SIGINT` and `SIGTERM` for cleanup

---

## Autostart

- Uses a `.desktop` file located in `~/.config/autostart/autowallpaper.desktop` to run automatically at login
- Users can enable/disable autostart without affecting other users

---

## Logging

- **Log file:** `/opt/autowallpaper/logs/wallpaper_daemon.log`
- Logs include timestamps and status messages for each wallpaper change
- Helps debug issues like missing files or failed wallpaper updates

---

## Troubleshooting

- **Wallpaper Not Changing:** Ensure `gsettings` is available and `DISPLAY` is set (handled automatically via `.desktop`)
- **No Wallpapers Found:** Confirm the directory contains supported `.jpg` or `.png` files
- **Permission Issues:** Run `sudo make install` to ensure proper access to `/opt/autowallpaper`

---

## Clean Uninstallation

```bash
sudo make uninstall
```

Removes:
- Binary `/opt/autowallpaper/bin/autowallpaper`
- Logs and config directories under `/opt/autowallpaper/`
- Autostart `.desktop` file in `~/.config/autostart/`

---

## Author

**Karun M**