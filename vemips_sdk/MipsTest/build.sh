if [ -z "${VEMIPS_SDK}" ]; then
	VEMIPS_SDK=../MIPS_SDK
fi
if [ -z "${VEMIPS_CC}" ]; then
	VEMIPS_CC="${VEMIPS_SDK}/bin/clang"
fi
if [ -z "${VEMIPS_CXX}" ]; then
	VEMIPS_CXX="${VEMIPS_SDK}/bin/clang++"
fi

COMMON_FLAGS=( \
	-std=gnu++23 \
	-ffunction-sections \
	-fdata-sections \
	-fasynchronous-unwind-tables \
	-funwind-tables \
	-fexceptions \
	-fcxx-exceptions \
	-Wno-assume \
)

DEBUG_FLAGS=( \
	"${COMMON_FLAGS[@]}" \
	-g3 \
	-Og \
)
	
RELEASE_FLAGS=( \
	"${COMMON_FLAGS[@]}" \
	-Wl,--gc-sections \
	-Wl,--icf=all \
	-Wl,--strip-all \
	-flto=full \
)

HOST_FLAGS=( \
	-fuse-ld=lld \
)

HOST_RELEASE_FLAGS=( \
	-O3 \
)

TARGET_FLAGS=( \
	-mno-check-zero-division \
	-mcompact-branches=always \
)

TARGET_RELEASE_FLAGS=( \
	-Wl,--discard-all \
	-Wl,-znorelro \
	-Wl,--eh-frame-hdr \
	-Wl,--fat-lto-objects \
	-Wl,--lto-whole-program-visibility \
	-Os \
)

any_job=false

get_target_host() {
	compiler="clang++"
	flags+=( "${RELEASE_FLAGS[@]}" )
	flags+=( "${HOST_FLAGS[@]}" )
	flags+=( "${HOST_RELEASE_FLAGS[@]}" )
	ext=".exe"
}

get_target_host_debug() {
	compiler="clang++"
	flags+=( "${DEBUG_FLAGS[@]}" )
	flags+=( "${HOST_FLAGS[@]}" )
	ext="D.exe"
}

get_target_target() {
	compiler="$VEMIPS_CXX"
	flags+=( "${RELEASE_FLAGS[@]}" )
	flags+=( "${TARGET_FLAGS[@]}" )
	flags+=( "${TARGET_RELEASE_FLAGS[@]}" )
	ext=".bin"
}

get_target_target_debug() {
	compiler="$VEMIPS_CXX"
	flags+=( "${DEBUG_FLAGS[@]}" )
	flags+=( "${TARGET_FLAGS[@]}" )
	ext="D.bin"
}

exec_job() {
	echo ${@:2} "./interpreter.cpp" "-o" "MipsTest${1}"
	rm -f "MipsTest${1}"
	${@:2} "./interpreter.cpp" "-o" "MipsTest${1}"
}

for a in "$@"; do
	flags=()
	compiler=
	ext=

	case "$a" in
		host)
			get_target_host
		;;
		host-debug)
			get_target_host_debug
		;;
		target)
			get_target_target
		;;
		target-debug)
			get_target_target_debug
		;;
		*) echo "Unknown Target: '${a}'"; exit -1;;
	esac
	
	any_job=true
	
	exec_job "$ext" "$compiler" "${flags[@]}" &
done

if ! $any_job; then
	get_target_target
	
	exec_job "$ext" "$compiler" "${flags[@]}" &
fi

wait
