#!/bin/sh

ROOTDIR=`dirname $0`
BINDIR=$ROOTDIR/build-release

echo ""
echo "=== Day 01, Part A ==="
time $BINDIR/day01a $ROOTDIR/day01/input.txt
echo "=== Day 01, Part B ==="
time $BINDIR/day01b $ROOTDIR/day01/input.txt

echo ""
echo "=== Day 02, Part A ==="
time $BINDIR/day02a $ROOTDIR/day02/input.txt
echo "=== Day 02, Part B ==="
time $BINDIR/day02b $ROOTDIR/day02/input.txt

echo ""
echo "=== Day 03, Part A ==="
time $BINDIR/day03a $ROOTDIR/day03/input.txt
echo "=== Day 03, Part B ==="
time $BINDIR/day03b $ROOTDIR/day03/input.txt

echo ""
echo "=== Day 04, Part A ==="
time $BINDIR/day04a $ROOTDIR/day04/input.txt
echo "=== Day 04, Part B ==="
time $BINDIR/day04b $ROOTDIR/day04/input.txt

echo ""
echo "=== Day 05, Part A ==="
time $BINDIR/day05a $ROOTDIR/day05/input.txt
echo "=== Day 05, Part B ==="
time $BINDIR/day05b $ROOTDIR/day05/input.txt

echo ""
echo "=== Day 06, Part A ==="
time $BINDIR/day06a $ROOTDIR/day06/input.txt
echo "=== Day 06, Part B ==="
time $BINDIR/day06b $ROOTDIR/day06/input.txt

echo ""
echo "=== Day 07, Part A ==="
time $BINDIR/day07a $ROOTDIR/day07/input.txt
echo "=== Day 07, Part B ==="
time $BINDIR/day07b $ROOTDIR/day07/input.txt

echo ""
echo "=== Day 08, Part A ==="
time $BINDIR/day08a $ROOTDIR/day08/input.txt
echo "=== Day 08, Part B ==="
time $BINDIR/day08b $ROOTDIR/day08/input.txt

echo ""
echo "=== Day 09, Part A ==="
time $BINDIR/day09a $ROOTDIR/day09/input.txt
echo "=== Day 09, Part B ==="
time $BINDIR/day09b $ROOTDIR/day09/input.txt

echo ""
echo "=== Day 10, Part A ==="
time $BINDIR/day10a $ROOTDIR/day10/input.txt
echo "=== Day 10, Part B ==="
time $BINDIR/day10b $ROOTDIR/day10/input.txt
