#!/usr/bin/env sh

# Collect Project 1 ELF evidences into evidence.txt.
# one blank line separates each command's output and stderr is also merged into the file

{
    # data_sync
    file data_sync              ; echo
    readelf -h data_sync        ; echo
    readelf -S data_sync        ; echo
    readelf -l data_sync        ; echo
    readelf -s data_sync        ; echo
    readelf -d data_sync        ; echo
    nm -n data_sync             ; echo
    nm -D data_sync             ; echo
    objdump -h data_sync        ; echo
    objdump -t data_sync        ; echo
    objdump -T data_sync        ; echo
    objdump -d data_sync        ; echo
    objdump -p data_sync        ; echo

    # data_sync_v2
    file data_sync_v2           ; echo
    readelf -h data_sync_v2     ; echo
    readelf -S data_sync_v2     ; echo
    readelf -l data_sync_v2     ; echo
    readelf -s data_sync_v2     ; echo
    readelf -d data_sync_v2     ; echo
    nm -n data_sync_v2          ; echo
    nm -D data_sync_v2          ; echo
    objdump -h data_sync_v2     ; echo
    objdump -t data_sync_v2     ; echo
    objdump -T data_sync_v2     ; echo
    objdump -d data_sync_v2     ; echo
    objdump -p data_sync_v2     ; echo
} > evidence.txt 2>&1
