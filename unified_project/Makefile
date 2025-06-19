# Makefile for Sensor Data Collection System
# 传感器数据采集系统 Makefile
# 注意：此Makefile用于非IAR环境的编译测试，主要用于语法检查

# 项目配置
PROJECT_NAME = sensor_system
VERSION = 1.0.0

# 目录配置
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
EXAMPLES_DIR = examples

# 编译器配置
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I$(INC_DIR) -DDEBUG -DTEST_BUILD
LDFLAGS = 

# 源文件
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
HEADERS = $(wildcard $(INC_DIR)/*.h)

# 示例文件
EXAMPLE_SOURCES = $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLE_OBJECTS = $(EXAMPLE_SOURCES:$(EXAMPLES_DIR)/%.c=$(BUILD_DIR)/examples/%.o)

# 目标文件
TARGET = $(BUILD_DIR)/$(PROJECT_NAME)
EXAMPLE_TARGET = $(BUILD_DIR)/examples/sensor_examples

# 默认目标
.PHONY: all clean help test examples

all: $(TARGET)

# 编译主程序
$(TARGET): $(OBJECTS) | $(BUILD_DIR)
	@echo "链接 $(PROJECT_NAME)..."
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "编译完成: $@"

# 编译目标文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	@echo "编译 $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# 编译示例程序
examples: $(EXAMPLE_TARGET)

$(EXAMPLE_TARGET): $(EXAMPLE_OBJECTS) $(filter-out $(BUILD_DIR)/main.o,$(OBJECTS)) | $(BUILD_DIR)/examples
	@echo "链接示例程序..."
	$(CC) $(EXAMPLE_OBJECTS) $(filter-out $(BUILD_DIR)/main.o,$(OBJECTS)) -o $@ $(LDFLAGS)
	@echo "示例程序编译完成: $@"

$(BUILD_DIR)/examples/%.o: $(EXAMPLES_DIR)/%.c $(HEADERS) | $(BUILD_DIR)/examples
	@echo "编译示例 $<..."
	$(CC) $(CFLAGS) -DEXAMPLE_MAIN -c $< -o $@

# 创建构建目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/examples:
	mkdir -p $(BUILD_DIR)/examples

# 语法检查
test: $(HEADERS) $(SOURCES)
	@echo "执行语法检查..."
	$(CC) $(CFLAGS) -fsyntax-only $(SOURCES)
	@echo "语法检查通过！"

# 代码分析
analyze: $(SOURCES) $(HEADERS)
	@echo "执行代码分析..."
	@for file in $(SOURCES); do \
		echo "分析 $$file..."; \
		$(CC) $(CFLAGS) -fanalyzer -fsyntax-only $$file; \
	done
	@echo "代码分析完成！"

# 清理
clean:
	@echo "清理构建文件..."
	rm -rf $(BUILD_DIR)
	@echo "清理完成！"

# 安装（复制到系统目录）
install: $(TARGET)
	@echo "安装程序..."
	@echo "注意：此程序为嵌入式程序，不适合在PC上运行"
	@echo "请使用IAR EMBEDDED WORKBENCH进行实际编译和下载"

# 显示帮助信息
help:
	@echo "传感器数据采集系统 Makefile"
	@echo "=========================="
	@echo ""
	@echo "可用目标："
	@echo "  all       - 编译主程序（语法检查用）"
	@echo "  examples  - 编译示例程序"
	@echo "  test      - 执行语法检查"
	@echo "  analyze   - 执行代码分析"
	@echo "  clean     - 清理构建文件"
	@echo "  help      - 显示此帮助信息"
	@echo ""
	@echo "注意事项："
	@echo "- 此Makefile主要用于语法检查和代码分析"
	@echo "- 实际的嵌入式编译请使用IAR EMBEDDED WORKBENCH"
	@echo "- 生成的可执行文件无法在PC上正常运行"
	@echo ""
	@echo "项目信息："
	@echo "- 项目名称: $(PROJECT_NAME)"
	@echo "- 版本: $(VERSION)"
	@echo "- 源文件目录: $(SRC_DIR)"
	@echo "- 头文件目录: $(INC_DIR)"
	@echo "- 构建目录: $(BUILD_DIR)"

# 显示项目信息
info:
	@echo "项目信息"
	@echo "========"
	@echo "名称: $(PROJECT_NAME)"
	@echo "版本: $(VERSION)"
	@echo "源文件: $(SOURCES)"
	@echo "头文件: $(HEADERS)"
	@echo "编译器: $(CC)"
	@echo "编译选项: $(CFLAGS)"

# 统计代码行数
stats:
	@echo "代码统计"
	@echo "========"
	@echo "源文件数量: $(words $(SOURCES))"
	@echo "头文件数量: $(words $(HEADERS))"
	@echo ""
	@echo "代码行数统计："
	@wc -l $(SOURCES) $(HEADERS) | tail -1
	@echo ""
	@echo "详细统计："
	@wc -l $(SOURCES) $(HEADERS)