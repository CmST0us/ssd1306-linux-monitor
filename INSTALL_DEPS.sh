#!/bin/bash
# 安装构建依赖脚本

echo "=========================================="
echo "安装 Debian 包构建依赖"
echo "=========================================="
echo ""

# 检查是否以root运行
if [ "$EUID" -ne 0 ]; then 
    echo "错误: 请使用sudo运行此脚本"
    echo "用法: sudo ./INSTALL_DEPS.sh"
    exit 1
fi

echo "更新软件包列表..."
apt-get update

echo ""
echo "安装构建依赖..."
apt-get install -y \
    devscripts \
    debhelper \
    cmake \
    build-essential \
    libc6-dev \
    dh-systemd

echo ""
echo "=========================================="
echo "依赖安装完成！"
echo "=========================================="
echo ""
echo "现在可以运行 ./build_deb.sh 构建deb包"

