#!/usr/bin/env python3
"""
Summarize AutoSA reference generation into Markdown tables and CSV.
Reads autosa_hls_refs/generation_summary.json and writes:
- docs/AUTOSA_REFERENCE_TABLES.md: per-kernel tables of spacetime and params
- docs/AUTOSA_REFERENCE_TABLES.csv: flat CSV of all references
"""
import json
from pathlib import Path

ROOT = Path('/workspaces/mlir-systolic')
SUMMARY_JSON = ROOT / 'autosa_hls_refs' / 'generation_summary.json'
DOC_MD = ROOT / 'docs' / 'AUTOSA_REFERENCE_TABLES.md'
DOC_CSV = ROOT / 'docs' / 'AUTOSA_REFERENCE_TABLES.csv'

COLUMNS = ['kernel','spacetime','array_part','latency','simd','path']


def load_summary():
    if not SUMMARY_JSON.exists():
        raise FileNotFoundError(f"Summary JSON not found: {SUMMARY_JSON}")
    with open(SUMMARY_JSON, 'r') as f:
        data = json.load(f)
    return data


def pick_kernel_cpp(files):
    # Prefer kernel_kernel.cpp; if not present, fall back to first file
    for p in files:
        if p.endswith('kernel_kernel.cpp'):
            return p
    return files[0] if files else ''


def rel_path(p):
    try:
        return str(Path(p).relative_to(ROOT))
    except Exception:
        return p


def to_rows(items):
    rows = []
    for it in items:
        rows.append({
            'kernel': it.get('kernel',''),
            'spacetime': it.get('spacetime',''),
            'array_part': it.get('array_part',''),
            'latency': it.get('latency',''),
            'simd': it.get('simd',''),
            'path': rel_path(pick_kernel_cpp(it.get('files',[])))
        })
    return rows


def write_markdown(rows):
    by_kernel = {}
    for r in rows:
        by_kernel.setdefault(r['kernel'], []).append(r)
    # sort within kernel
    for k in by_kernel:
        by_kernel[k].sort(key=lambda x: (x['spacetime'], str(x['array_part']), str(x['latency']), str(x['simd'])))

    lines = []
    lines.append('# AutoSA 参考参数表（按 kernel 分组）\n')
    lines.append('本表格基于 `autosa_hls_refs/generation_summary.json` 自动生成，展示每个参考代码的关键参数与路径。\n')
    lines.append('注：路径为 `kernel_kernel.cpp` 所在文件相对路径。\n')

    for kernel in sorted(by_kernel.keys()):
        entries = by_kernel[kernel]
        lines.append(f"\n## {kernel}（{len(entries)} 条）\n")
        lines.append('| spacetime | array_part | latency | simd | path |\n')
        lines.append('|-----------:|-----------|---------|------|------|\n')
        for r in entries:
            lines.append(f"| {r['spacetime']} | {r['array_part']} | {r['latency']} | {r['simd']} | {r['path']} |")
    DOC_MD.parent.mkdir(parents=True, exist_ok=True)
    DOC_MD.write_text('\n'.join(lines))


def write_csv(rows):
    DOC_CSV.parent.mkdir(parents=True, exist_ok=True)
    with open(DOC_CSV, 'w') as f:
        # header
        f.write(','.join(COLUMNS) + '\n')
        for r in rows:
            vals = [str(r.get(col, '')) for col in COLUMNS]
            # naive CSV escaping: wrap with quotes if comma present
            def esc(x):
                if ',' in x or '"' in x:
                    return '"' + x.replace('"','""') + '"'
                return x
            f.write(','.join(esc(v) for v in vals) + '\n')


def main():
    data = load_summary()
    rows = to_rows(data)
    write_markdown(rows)
    write_csv(rows)
    print(f"Wrote {DOC_MD} and {DOC_CSV}. Total rows: {len(rows)}")

if __name__ == '__main__':
    main()
