#!/bin/bash
# SSD1306监控服务卸载脚本

SERVICE_NAME="ssd1306-monitor"
SERVICE_PATH="/etc/systemd/system/${SERVICE_NAME}.service"

echo "=========================================="
echo "SSD1306监控服务卸载脚本"
echo "=========================================="
echo ""

# 检查是否以root运行
if [ "$EUID" -ne 0 ]; then 
    echo "错误: 请使用sudo运行此脚本"
    echo "用法: sudo ./uninstall_service.sh"
    exit 1
fi

# 检查服务是否存在
if [ ! -f "${SERVICE_PATH}" ]; then
    echo "服务文件不存在，可能已经卸载"
    exit 0
fi

echo "1. 停止服务..."
systemctl stop ${SERVICE_NAME} 2>/dev/null
echo "   ✓ 服务已停止"

echo ""
echo "2. 禁用服务..."
systemctl disable ${SERVICE_NAME} 2>/dev/null
echo "   ✓ 服务已禁用"

echo ""
echo "3. 删除服务文件..."
rm -f "${SERVICE_PATH}"
if [ $? -eq 0 ]; then
    echo "   ✓ 服务文件已删除"
else
    echo "   ✗ 删除失败！"
    exit 1
fi

echo ""
echo "4. 重新加载systemd配置..."
systemctl daemon-reload
if [ $? -eq 0 ]; then
    echo "   ✓ systemd配置已重新加载"
else
    echo "   ✗ 重新加载失败！"
    exit 1
fi

echo ""
echo "=========================================="
echo "卸载完成！"
echo "=========================================="


