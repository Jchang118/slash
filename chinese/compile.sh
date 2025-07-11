#!/bin/bash

echo "正在编译会计系统..."
g++ -std=c++11 -o accounting_system accounting_system.cpp

if [ $? -eq 0 ]; then
    echo "编译成功！"
    echo "运行命令: ./accounting_system"
else
    echo "编译失败！"
    exit 1
fi