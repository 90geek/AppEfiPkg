#!/bin/bash

export WORKSPACE=`pwd`
export PACKAGES_PATH=$WORKSPACE/edk2:$WORKSPACE/LsFwSdk/:$WORKSPACE/AppEfiPkg/
export EDK_TOOLS_PATH=$WORKSPACE/edk2/BaseTools
VERSION=DEBUG
DESCRIPT_FILE_PATH=AppEfiPkg/AppEfiPkg.dsc
# DESCRIPT_FILE_PATH=AppEfiPkg/EmulatorApps.dsc



case $('uname') in
  Linux*)
    case $(uname -m) in
      x86_64)
         export GCC5_LOONGARCH64_PREFIX=loongarch64-unknown-linux-gnu-
         ;;
      loongarch64)
         export GCC5_LOONGARCH64_PREFIX=""
         ;;
    esac
    ;;
  *)
    echo Only support Linux.
    exit 1
    ;;
esac

if ! [ -z "$1" ]; then
  input=$1
fi

case $input in
  clean)
    rm -rf $WORKSPACE/Build/
    source $WORKSPACE/edk2/edksetup.sh --reconfig
    exit 0
    ;;
esac

source $WORKSPACE/edk2/edksetup.sh --reconfig
# --- Decide whether to (re)build BaseTools ------------------------------
need_build=false

for tool in build GenFv; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        need_build=true
        break
    fi
done

if [[ ! -d "${EDK_TOOLS_PATH}/Source/C/bin" ]]; then
    need_build=true
fi

# --- Build if necessary --------------------------------------------------
if $need_build; then
    printf 'Building BaseTools in %s …\n' "$EDK_TOOLS_PATH"
    make -C "$EDK_TOOLS_PATH"
else
    echo 'BaseTools already exist — skip.'
fi

source $WORKSPACE/edk2/edksetup.sh BaseTools

build -b $VERSION -t GCC5 -a LOONGARCH64 -p $DESCRIPT_FILE_PATH | tee build.log
ret=${PIPESTATUS[0]}
if [ $ret -ne 0 ]; then
    echo "Build failed with code $ret"
    exit $ret
fi

#PLATFORM_NAME=`ls -t Build/*/$VERSION*/FV/*.fd | head -1 | cut -d'/' -f2`

#cp "$(find Build/ -type f -name '*.fd' -printf '%T@ %p\n' | sort -n | tail -1 | cut -d' ' -f2-)" .
