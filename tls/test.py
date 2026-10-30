import json
from collections import defaultdict

with open("build/x25519.json") as f:
    trace = json.load(f)

events = defaultdict(float)

for event in trace["traceEvents"]:
    if event.get("ph") == "X":
        events[event.get("name", "unknown")] += event.get("dur", 0)

for name, duration in sorted(
    events.items(), key=lambda item: item[1], reverse=True
)[:20]:
    print(f"{duration / 1000:10.2f} ms  {name}")