TARGET=$1
echo "Target chosen: " $TARGET

if [ "$TARGET" = "x86_64" ] || [ "$TARGET" == "ALL" ]; then
    #Build x86_64 binaries
    #IvyBridge
   # make -f Makefile clean
    #make -f Makefile ARCH=x86_64 OS=linux CPU=ivybridge
    #mv QRandomGenMT.elf binaries/QRandomGenMT.x86_64.ivybridge.g++.6.2.0.linux.elf
    #mv QRandomGenLFSR.elf binaries/QRandomGenLFSR.x86_64.ivybridge.g++.6.2.0.linux.elf
    #mv QSortRnd.elf binaries/QSortRnd.x86_64.ivybridge.g++.6.2.0.linux.elf
    #mv QTeraSort.elf binaries/QTeraSort.x86_64.ivybridge.g++.6.2.0.linux.elf
    #echo "Completed building x86_64 ivybridge elf"

    ##Haswell
    #make -f Makefile clean
    #make -f Makefile ARCH=x86_64 OS=linux CPU=haswell
    #mv QSortRnd.elf binaries/QSortRnd.x86_64.haswell.g++.6.2.0.linux.elf
    #mv QTeraSort.elf binaries/QTeraSort.x86_64.haswell.g++.6.2.0.linux.elf
    #echo "Completed building x86_64 haswell elf"

    #Broadwell
    make -f Makefile clean
    make -f Makefile ARCH=x86_64 OS=linux CPU=broadwell
    mv QSortRnd.elf binaries/QSortRnd.x86_64.broadwell.g++.6.2.0.linux.elf
    mv QTeraSort.elf binaries/QTeraSort.x86_64.broadwell.g++.6.2.0.linux.elf
    echo "Completed building x86_64 broadwell elf"
fi

if [ "$TARGET" = "AARCH64" ] || [ "$TARGET" == "ALL" ]; then
    #Build aarch64 binaries
    make -f Makefile clean
    make -f Makefile ARCH=aarch64 OS=linux
    mv QSortRnd.elf binaries/QSortRnd.aarch64.g++.lin6.2.1.linux.elf
    mv QTeraSort.elf binaries/QTeraSort.aarch64.g++.lin6.2.1.linux.elf
    echo "Completed building AARCH64 elf"
fi
