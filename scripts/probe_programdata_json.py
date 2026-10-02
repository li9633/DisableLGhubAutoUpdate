import json, os

base = r"C:\ProgramData\LGHUB"
files = ["current.json", "installation.json", "next.json", "version.json", "periodic_check.json"]

for fname in files:
    path = os.path.join(base, fname)
    if not os.path.exists(path):
        print(f"=== {fname} === NOT FOUND\n")
        continue
    with open(path, "r", encoding="utf-8") as f:
        content = f.read()
    print(f"=== {fname} ({len(content)} bytes) ===")
    try:
        obj = json.loads(content)
        if isinstance(obj, dict):
            for k, v in obj.items():
                v_str = json.dumps(v, ensure_ascii=False)
                print(f"  {k}: {v_str[:200]}")
        elif isinstance(obj, list):
            print(f"  (list, {len(obj)} items)")
            for i, item in enumerate(obj):
                print(f"  [{i}]: {json.dumps(item, ensure_ascii=False)[:200]}")
        else:
            print(f"  {obj}")
    except:
        print(content[:500])
    print()