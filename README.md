# KVS Linux - EasyFlash Key-Value Store Reader for PC

用于在 PC 上解析 MCU 端 EasyFlash KVS (Key-Value Store) 的 bin dump 文件，方便调试。

**注意**: 此工具基于 EasyFlash V4.x 设计，使用时请注意嵌入式端的 EasyFlash 版本。

## 功能特性

- 读取 bin 文件中的 KVS 数据
- 支持列出所有 key
- 支持读取/写入 key-value
- Kconfig 图形化配置 (menuconfig)

## 快速开始

```bash
# 编译
make

# 列出所有 keys
./kvs_linux --load-disk kvs_data.bin --list-keys

# 读取指定 key
./kvs_linux --load-disk kvs_data.bin -r /srf/pair_info

# 写入 key-value (注意: 仅修改 ef.disk，不会写入原 bin, 仅支持写入 str)
./kvs_linux --load-disk kvs_data.bin -w test_key test_value
```

## 命令行选项

| 选项 | 说明 |
|------|------|
| `--load-disk <file>` | 指定要加载的 bin 文件 (首次使用必须指定) |
| `--list-keys` | 列出所有 KVS keys |
| `-r <key>` | 读取指定 key 的值 |
| `-w <key> <value>` | 写入 key-value 到 cache |
| `-h` | 显示帮助信息 |

## 配置管理

```bash
# 图形化配置 (ncurses)
make menuconfig

# 查看当前配置
make config

# 生成默认配置
make defconfig

# 保存精简配置
make savedefconfig

# 清理构建
make clean
```

## 配置选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| Flash Size | MCU flash 大小 (256K/512K/1M) | 256K |
| User ENV Bank Num | 用户 ENV 区域 bank 数 | 0 |
| KVS ENV Bank Num | KVS ENV 区域 bank 数 | 2 |
| Flash Write Granularity | 写粒度 (1/8/32 bit) | 1 bit |
| Flash Erase Min Size | 最小擦除单位 | 0x1000 |
| Use External CRC32 | 使用外部 CRC32 模块 | y |
| Enable Debug | 启用调试输出 | n |

## 工作原理

1. 将 bin 文件加载到 RAM，模拟 flash 操作
2. `ef_port_read/write/erase` 对 RAM 进行读写/擦除，模拟 erase、read、write 操作
3. KVS 数据在 RAM 中的特定偏移地址下进行读写，write 操作后立即通过文件 IO 写入 `ef.disk`
4. KVS 起始地址: `FLASH_KVS_ENV_ADDR` 大小：`FLASH_USER_ENV_SIZE` (可通过 `make menuconfig` 修改)

## 项目结构

```
kvs_linux/
├── Kconfig              # Kconfig 配置定义
├── Makefile             # 构建配置
├── kvs_linux.c          # 主程序入口
├── easyflash/           # EasyFlash 源码
│   ├── inc/             # 头文件
│   ├── src/             # 源码 (不修改)
│   └── port/            # Linux porting 层
├── inc/                 # 本地头文件
│   ├── flash_map.h      # Flash 布局定义 (从 Kconfig 生成)
│   └── kvs_config.h     # Kconfig 自动生成
├── crc/                 # CRC32 模块
├── config/              # 配置文件目录
│   └── .config          # Kconfig 输出
└── cache/               # 运行时缓存 (ef.disk)
```

## 依赖

- Python 3.x with kconfiglib (`pip install kconfiglib`)
- ncurses (for menuconfig)

## 注意

- `--list-keys` 对于 blob 类型会显示 `blob @0xXXXXXX size` 而非完整内容
- `--load-disk` 用于导入外部 bin 文件作为 easyflash 的磁盘系统
    - 除非需要导入其他 bin 文件，或者程序提示 `No disk file (.bin) specified`，否则不需要二次导入
    - 导入 bin 文件后，程序将在 `cache` 路径下维护磁盘 `ef.disk`, 用户的读写将操作该磁盘而不是导入的 bin 文件

## 适用场景

- 从 MCU dump 出来的完整 bin 文件中提取 KVS 数据
