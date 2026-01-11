#!/bin/bash
# 应用I2C 1MHz速度设置的脚本

DTBO_FILE="/boot/dtb/rockchip/overlay/rockchip-rk3588-i2c8-m2.dtbo"
BACKUP_FILE="/boot/dtb/rockchip/overlay/rockchip-rk3588-i2c8-m2.dtbo.backup"
NEW_DTBO="/home/pi/Project/SSD1306/rockchip-rk3588-i2c8-m2.dtbo"

echo "I2C-8 1MHz速度设置工具"
echo "================================"
echo ""

# 检查新文件是否存在
if [ ! -f "$NEW_DTBO" ]; then
    echo "错误: 找不到新的dtbo文件: $NEW_DTBO"
    exit 1
fi

# 备份原文件
if [ -f "$DTBO_FILE" ]; then
    echo "1. 备份原文件..."
    sudo cp "$DTBO_FILE" "$BACKUP_FILE"
    if [ $? -eq 0 ]; then
        echo "   ✓ 已备份到: $BACKUP_FILE"
    else
        echo "   ✗ 备份失败！"
        exit 1
    fi
else
    echo "警告: 原文件不存在: $DTBO_FILE"
fi

# 复制新文件
echo ""
echo "2. 应用新的设备树覆盖文件..."
sudo cp "$NEW_DTBO" "$DTBO_FILE"
if [ $? -eq 0 ]; then
    echo "   ✓ 文件已更新"
    echo "   ✓ 新文件大小: $(ls -lh "$DTBO_FILE" | awk '{print $5}')"
else
    echo "   ✗ 更新失败！"
    exit 1
fi

echo ""
echo "3. 验证文件..."
if [ -f "$DTBO_FILE" ]; then
    echo "   ✓ 文件存在"
    file "$DTBO_FILE"
else
    echo "   ✗ 文件不存在！"
    exit 1
fi

echo ""
echo "================================"
echo "完成！"
echo ""
echo "⚠️  重要提示："
echo "1. 需要重启系统才能使更改生效"
echo "2. SSD1306通常只支持最高400kHz，1MHz可能不稳定"
echo "3. 如果出现问题，可以恢复备份："
echo "   sudo cp $BACKUP_FILE $DTBO_FILE"
echo ""
echo "重启命令: sudo reboot"


