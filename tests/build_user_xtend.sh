
#!/bin/bash

# take an xe file an converit to a c header file
set -euo pipefail

xe_path="$1"
xb_path="${xe_path%.xe}.xb"

echo "xe_path: $xe_path"
echo "xb_path: $xb_path"

xobjdump --strip $xe_path
xobjdump --split $xb_path

# result in image_n0c0.bin
mv image_n0c0.bin xtend_blob.bin
xxd -i xtend_blob.bin > $2




