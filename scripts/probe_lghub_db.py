# probe_lghub_db.py
# 探测 LG Hub 的 settings.db，查找与自动更新相关的配置
import sqlite3
import os

DB_PATH = os.path.expandvars(
    r"%LOCALAPPDATA%\LGHUB\settings.db"
)

UPDATE_KEYWORDS = [
    "update", "version", "check", "download", "periodic",
    "interval", "auto", "upgrade", "patch", "release",
    "install", "setup", "next", "due", "schedule",
]

def probe():
    print(f"[*] 数据库路径: {DB_PATH}")
    print(f"[*] 文件大小: {os.path.getsize(DB_PATH):,} bytes\n")

    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    # 1. 获取所有表名
    cursor.execute("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name")
    tables = [row[0] for row in cursor.fetchall()]
    print(f"[+] 发现 {len(tables)} 个表: {tables}\n")

    # 2. 逐个表分析
    for table in tables:
        print(f"{'='*60}")
        print(f"  表: {table}")
        print(f"{'='*60}")

        # 列信息
        cursor.execute(f"PRAGMA table_info('{table}')")
        columns = cursor.fetchall()
        col_names = [c[1] for c in columns]
        print(f"  列 ({len(columns)}): {col_names}")

        # 行数
        cursor.execute(f"SELECT COUNT(*) FROM '{table}'")
        row_count = cursor.fetchone()[0]
        print(f"  行数: {row_count}")

        # 3. 查找与更新相关的列
        related_cols = [
            c for c in col_names
            if any(kw in c.lower() for kw in UPDATE_KEYWORDS)
        ]
        if related_cols:
            print(f"  ⚠  [匹配关键字] 相关列: {related_cols}")

        # 4. 展示数据 (最多30行)
        if row_count > 0:
            cursor.execute(f"SELECT * FROM '{table}' LIMIT 30")
            rows = cursor.fetchall()
            for i, row in enumerate(rows):
                print(f"  [{i}] {dict(zip(col_names, row))}")
        else:
            print("  (空表)")

        # 5. 如果列中可能包含 JSON，尝试解析
        for col_name in col_names:
            if any(kw in col_name.lower() for kw in ["json", "data", "value", "config", "settings"]):
                cursor.execute(f"SELECT DISTINCT `{col_name}` FROM '{table}' WHERE `{col_name}` IS NOT NULL LIMIT 10")
                vals = cursor.fetchall()
                for (val,) in vals:
                    if isinstance(val, str) and val.strip().startswith("{"):
                        print(f"\n  📄 [{col_name}] 疑似 JSON, 长度={len(val)}")
                        print(f"     {val[:300]}...")
                        break

        print()

    # 6. 额外: 搜索所有列值中的关键字
    print(f"\n{'='*60}")
    print("  全文关键字搜索")
    print(f"{'='*60}")
    for table in tables:
        cursor.execute(f"PRAGMA table_info('{table}')")
        col_names = [c[1] for c in cursor.fetchall()]
        for col_name in col_names:
            for kw in UPDATE_KEYWORDS:
                try:
                    cursor.execute(
                        f"SELECT `{col_name}` FROM '{table}' "
                        f"WHERE CAST(`{col_name}` AS TEXT) LIKE ? LIMIT 3",
                        (f"%{kw}%",)
                    )
                    hits = cursor.fetchall()
                    for (val,) in hits:
                        print(f"  [{table}.{col_name}] LIKE '%{kw}%' → {str(val)[:200]}")
                except Exception:
                    pass

    conn.close()

if __name__ == "__main__":
    probe()