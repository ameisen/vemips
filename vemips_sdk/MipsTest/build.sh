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
	-fomit-frame-pointer \
	-momit-leaf-frame-pointer
)

DEBUG_FLAGS=( \
	"${COMMON_FLAGS[@]}" \
	-g3 \
	-O0 \
)
	
RELEASE_HOST_FLAGS=( \
	"${COMMON_FLAGS[@]}" \
	'-Wl,-LARGEADDRESSAWARE' \
	'-Wl,-LTCG' \
	'-Wl,-OPT:ICF=4' \
	'-Wl,-OPT:REF' \
	-flto=full \
)

# -fno-optimize-sibling-calls \
# -fno-omit-frame-pointer \
RELEASE_TARGET_FLAGS=( \
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
	flags+=( \
		"${RELEASE_HOST_FLAGS[@]}" \
		"${HOST_FLAGS[@]}" \
		"${HOST_RELEASE_FLAGS[@]}" \
	)
	ext=".exe"
}

get_target_host_debug() {
	compiler="clang++"
	flags+=( \
		"${DEBUG_FLAGS[@]}" \
		"${HOST_FLAGS[@]}" \
		"-fsanitize=undefined" \
		"-fsanitize=address" \
	)
	ext="D.exe"
}

get_target_target() {
	compiler="$VEMIPS_CXX"
	flags+=( \
		"${RELEASE_TARGET_FLAGS[@]}" \
		"${TARGET_FLAGS[@]}" \
		"${TARGET_RELEASE_FLAGS[@]}" \
	)
	ext=".vebin"
}

get_target_target_debug() {
	compiler="$VEMIPS_CXX"
	flags+=( \
		"${DEBUG_FLAGS[@]}" \
		"${TARGET_FLAGS[@]}" \
	)
	ext="D.vebin"
}

exec_job() {
	echo ${@:2} "./interpreter.cpp" "-o" "MipsTest${1}"
	rm -f "MipsTest${1}"
	${@:2} "./interpreter.cpp" "-o" "MipsTest${1}"
	
	echo ${@:2} "./self_modifying_test.cpp" "-o" "self_modifying_test${1}"
	rm -f "self_modifying_test${1}"
	${@:2} "./self_modifying_test.cpp" "-o" "self_modifying_test${1}"
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
