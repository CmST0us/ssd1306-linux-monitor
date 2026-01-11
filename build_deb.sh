#!/bin/bash
# SSD1306 Monitor DEB包构建脚本

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="${SCRIPT_DIR}"

echo "=========================================="
echo "SSD1306 Monitor DEB包构建脚本"
echo "=========================================="
echo ""

# 检查是否安装了必要的工具
echo "检查构建依赖..."
MISSING_DEPS=()

if ! command -v dpkg-buildpackage &> /dev/null; then
    MISSING_DEPS+=("devscripts")
fi

if ! command -v cmake &> /dev/null; then
    MISSING_DEPS+=("cmake")
fi

if [ ${#MISSING_DEPS[@]} -ne 0 ]; then
    echo "错误: 缺少以下依赖包:"
    for dep in "${MISSING_DEPS[@]}"; do
        echo "  - $dep"
    done
    echo ""
    echo "请运行以下命令安装:"
    echo "  sudo apt update"
    echo "  sudo apt install -y ${MISSING_DEPS[*]}"
    exit 1
fi

echo "✓ 所有依赖已满足"
echo ""

# 清理之前的构建
echo "清理之前的构建..."
rm -rf ${PROJECT_DIR}/build
rm -rf ${PROJECT_DIR}/debian/ssd1306-monitor
rm -f ${PROJECT_DIR}/../ssd1306-monitor_*.deb
rm -f ${PROJECT_DIR}/../ssd1306-monitor_*.dsc
rm -f ${PROJECT_DIR}/../ssd1306-monitor_*.tar.gz
rm -f ${PROJECT_DIR}/../ssd1306-monitor_*.changes
rm -f ${PROJECT_DIR}/../ssd1306-monitor_*.buildinfo
echo "✓ 清理完成"
echo ""

# 构建deb包
echo "开始构建DEB包..."
cd ${PROJECT_DIR}

# 使用 dpkg-buildpackage 构建
# -b: 只构建二进制包
# -us -uc: 不签名
dpkg-buildpackage -b -us -uc

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "构建成功！"
    echo "=========================================="
    echo ""
    
    # 查找生成的deb包
    DEB_FILE=$(ls -t ${PROJECT_DIR}/../ssd1306-monitor_*.deb 2>/dev/null | head -1)
    
    if [ -n "$DEB_FILE" ]; then
        echo "DEB包位置: $DEB_FILE"
        echo ""
        echo "安装命令:"
        echo "  sudo dpkg -i $DEB_FILE"
        echo ""
        echo "如果安装时提示依赖问题，请运行:"
        echo "  sudo apt install -f"
    fi
else
    echo ""
    echo "=========================================="
    echo "构建失败！"
    echo "=========================================="
    exit 1
fi
