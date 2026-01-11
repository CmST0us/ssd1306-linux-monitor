#!/bin/bash
# SSD1306监控服务安装脚本

SERVICE_NAME="ssd1306-monitor"
SERVICE_FILE="ssd1306-monitor.service"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
SERVICE_PATH="/etc/systemd/system/${SERVICE_NAME}.service"
EXECUTABLE="${BUILD_DIR}/SSD1306_Monitor"

echo "=========================================="
echo "SSD1306监控服务安装脚本"
echo "=========================================="
echo ""

# 检查是否以root运行
if [ "$EUID" -ne 0 ]; then 
    echo "错误: 请使用sudo运行此脚本"
    echo "用法: sudo ./install_service.sh"
    exit 1
fi

# 检查程序文件是否存在
if [ ! -f "${EXECUTABLE}" ]; then
    echo "错误: 找不到程序文件 ${EXECUTABLE}"
    echo "请先编译程序:"
    echo "  cd ${PROJECT_DIR}"
    echo "  mkdir -p build && cd build"
    echo "  cmake .."
    echo "  make"
    exit 1
fi

# 检查服务文件是否存在
if [ ! -f "${SCRIPT_DIR}/${SERVICE_FILE}" ]; then
    echo "错误: 找不到服务文件 ${SERVICE_FILE}"
    exit 1
fi

echo "1. 复制服务文件..."
# 更新服务文件中的路径
sed "s|WorkingDirectory=.*|WorkingDirectory=${PROJECT_DIR}|g" \
    "${SCRIPT_DIR}/${SERVICE_FILE}" | \
sed "s|ExecStart=.*|ExecStart=${EXECUTABLE}|g" > "${SERVICE_PATH}"

if [ $? -eq 0 ]; then
    echo "   ✓ 服务文件已复制到 ${SERVICE_PATH}"
else
    echo "   ✗ 复制失败！"
    exit 1
fi

echo ""
echo "2. 重新加载systemd配置..."
systemctl daemon-reload
if [ $? -eq 0 ]; then
    echo "   ✓ systemd配置已重新加载"
else
    echo "   ✗ 重新加载失败！"
    exit 1
fi

echo ""
echo "3. 启用服务（开机自启动）..."
systemctl enable ${SERVICE_NAME}
if [ $? -eq 0 ]; then
    echo "   ✓ 服务已启用（开机自启动）"
else
    echo "   ✗ 启用失败！"
    exit 1
fi

echo ""
echo "=========================================="
echo "安装完成！"
echo "=========================================="
echo ""
echo "服务管理命令："
echo "  启动服务:   sudo systemctl start ${SERVICE_NAME}"
echo "  停止服务:   sudo systemctl stop ${SERVICE_NAME}"
echo "  重启服务:   sudo systemctl restart ${SERVICE_NAME}"
echo "  查看状态:   sudo systemctl status ${SERVICE_NAME}"
echo "  查看日志:   sudo journalctl -u ${SERVICE_NAME} -f"
echo "  禁用自启:   sudo systemctl disable ${SERVICE_NAME}"
echo ""
echo "现在可以启动服务："
echo "  sudo systemctl start ${SERVICE_NAME}"


