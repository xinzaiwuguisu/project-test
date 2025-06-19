-- 传感器数据采集系统数据库建表脚本
-- 创建数据库
CREATE DATABASE IF NOT EXISTS sensor_data CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE sensor_data;

-- 创建用户并授权
CREATE USER IF NOT EXISTS 'sensor_user'@'localhost' IDENTIFIED BY 'sensor_pass';
GRANT ALL PRIVILEGES ON sensor_data.* TO 'sensor_user'@'localhost';
FLUSH PRIVILEGES;

-- 创建传感器1数据表（温湿度传感器）
CREATE TABLE IF NOT EXISTS sensor1_data (
    id INT AUTO_INCREMENT PRIMARY KEY COMMENT '主键ID',
    student_id VARCHAR(20) NOT NULL COMMENT '学号姓名缩写',
    sensor_name VARCHAR(16) NOT NULL COMMENT '传感器名称',
    temperature DECIMAL(5,2) NOT NULL COMMENT '温度值（摄氏度）',
    humidity DECIMAL(5,2) NOT NULL COMMENT '湿度值（百分比）',
    status VARCHAR(10) NOT NULL DEFAULT 'NORMAL' COMMENT '传感器状态',
    timestamp INT UNSIGNED NOT NULL COMMENT '时间戳',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '记录创建时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '记录更新时间',
    INDEX idx_student_id (student_id),
    INDEX idx_timestamp (timestamp),
    INDEX idx_created_at (created_at)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='传感器1数据表（温湿度传感器）';

-- 创建传感器2数据表（中断传感器）
CREATE TABLE IF NOT EXISTS sensor2_data (
    id INT AUTO_INCREMENT PRIMARY KEY COMMENT '主键ID',
    student_id VARCHAR(20) NOT NULL COMMENT '学号姓名缩写',
    sensor_name VARCHAR(16) NOT NULL COMMENT '传感器名称',
    interrupt_type TINYINT UNSIGNED NOT NULL COMMENT '中断类型',
    interrupt_count INT UNSIGNED NOT NULL DEFAULT 1 COMMENT '中断次数',
    status VARCHAR(10) NOT NULL DEFAULT 'NORMAL' COMMENT '传感器状态',
    timestamp INT UNSIGNED NOT NULL COMMENT '时间戳',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '记录创建时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '记录更新时间',
    INDEX idx_student_id (student_id),
    INDEX idx_sensor_name (sensor_name),
    INDEX idx_timestamp (timestamp),
    INDEX idx_created_at (created_at)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='传感器2数据表（中断传感器）';

-- 创建传感器状态统计视图
CREATE VIEW sensor_status_summary AS
SELECT 
    'sensor1' as sensor_type,
    student_id,
    sensor_name,
    status,
    COUNT(*) as count,
    MAX(created_at) as last_update
FROM sensor1_data 
GROUP BY student_id, sensor_name, status
UNION ALL
SELECT 
    'sensor2' as sensor_type,
    student_id,
    sensor_name,
    status,
    COUNT(*) as count,
    MAX(created_at) as last_update
FROM sensor2_data 
GROUP BY student_id, sensor_name, status;

-- 插入一些示例数据
INSERT INTO sensor1_data (student_id, sensor_name, temperature, humidity, status, timestamp) VALUES
('2021001ZS', 'TEMP_HUM', 25.60, 60.20, 'NORMAL', UNIX_TIMESTAMP()),
('2021001ZS', 'TEMP_HUM', 26.10, 58.50, 'NORMAL', UNIX_TIMESTAMP()),
('2021002LS', 'TEMP_HUM', 24.80, 62.30, 'NORMAL', UNIX_TIMESTAMP());

INSERT INTO sensor2_data (student_id, sensor_name, interrupt_type, interrupt_count, status, timestamp) VALUES
('2021001ZS', 'DOOR_SENSOR', 1, 1, 'NORMAL', UNIX_TIMESTAMP()),
('2021001ZS', 'MOTION_SENSOR', 2, 1, 'NORMAL', UNIX_TIMESTAMP()),
('2021002LS', 'DOOR_SENSOR', 1, 1, 'NORMAL', UNIX_TIMESTAMP());

-- 显示表结构
DESCRIBE sensor1_data;
DESCRIBE sensor2_data;

-- 显示示例数据
SELECT '=== 传感器1数据示例 ===' as info;
SELECT * FROM sensor1_data ORDER BY created_at DESC LIMIT 5;

SELECT '=== 传感器2数据示例 ===' as info;
SELECT * FROM sensor2_data ORDER BY created_at DESC LIMIT 5;

SELECT '=== 传感器状态统计 ===' as info;
SELECT * FROM sensor_status_summary;