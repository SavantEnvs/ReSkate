#!/usr/bin/env bash
# mayhem/build.sh — ReSkate (DingoSDK) fuzz harnesses + the portable regression tests.
# Compiles the Linux-portable parsers directly (the same source lists as the upstream CMake
# libraries dingosdk_frostbite / dingosdk_native_db and the multiplayer protocol sources), since
# the full CMake project is mostly Windows-only. Objects go under build/mayhem/ (gitignored: /build/).
set -euo pipefail
[ -n "${SOURCE_DATE_EPOCH:-}" ] || unset SOURCE_DATE_EPOCH
: "${SANITIZER_FLAGS=-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer}"
: "${DEBUG_FLAGS:=-g -gdwarf-3}"
: "${CC:=clang}" ; : "${CXX:=clang++}" ; : "${LIB_FUZZING_ENGINE:=-fsanitize=fuzzer}"
: "${MAYHEM_JOBS:=$(nproc)}"
: "${COVERAGE_FLAGS=}"
: "${SRC:=/mayhem}"
export SANITIZER_FLAGS DEBUG_FLAGS CC CXX LIB_FUZZING_ENGINE MAYHEM_JOBS COVERAGE_FLAGS
cd "$SRC"

OUT="$SRC/build/mayhem"
mkdir -p "$OUT/fuzz" "$OUT/test" "$OUT/bin"

INCS="-I$SRC -I$SRC/External -I$SRC/External/bcdec -I$SRC/External/lz4 -I$SRC/External/miniz -I$SRC/External/zstd/lib"
DEFS="-DMINIZ_NO_ARCHIVE_WRITING_APIS -DMINIZ_NO_ZLIB_APIS -DMINIZ_NO_STDIO -DMINIZ_NO_TIME -DZSTD_LEGACY_SUPPORT=0 -DZSTD_DISABLE_ASM"

PROJECT_SRCS=(
  Engine/Resource/binary_io.cpp Engine/Resource/binary_bundle.cpp Engine/Resource/cas_codec.cpp
  Engine/Resource/ebx_document.cpp Engine/Resource/ebx_fixup_repair.cpp Engine/Resource/ebx_merge.cpp
  Engine/Resource/material_grid.cpp Engine/Resource/shader_lookup.cpp Engine/Resource/bundle_ref_table.cpp
  Engine/Resource/ebx_writer.cpp Engine/Resource/toc.cpp Engine/Resource/texture.cpp
  Engine/Resource/mesh_set.cpp Engine/Resource/skeleton_asset.cpp
  Engine/Vfs/native_db.cpp
  Extension/Multiplayer/Net/protocol.cpp Extension/Multiplayer/Net/delta_codec.cpp
  Extension/Multiplayer/Net/wire_codec.cpp Extension/Throwdowns/throwdown_wire.cpp
  Extension/Plugins/plugin_manifest.cpp Extension/Plugins/plugin_effects.cpp Extension/Plugins/plugin_wave.cpp Engine/Core/Json/json.cpp
  Server/server_release.cpp Engine/Vfs/thunderstore_package.cpp
)
DEP_SRCS=( External/lz4/lz4.c External/miniz/miniz.c External/zstd/lib/common/*.c
           External/zstd/lib/compress/*.c External/zstd/lib/decompress/*.c )

# compile <outdir> <cflags> <cxxflags> <sources...>: one object per source, in parallel.
compile() {
  local dir="$1" cflags="$2" cxxflags="$3"; shift 3
  printf '%s\n' "$@" | DIR="$dir" CF="$cflags" CXF="$cxxflags" INCS="$INCS" DEFS="$DEFS" \
    xargs -P "$MAYHEM_JOBS" -I{} bash -c '
      s="{}"; o="$DIR/$(echo "$s" | tr / _).o"
      case "$s" in
        # vendored lz4 computes NULL+0 for empty blocks (benign); only that UBSan check is relaxed, only there
        External/lz4/lz4.c) $CC $CF -fno-sanitize=pointer-overflow $INCS $DEFS -c "$s" -o "$o" ;;
        *.c)   $CC  $CF  $INCS $DEFS -c "$s" -o "$o" ;;
        *)     $CXX $CXF $INCS $DEFS -c "$s" -o "$o" ;;
      esac'
}
objs() { local dir="$1"; shift; for s in "$@"; do printf '%s ' "$dir/$(echo "$s" | tr / _).o"; done; }

# 1) Sanitized + coverage-instrumented project objects; vendored codecs sanitized, not coverage-instrumented.
FUZZ_CXXFLAGS="-std=c++20 -O1 $SANITIZER_FLAGS -fsanitize=fuzzer-no-link $DEBUG_FLAGS"
compile "$OUT/fuzz" "-O1 $SANITIZER_FLAGS $DEBUG_FLAGS" "$FUZZ_CXXFLAGS" "${PROJECT_SRCS[@]}"
compile "$OUT/fuzz" "-O1 $SANITIZER_FLAGS $DEBUG_FLAGS" "$FUZZ_CXXFLAGS" "${DEP_SRCS[@]}"
rm -f "$OUT/fuzz/libreskate_fuzz.a"
ar rcs "$OUT/fuzz/libreskate_fuzz.a" $(objs "$OUT/fuzz" "${PROJECT_SRCS[@]}" "${DEP_SRCS[@]}")

$CC $SANITIZER_FLAGS $DEBUG_FLAGS -c mayhem/lsan_off.cc -o "$OUT/fuzz/lsan_off.o" -x c++
$CC $SANITIZER_FLAGS $DEBUG_FLAGS -c "$STANDALONE_FUZZ_MAIN" -o "$OUT/fuzz/standalone_main.o"

# 2) Each harness twice: libFuzzer binary and run-once standalone reproducer.
for h in mayhem/harnesses/fuzz_*.cpp; do
  name="$(basename "$h" .cpp)"
  $CXX $FUZZ_CXXFLAGS $INCS $DEFS -Imayhem/harnesses -c "$h" -o "$OUT/fuzz/$name.o"
  $CXX $SANITIZER_FLAGS $DEBUG_FLAGS $LIB_FUZZING_ENGINE "$OUT/fuzz/$name.o" "$OUT/fuzz/lsan_off.o" \
       "$OUT/fuzz/libreskate_fuzz.a" -o "$OUT/bin/$name"
  $CXX $SANITIZER_FLAGS $DEBUG_FLAGS "$OUT/fuzz/standalone_main.o" "$OUT/fuzz/$name.o" "$OUT/fuzz/lsan_off.o" \
       "$OUT/fuzz/libreskate_fuzz.a" -o "$OUT/bin/$name-standalone"
done

# 3) Upstream's own regression tests over the same code, NORMAL flags (no sanitizers).
TEST_CXXFLAGS="-std=c++20 -O1 -include cstdint $COVERAGE_FLAGS"
compile "$OUT/test" "-O1 $COVERAGE_FLAGS" "$TEST_CXXFLAGS" "${PROJECT_SRCS[@]}" "${DEP_SRCS[@]}"
TEST_OBJS=$(objs "$OUT/test" "${PROJECT_SRCS[@]}" "${DEP_SRCS[@]}")
for t in shader_lookup_tests ebx_fixup_repair_tests skinned_mesh_tests; do
  $CXX $TEST_CXXFLAGS $INCS $DEFS "Engine/Resource/Test/$t.cpp" $TEST_OBJS $COVERAGE_FLAGS -o "$OUT/test/$t"
done
echo "build.sh: done"
