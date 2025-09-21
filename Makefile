CC = gcc
CFLAGS = -Wall -O2
TARGET = autowallpaper
SRC = autowallpaper.c

PREFIX = /opt/autowallpaper
BIN_DIR = $(PREFIX)/bin
LOG_DIR = $(PREFIX)/logs
CONFIG_DIR = $(PREFIX)/config
AUTOSTART_DIR = $(HOME)/.config/autostart
DESKTOP_FILE = autowallpaper.desktop

.PHONY: all clean install uninstall

GREEN=\033[0;32m
YELLOW=\033[1;33m
NC=\033[0m

all: $(TARGET)

$(TARGET): $(SRC)
	@echo -e "$(GREEN)Compiling $(SRC) -> $(TARGET)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $^

clean:
	@echo -e "$(GREEN)Cleaning build files...$(NC)"
	@rm -f $(TARGET)

install: $(TARGET)
	@echo -e "$(GREEN)Creating directories...$(NC)"
	@sudo mkdir -p $(BIN_DIR) $(LOG_DIR) $(CONFIG_DIR) $(AUTOSTART_DIR)
	@echo -e "$(GREEN)Installing binary...$(NC)"
	@sudo cp $(TARGET) $(BIN_DIR)/$(TARGET)
	@sudo chmod 755 $(BIN_DIR)/$(TARGET)
	@echo -e "$(GREEN)Copying .desktop autostart...$(NC)"
	@cp $(DESKTOP_FILE) $(AUTOSTART_DIR)/$(DESKTOP_FILE)
	@chmod 644 $(AUTOSTART_DIR)/$(DESKTOP_FILE)
	@echo -e "$(YELLOW)Installation complete!$(NC)"

uninstall:
	@echo -e "$(YELLOW)Removing all installed files and directories...$(NC)"
	@sudo rm -rf $(BIN_DIR) $(LOG_DIR) $(CONFIG_DIR)
	@rm -f $(AUTOSTART_DIR)/$(DESKTOP_FILE)
	@echo -e "$(YELLOW)Uninstallation complete!$(NC)"
