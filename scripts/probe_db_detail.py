import sqlite3, os, json

DB = os.path.expandvars(r"%LOCALAPPDATA%\LGHUB\settings.db")
conn = sqlite3.connect(DB)
c = conn.cursor()

# 1. 表结构
c.execute("SELECT name, sql FROM sqlite_master WHERE type='table'")
for name, sql in c.fetchall():
    print(f"=== {name} ===")
    print(sql)
    c.execute(f"SELECT COUNT(*) FROM '{name}'")
    print(f"Rows: {c.fetchone()[0]}\n")

# 2. data 表的详细内容 - 只取 file 列的 JSON 顶层 key
c.execute("SELECT file FROM data LIMIT 1")
row = c.fetchone()
if row:
    try:
        obj = json.loads(row[0])
        print("=== data.file JSON 顶层 key ===")
        for k in obj.keys():
            v = obj[k]
            vtype = type(v).__name__
            if isinstance(v, (dict, list)):
                summary = f"{vtype}[{len(v)}]"
            elif isinstance(v, str) and len(str(v)) > 100:
                summary = f"str({len(str(v))} chars)"
            else:
                summary = repr(v)[:80]
            print(f"  {k}: {summary}")
    except:
        print("  (非 JSON)")

# 3. 查找与 update 相关的具体 key/value
print("\n=== 与更新/版本相关的字段 ===")
c.execute("SELECT file FROM data LIMIT 1")
raw = c.fetchone()[0]
obj = json.loads(raw)

update_related = {}
for k, v in obj.items():
    kl = k.lower()
    if any(kw in kl for kw in ["update", "version", "check", "periodic", "auto", "download", "release", "install", "upgrade", "next", "due", "schedule"]):
        update_related[k] = v

if update_related:
    for k, v in update_related.items():
        print(f"  {k} = {json.dumps(v, indent=4)[:500]}")
else:
    print("  (无直接匹配)")

# 4. 递归搜索嵌套 JSON 中的更新相关 key
print("\n=== 嵌套 JSON 中搜索更新关键字 ===")
def search_nested(obj, path=""):
    if isinstance(obj, dict):
        for k, v in obj.items():
            kl = k.lower()
            if any(kw in kl for kw in ["update", "version", "check", "periodic", "auto", "download", "release", "install", "upgrade", "next", "due", "schedule"]):
                val_str = json.dumps(v) if not isinstance(v, str) else v
                print(f"  {path}/{k} = {val_str[:200]}")
            search_nested(v, f"{path}/{k}")
    elif isinstance(obj, list):
        for i, item in enumerate(obj):
            search_nested(item, f"{path}[{i}]")

search_nested(obj)

conn.close()