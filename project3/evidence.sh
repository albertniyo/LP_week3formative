#!/usr/bin/env sh
# equivalence evidence into evidence.txt

{
    SIZE=1000

    echo "running Python module:"
    python main.py python $SIZE 2>&1
    echo ""

    echo "running C module:"
    python main.py c $SIZE 2>&1
    echo ""

    echo "---Test Results---"

    # format floats to %.6f to handle precision diffs
    python main.py python $SIZE | grep -v "backend:" | awk '{for(i=1;i<=NF;i++) if($i~/^-?[0-9]+\.[0-9]+$/) $i=sprintf("%.6f", $i); print}' > /tmp/test_py.txt
    python main.py c $SIZE | grep -v "backend:" | awk '{for(i=1;i<=NF;i++) if($i~/^-?[0-9]+\.[0-9]+$/) $i=sprintf("%.6f", $i); print}' > /tmp/test_c.txt

    if diff /tmp/test_py.txt /tmp/test_c.txt; then
        echo "pass: Python and C results are identical"
    else
        echo "fail: Results do not match"
    fi

    # clean up temp
    rm -f /tmp/test_py.txt /tmp/test_c.txt

} > evidence.txt
