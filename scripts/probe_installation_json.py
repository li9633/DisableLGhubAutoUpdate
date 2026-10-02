import json

with open(r"C:\ProgramData\LGHUB\installation.json", "r") as f:
    data = json.load(f)

# 只看 services 和 processControl
print("=== processControl ===")
print(json.dumps(data.get("processControl", {}), indent=2, ensure_ascii=False)[:2000])
print()

print("=== services ===")
print(json.dumps(data.get("services", []), indent=2, ensure_ascii=False)[:3000])
print()

print("=== uninstaller ===")
print(json.dumps(data.get("uninstaller", {}), indent=2, ensure_ascii=False))