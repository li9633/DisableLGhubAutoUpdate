# 架构设计方案

## 1. 项目定位

DisableLGhubAutoUpdate 是一个用于禁止 LG Hub 软件自动更新的 Windows 开源工具。项目采用 **分层架构**，Core 和 UI 完全解耦，便于二次开发和独立集成。

## 2. 禁止更新策略分析

### 2.1 LG Hub 更新机制探索

通过对 LG Hub 安装目录 (`%ProgramData%\LGHUB\`、`%LocalAppData%\LGHUB\`) 的完整探测，
梳理出 LG Hub 的关键组件：

| 组件 | 路径 / 名称 | 用途 |
|------|-------------|------|
| 更新服务 | `LGHUBUpdaterService` | 后台更新检查与下载 |
| 主服务 | `LGHUB` (LGHUBService) | LG Hub 主进程后台服务 |
| 更新程序 | `lghub_updater.exe` | 更新执行体（也是卸载程序） |
| 代理程序 | `lghub_agent.exe` | 托盘/代理进程 |
| 安装器 | `lghub_installer` | 进程标识（processControl） |
| 定期检查配置 | `periodic_check.json` | 更新频率与开关 |
| 待更新信息 | `next.json` | 下一版本更新元数据（平时为空） |
| 安装清单 | `installation.json` | 服务定义、文件列表 |
| 应用设置 | `settings.db` (SQLite) | 用户偏好（非更新相关） |

### 2.2 策略总览

| # | 策略 | 目标 | 是否实施 | 说明 |
|---|------|------|---------|------|
| 1 | **禁用更新服务** | `LGHUBUpdaterService` | ✅ | 将启动类型改为 `SERVICE_DISABLED` |
| 2 | **修改定期检查配置** | `periodic_check.json` | ✅ | `checkPeriodically: false` 等 |
| 3 | **锁定更新元数据** | `next.json` | ✅ 新增 | 清空并设为只读，阻止写入更新信息 |
| 4 | ~~防火墙阻止网络~~ | — | ❌ 不采用 | 阻止网络会导致鼠标配置加载异常 |

> **重要设计原则**：只阻止更新行为，不影响 LG Hub 的正常使用（鼠标配置加载、灯光同步等功能）。

### 2.3 策略 3 详述：`next.json` 锁定

`next.json` 是 LG Hub 在检测到新版本后写入的文件，内容格式如下：

```json
{"branch": "", "buildId": "", "depots": [], "version": ""}
```

平时为空对象，当 LG Hub 检测到更新后，此文件会被填充更新元数据，进而触发更新流程。

**操作方案**：
1. 将 `next.json` 内容设为 `{}`
2. 使用 Windows ACL 剥夺 `lghub_updater.exe` 对该文件的写入权限
3. 即使更新检查通过，LG Hub 也无法写入更新信息，更新流程被阻断

---

## 3. 分层架构

```
┌──────────────────────────────────────────────┐
│                   GUI 层                      │
│       (面向普通用户，提供一键式图形界面)        │
│          [未来: Qt / ImGui / Web]             │
├──────────────────────────────────────────────┤
│                   CLI 层                      │
│     (当前交互入口，对 Core 层做薄封装)          │
├──────────────────────────────────────────────┤
│                  Core 层                      │
│  ┌────────────┐ ┌────────────┐ ┌──────────┐  │
│  │  Service   │ │  Config    │ │  File     │  │
│  │  Manager   │ │  Manager   │ │  Locker   │  │
│  └────────────┘ └────────────┘ └──────────┘  │
│         独立编译、零 UI 依赖、纯逻辑            │
└──────────────────────────────────────────────┘
```

### 层级职责

| 层 | 职责 | 依赖 |
|----|------|------|
| **Core** | 所有业务逻辑：服务管理、配置修改、文件锁定 | 仅 Windows API |
| **CLI** | 命令行交互、参数解析、结果输出 | Core |
| **GUI** | 图形界面交互（未来） | Core + GUI 框架 |

---

## 4. 目录结构

```
DisableLGhubAutoUpdate/
│
├── core/                                # ★ Core 库（零 UI 依赖）
│   ├── include/disable_lghub/
│   │   ├── core.hpp                     # 统一导出头文件
│   │   ├── types.hpp                    # 公共类型定义（ErrorCode 等）
│   │   ├── service_manager.hpp          # 服务管理器接口
│   │   ├── config_manager.hpp           # 配置管理器接口（JSON/SQLite）
│   │   └── file_locker.hpp              # 文件锁定器接口（ACL）
│   ├── src/
│   │   ├── service_manager.cpp
│   │   ├── config_manager.cpp
│   │   ├── file_locker.cpp
│   │   └── win32_detail.hpp             # Win32 内部实现（不对外暴露）
│   └── CMakeLists.txt
│
├── cli/                                 # CLI 入口层
│   ├── main.cpp
│   └── CMakeLists.txt
│
├── gui/                                 # GUI 入口层（Phase 2）
│   ├── main.cpp
│   └── CMakeLists.txt
│
├── tests/                               # 单元测试（Phase 3）
│   ├── test_service_manager.cpp
│   ├── test_config_manager.cpp
│   ├── test_file_locker.cpp
│   └── CMakeLists.txt
│
├── scripts/                             # 辅助脚本
│   ├── probe_lghub_db.py                # 探测 settings.db
│   ├── probe_db_detail.py               # 详细分析 settings.db
│   ├── probe_programdata_json.py        # 探测 ProgramData JSON
│   └── probe_installation_json.py       # 分析 installation.json
│
├── cmake/                               # CMake 辅助模块
│   └── FetchNlohmannJson.cmake          # 第三方库获取
│
├── docs/                                # 项目文档
│   └── design.md                        # 本设计文档
│
├── CMakeLists.txt                       # 顶层 CMake 构建
├── CMakePresets.json                    # CMake 预设配置
├── .gitignore
├── LICENSE
└── README.md
```

---

## 5. Core 层 API 设计

### 5.1 公共类型（types.hpp）

```cpp
namespace lghub::core {

enum class ErrorCode {
    Success = 0,
    ServiceNotFound,
    ServiceAccessDenied,
    ServiceStartFailed,
    ServiceStopFailed,
    ServiceConfigFailed,
    ServiceQueryFailed,
    ConfigFileNotFound,
    ConfigWriteFailed,
    ConfigParseFailed,
    FileLockFailed,
    FileUnlockFailed,
    UnknownError,
};

// Result<T> 简洁错误处理，兼容 C++17
// 优先级：std::expected (C++23) → tl::expected → 自实现简版 Result<T, E>
template <typename T>
using Result = std::expected<T, ErrorCode>;

} // namespace lghub::core
```

### 5.2 服务管理器（service_manager.hpp）

```cpp
namespace lghub::core {

class ServiceManager {
public:
    static constexpr std::string_view UPDATER_SERVICE_NAME = "LGHUBUpdaterService";

    // 将指定服务设为禁用 (SERVICE_DISABLED)
    static Result<void> disable_service(std::string_view service_name);

    // 停止并禁用服务（一步到位）
    static Result<void> stop_and_disable(std::string_view service_name);

    // 获取服务对应可执行文件的完整路径
    static Result<std::filesystem::path> get_binary_path(
        std::string_view service_name
    );
};

} // namespace lghub::core
```

### 5.3 配置管理器（config_manager.hpp）

```cpp
namespace lghub::core {

class ConfigManager {
public:
    // 默认 LG Hub 公共数据目录
    static constexpr std::string_view PROGRAMDATA_DIR = R"(C:\ProgramData\LGHUB)";

    // 禁用定期更新检查 (修改 periodic_check.json)
    static Result<void> disable_periodic_check(
        const std::filesystem::path& dir = PROGRAMDATA_DIR
    );

    // 清空 next.json 并锁定它
    static Result<void> clear_and_lock_next_update(
        const std::filesystem::path& dir = PROGRAMDATA_DIR
    );
};

} // namespace lghub::core
```

### 5.4 文件锁定器（file_locker.hpp）

```cpp
namespace lghub::core {

class FileLocker {
public:
    // 剥夺指定进程对目标文件的写入权限（通过 ACL）
    static Result<void> deny_write_access(
        const std::filesystem::path& file_path,
        std::wstring_view target_executable  // e.g. L"lghub_updater.exe"
    );

    // 恢复目标文件的完整访问权限
    static Result<void> restore_access(
        const std::filesystem::path& file_path
    );
};

} // namespace lghub::core
```

---

## 6. CLI 层调用示例

```cpp
#include <disable_lghub/core.hpp>
#include <iostream>

int main() {
    using namespace lghub::core;
    namespace fs = std::filesystem;

    // 策略 1: 停止并禁用更新服务
    if (auto r = ServiceManager::stop_and_disable(
            ServiceManager::UPDATER_SERVICE_NAME); !r) {
        std::cerr << "服务禁用失败: " << static_cast<int>(r.error()) << "\n";
    }

    // 策略 2: 修改定期检查配置
    if (auto r = ConfigManager::disable_periodic_check(); !r) {
        std::cerr << "配置修改失败: " << static_cast<int>(r.error()) << "\n";
    }

    // 策略 3: 锁定 next.json
    if (auto r = ConfigManager::clear_and_lock_next_update(); !r) {
        std::cerr << "next.json 锁定失败: " << static_cast<int>(r.error()) << "\n";
    }

    std::cout << "LG Hub 自动更新已禁止。\n";
    return 0;
}
```

---

## 7. 关键技术决策

| 决策项 | 选择 | 理由 |
|--------|------|------|
| **Core 编译方式** | 静态库 (`.lib`) | 独立组件，静态库最自然，下游直接链接 |
| **错误处理** | 自定义 `Result<T, ErrorCode>` | 零额外依赖，兼容 C++17 |
| **C++ 标准** | C++17 | MSVC 完全支持，社区兼容性好 |
| **PIMPL** | 是 | Core 头文件不暴露 `<Windows.h>`，下游开发者无污染 |
| **第三方库获取** | CMake FetchContent | 现代 CMake 标准做法，无需手动管理依赖 |
| **第三方库** | nlohmann/json | 仅 ConfigManager 内部使用，Header-only，不污染下游 |
| **GUI 框架** | 未定（Phase 2） | 当前专注 Core + CLI，GUI 后续独立开发 |
| **测试框架** | Catch2 / GoogleTest | FetchContent 引入，不影响下游用户 |
| **代码风格** | 类 STL 风格 (`snake_case`) | 符合现代 C++ 社区惯例 |
| **网络隔离** | 不采用 | 阻止网络通讯会破坏鼠标配置加载功能 |

---

## 8. 与旧代码的映射关系

| 旧文件（将被废弃） | 重构后（Core 层） |
|---------------------|-------------------|
| `LGServices.hpp` / `.cpp` | 拆分为各 Manager，类型定义移入 `types.hpp` |
| `ChangeLGServices.hpp` / `.cpp` | → `service_manager.hpp` / `.cpp` |
| `AddLGFWRule.hpp` / `.cpp` | → **废弃**（防火墙策略不再采用） |
| `LGhubJson.hpp` / `.cpp` | → `config_manager.hpp` / `.cpp` |
| `GlobalErrorCode.hpp` / `.cpp` | → 整合到 `Result<T>` 错误处理 |
| `Main.hpp` / `Main.cpp` | → 迁移至 `cli/main.cpp` |

---

## 9. 实施路线图

| 阶段 | 内容 | 预计产出 |
|------|------|----------|
| **Phase 1** | 建立项目骨架：目录结构、顶层 CMake、Core 静态库目标、types.hpp | 可编译的空 Core 库 |
| **Phase 2** | 迁移 ServiceManager | 服务禁用功能完工 |
| **Phase 3** | 迁移 ConfigManager | JSON 配置修改功能完工 |
| **Phase 4** | 实现 FileLocker | ACL 文件锁定功能完工 |
| **Phase 5** | 重写 CLI 层 | 新的命令行入口 |
| **Phase 6** | 清理旧代码 | 删除旧 .sln/.vcxproj/旧源码 |
| **Phase 7** | 单元测试 | test/ 目录完整测试覆盖 |
| **Phase 8** | GUI 层（可选） | 图形界面 |