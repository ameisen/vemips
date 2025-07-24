#pragma once

#include "host_mmu.hpp"

// TODO : remove when all sys APIs abstracted
// ReSharper disable IdentifierTypo
#pragma region Windows.h
#	define NOMINMAX 1
#	undef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN 1  // NOLINT(clang-diagnostic-unused-macros)
#	define NOCOMM 1
#	define NOGDICAPMASKS 1
#	define NOVIRTUALKEYCODES 1
#	define NOWINMESSAGES 1
#	define NOWINSTYLES 1
#	define NOSYSMETRICS 1
#	define NOMENUS 1
#	define NOICONS 1
#	define NOKEYSTATES 1
#	define NOSYSCOMMANDS 1
#	define NORASTEROPS 1
#	define NOSHOWWINDOW 1
#	define OEMRESOURCE 1
#	define NOATOM 1
#	define NOCLIPBOARD 1
#	define NOCOLOR 1
#	define NOCTLMGR 1
#	define NODRAWTEXT 1
#	define NOGDI 1
#	define NOKERNEL 1
#	define NOUSER 1
#	define NONLS 1
#	define NOMB 1
#	define NOMEMMGR 1
#	define NOMETAFILE 1
#	define NOMINMAX 1
#	define NOMSG 1
#	define NOOPENFILE 1
#	define NOSCROLL 1
#	define NOSERVICE 1
#	define NOSOUND 1
#	define NOTEXTMETRIC 1
#	define NOWH 1
#	define NOWINOFFSETS 1
#	define NOCOMM 1
#	define NOKANJI 1
#	define NOHELP 1
#	define NOPROFILER 1
#	define NODEFERWINDOWPOS 1
#	define NOMCX 1
#	include <Windows.h>
#	undef NOCOMM
#	undef NOGDICAPMASKS
#	undef NOVIRTUALKEYCODES
#	undef NOWINMESSAGES
#	undef NOWINSTYLES
#	undef NOSYSMETRICS
#	undef NOMENUS
#	undef NOICONS
#	undef NOKEYSTATES
#	undef NOSYSCOMMANDS
#	undef NORASTEROPS
#	undef NOSHOWWINDOW
#	undef OEMRESOURCE
#	undef NOATOM
#	undef NOCLIPBOARD
#	undef NOCOLOR
#	undef NOCTLMGR
#	undef NODRAWTEXT
#	undef NOGDI
#	undef NOKERNEL
#	undef NOUSER
#	undef NONLS
#	undef NOMB
#	undef NOMEMMGR
#	undef NOMETAFILE
#	undef NOMINMAX
#	undef NOMSG
#	undef NOOPENFILE
#	undef NOSCROLL
#	undef NOSERVICE
#	undef NOSOUND
#	undef NOTEXTMETRIC
#	undef NOWH
#	undef NOWINOFFSETS
#	undef NOCOMM
#	undef NOKANJI
#	undef NOHELP
#	undef NOPROFILER
#	undef NODEFERWINDOWPOS
#	undef NOMCX
#	undef WIN32_LEAN_AND_MEAN
#	undef NOMINMAX
#pragma endregion
// ReSharper restore IdentifierTypo
