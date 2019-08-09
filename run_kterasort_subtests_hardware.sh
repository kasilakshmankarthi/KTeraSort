#!/bin/bash
TARGET=$1
echo "Target chosen:" $TARGET

RES=./results
mkdir -p ${RES}

if [ "$TARGET" = "x86_64" ]; then
    CPU=$2
    echo "CPU tested:" $CPU

    TURBO=$3
    echo "TURBO testeed:" $TURBO

    ##Sorting
    ./binaries/QSortRnd.x86_64.${CPU}.g++.6.2.0.linux.elf --test=sort inputs/LFSRBased/input-16384.1bin 16384 0 1 > ${RES}/sort_2_14.${CPU}.${TURBO}.txt
    echo "========================" >> ${RES}/sort_2_14.${CPU}.${TURBO}.txt
    ./binaries/QSortRnd.x86_64.${CPU}.g++.6.2.0.linux.elf --test=sort inputs/LFSRBased/input-16384.2bin 16384 0 1 >> ${RES}/sort_2_14.${CPU}.${TURBO}.txt
    echo "========================" >> ${RES}/sort_2_14.${CPU}.${TURBO}.txt
    ./binaries/QSortRnd.x86_64.${CPU}.g++.6.2.0.linux.elf --test=sort inputs/LFSRBased/input-16384.3bin 16384 0 1 >> ${RES}/sort_2_14.${CPU}.${TURBO}.txt

    ./binaries/QSortRnd.x86_64.${CPU}.g++.6.2.0.linux.elf --test=sort inputs/LFSRBased/input-65536.1bin 65536 0 1 > ${RES}/sort_2_16.${CPU}.${TURBO}.txt
    echo "========================" >> ${RES}/sort_2_16.${CPU}.${TURBO}.txt
    ./binaries/QSortRnd.x86_64.${CPU}.g++.6.2.0.linux.elf --test=sort inputs/LFSRBased/input-65536.2bin 65536 0 1 >> ${RES}/sort_2_16.${CPU}.${TURBO}.txt
    echo "========================" >> ${RES}/sort_2_16.${CPU}.${TURBO}.txt
    ./binaries/QSortRnd.x86_64.${CPU}.g++.6.2.0.linux.elf --test=sort inputs/LFSRBased/input-65536.3bin 65536 0 1 >> ${RES}/sort_2_16.${CPU}.${TURBO}.txt

    ./binaries/QSortRnd.x86_64.${CPU}.g++.6.2.0.linux.elf --test=sort inputs/LFSRBased/input-4194304.1bin 4194304 0 1 > ${RES}/sort_2_22.${CPU}.${TURBO}.txt
    echo "========================" >> ${RES}/sort_2_22.${CPU}.${TURBO}.txt
    ./binaries/QSortRnd.x86_64.${CPU}.g++.6.2.0.linux.elf --test=sort inputs/LFSRBased/input-4194304.2bin 4194304 0 1 >> ${RES}/sort_2_22.${CPU}.${TURBO}.txt
    echo "========================" >> ${RES}/sort_2_22.${CPU}.${TURBO}.txt
    ./binaries/QSortRnd.x86_64.${CPU}.g++.6.2.0.linux.elf --test=sort inputs/LFSRBased/input-4194304.3bin 4194304 0 1 >> ${RES}/sort_2_22.${CPU}.${TURBO}.txt

    #Partitioning
    ./binaries/QTeraSort.x86_64.${CPU}.g++.6.2.0.linux.elf --test=partition inputs/LFSRBased/input-262144.1bin 262144 100 0 1 > ${RES}/part_2_18.${CPU}.${TURBO}.txt
    echo "========================" >> ${RES}/part_2_18.${CPU}.${TURBO}.txt
    ./binaries/QTeraSort.x86_64.${CPU}.g++.6.2.0.linux.elf --test=partition inputs/LFSRBased/input-262144.2bin 262144 100 0 1 >> ${RES}/part_2_18.${CPU}.${TURBO}.txt
    echo "========================" >> ${RES}/part_2_18.${CPU}.${TURBO}.txt
    ./binaries/QTeraSort.x86_64.${CPU}.g++.6.2.0.linux.elf --test=partition inputs/LFSRBased/input-262144.3bin 262144 100 0 1 >> ${RES}/part_2_18.${CPU}.${TURBO}.txt

    #for i in 0 1 2
    #do
     #Partitioning
     #./binaries/QTeraSort.x86_64.${CPU}.g++.6.2.0.linux.elf_${i} --test=partition inputs/LFSRBased/input-262144.1bin 262144 100 0 1 > ${RES}/part_2_18.${CPU}.${TURBO}.${i}.txt
     #echo "========================" >> ${RES}/part_2_18.${CPU}.${TURBO}.txt
     #./binaries/QTeraSort.x86_64.${CPU}.g++.6.2.0.linux.elf_${i} --test=partition inputs/LFSRBased/input-262144.2bin 262144 100 0 1 >> ${RES}/part_2_18.${CPU}.${TURBO}.${i}.txt
     #echo "========================" >> ${RES}/part_2_18.${CPU}.${TURBO}.txt
     #./binaries/QTeraSort.x86_64.${CPU}.g++.6.2.0.linux.elf_${i} --test=partition inputs/LFSRBased/input-262144.3bin 262144 100 0 1 >> ${RES}/part_2_18.${CPU}.${TURBO}.${i}.txt
    #done
fi

if [ "$TARGET" = "AARCH64" ]; then
    #Sorting
   ./binaries/QSortRnd.aarch64.g++.lin6.2.1.linux.elf --test=sort inputs/LFSRBased/input-16384.1bin 16384 0 1 > ${RES}/sort_2_14.${TARGET}.txt
    echo "========================" >> ${RES}/sort_2_14.${TARGET}.txt
    ./binaries/QSortRnd.aarch64.g++.lin6.2.1.linux.elf --test=sort inputs/LFSRBased/input-16384.2bin 16384 0 1 >> ${RES}/sort_2_14.${TARGET}.txt
    echo "========================" >> ${RES}/sort_2_14.${TARGET}.txt
    ./binaries/QSortRnd.aarch64.g++.lin6.2.1.linux.elf --test=sort inputs/LFSRBased/input-16384.3bin 16384 0 1 >> ${RES}/sort_2_14.${TARGET}.txt

    ./binaries/QSortRnd.aarch64.g++.lin6.2.1.linux.elf --test=sort inputs/LFSRBased/input-65536.1bin 65536 0 1 > ${RES}/sort_2_16.${TARGET}.txt
    echo "========================" >> ${RES}/sort_2_16.${TARGET}.txt
    ./binaries/QSortRnd.aarch64.g++.lin6.2.1.linux.elf --test=sort inputs/LFSRBased/input-65536.2bin 65536 0 1 >> ${RES}/sort_2_16.${TARGET}.txt
    echo "========================" >> ${RES}/sort_2_16.${TARGET}.txt
    ./binaries/QSortRnd.aarch64.g++.lin6.2.1.linux.elf --test=sort inputs/LFSRBased/input-65536.3bin 65536 0 1 >> ${RES}/sort_2_16.${TARGET}.txt

    ./binaries/QSortRnd.aarch64.g++.lin6.2.1.linux.elf --test=sort inputs/LFSRBased/input-4194304.1bin 4194304 0 1 > ${RES}/sort_2_22.${TARGET}.txt
    echo "========================" >> ${RES}/sort_2_22.${TARGET}.txt
    ./binaries/QSortRnd.aarch64.g++.lin6.2.1.linux.elf --test=sort inputs/LFSRBased/input-4194304.2bin 4194304 0 1 >> ${RES}/sort_2_22.${TARGET}.txt
    echo "========================" >> ${RES}/sort_2_22.${TARGET}.txt
    ./binaries/QSortRnd.aarch64.g++.lin6.2.1.linux.elf --test=sort inputs/LFSRBased/input-4194304.3bin 4194304 0 1 >> ${RES}/sort_2_22.${TARGET}.txt

    ##Partitioning
    ./binaries/QTeraSort.aarch64.g++.lin6.2.1.linux.elf --test=partition inputs/LFSRBased/input-262144.1bin 262144 100 0 1 > ${RES}/part_2_18.${TARGET}.txt
    echo "========================" >> ${RES}/part_2_18.${TARGET}.txt
    ./binaries/QTeraSort.aarch64.g++.lin6.2.1.linux.elf --test=partition inputs/LFSRBased/input-262144.2bin 262144 100 0 1 >> ${RES}/part_2_18.${TARGET}.txt
    echo "========================" >> ${RES}/part_2_18.${TARGET}.txt
    ./binaries/QTeraSort.aarch64.g++.lin6.2.1.linux.elf --test=partition inputs/LFSRBased/input-262144.3bin 262144 100 0 1 >> ${RES}/part_2_18.${TARGET}.txt

    #for i in 0 1 2
    #do
    ##Partitioning
    #./binaries/QTeraSort.aarch64.g++.lin6.2.1.linux.elf_${i} --test=partition inputs/LFSRBased/input-262144.1bin 262144 100 0 1 > ${RES}/part_2_18.${TARGET}.${i}.txt
    #echo "========================" >> ${RES}/part_2_18.${TARGET}.${i}.txt
    #./binaries/QTeraSort.aarch64.g++.lin6.2.1.linux.elf_${i} --test=partition inputs/LFSRBased/input-262144.2bin 262144 100 0 1 >> ${RES}/part_2_18.${TARGET}.${i}.txt
    #echo "========================" >> ${RES}/part_2_18.${TARGET}.${i}.txt
    #./binaries/QTeraSort.aarch64.g++.lin6.2.1.linux.elf_${i} --test=partition inputs/LFSRBased/input-262144.3bin 262144 100 0 1 >> ${RES}/part_2_18.${TARGET}.${i}.txt
    #done
fi

