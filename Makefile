# Makefile for Sensor Data Collection System
# 适用于IAR EMBEDDED WORKBENCH项目

# 项目配置
PROJECT_NAME = sensor_system
SRC_DIR = src
PROJECT_DIR = project
BUILD_DIR = build

# 源文件
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/sensor_data.c \
          $(SRC_DIR)/database.c \
          $(SRC_DIR)/communication.c

# 头文件
HEADERS = $(SRC_DIR)/config.h \
          $(SRC_DIR)/sensor_data.h \
          $(SRC_DIR)/database.h \
          $(SRC_DIR)/communication.h

# IAR工具链路径（需要根据实际安装路径修改）
IAR_PATH = C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.4/arm
ICCARM = "$(IAR_PATH)/bin/iccarm.exe"
ILINKARM = "$(IAR_PATH)/bin/ilinkarm.exe"

# 编译选项
CFLAGS = --cpu=Cortex-M3 \
         --dlib_config "$(IAR_PATH)/inc/c/DLib_Config_Normal.h" \
         -I$(SRC_DIR) \
         -DDEBUG=1 \
         -DSTM32F103xB \
         --debug \
         --endian=little \
         --cpu_mode=thumb

# 链接选项
LFLAGS = --cpu=Cortex-M3 \
         --config "$(IAR_PATH)/config/linker/ST/stm32f103xB.icf" \
         --map $(BUILD_DIR)/$(PROJECT_NAME).map \
         --entry __iar_program_start

# 目标文件
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# 默认目标
all: $(BUILD_DIR)/$(PROJECT_NAME).out

# 创建构建目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 编译源文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(ICCARM) $(CFLAGS) -o $@ $<

# 链接生成可执行文件
$(BUILD_DIR)/$(PROJECT_NAME).out: $(OBJECTS)
	$(ILINKARM) $(LFLAGS) -o $@ $^

# 清理构建文件
clean:
	rm -rf $(BUILD_DIR)

# 重新构建
rebuild: clean all

# 显示项目信息
info:
	@echo "项目名称: $(PROJECT_NAME)"
	@echo "源文件目录: $(SRC_DIR)"
	@echo "项目文件目录: $(PROJECT_DIR)"
	@echo "构建目录: $(BUILD_DIR)"
	@echo "源文件:"
	@echo "$(SOURCES)" | tr ' ' '\n' | sed 's/^/  /'
	@echo "头文件:"
	@echo "$(HEADERS)" | tr ' ' '\n' | sed 's/^/  /'

# 检查代码风格（需要安装cppcheck）
check:
	@echo "检查代码风格和潜在问题..."
	cppcheck --enable=all --std=c99 $(SRC_DIR)

# 生成文档（需要安装doxygen）
doc:
	@echo "生成代码文档..."
	doxygen Doxyfile

# 安装数据库表
install-db:
	@echo "创建数据库表..."
	mysql -u root -p < database/create_tables.sql

# 帮助信息
help:
	@echo "可用的make目标:"
	@echo "  all        - 编译整个项目"
	@echo "  clean      - 清理构建文件"
	@echo "  rebuild    - 重新构建项目"
	@echo "  info       - 显示项目信息"
	@echo "  check      - 检查代码风格"
	@echo "  doc        - 生成文档"
	@echo "  install-db - 安装数据库表"
	@echo "  help       - 显示此帮助信息"

.PHONY: all clean rebuild info check doc install-db help