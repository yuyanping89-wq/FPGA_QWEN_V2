"""BF16 bit-copy reference for sparse JSON table rows; no floating point math.
Usage: python embedding_reference.py ../golden/embedding_cases.json output.json
"""
import json, sys
from pathlib import Path
def lookup(table, ids):
    output = []
    for token in ids:
        if not 0 <= token < 151936:
            raise ValueError("invalid token")
        row = table[str(token)]
        if len(row) != 896 or any(not 0 <= x <= 65535 for x in row):
            raise ValueError("invalid BF16 row")
        output.append(list(row))
    return output
if __name__ == "__main__":
    case = json.loads(Path(sys.argv[1]).read_text(encoding="utf-8"))
    result = [lookup(case["table_rows"], c["token_ids"]) for c in case["cases"]]
    Path(sys.argv[2]).write_text(json.dumps(result), encoding="utf-8")
