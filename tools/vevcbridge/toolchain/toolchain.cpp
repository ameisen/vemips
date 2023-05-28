#include "buildcarbide.hpp"

#include "toolchain.hpp"

#include <vector>

using namespace std::literals;

namespace buildcarbide::toolchain {
    namespace {
        // https://learn.microsoft.com/en-us/archive/blogs/twistylittlepassagesallalike/everyone-quotes-command-line-arguments-the-wrong-way
        template <bool Force = false, typename TString> requires(
            std::is_same_v<std::remove_cvref_t<TString>, wstring> || 
            std::is_same_v<std::remove_cvref_t<TString>, wstring_view>
        )
        static wstring escape(TString& argument) {
            if (Force == false &&
                argument.empty() == false &&
                argument.find_first_of(L" \t\n\v\"") == TString::npos
            ) [[likely]] {
                if constexpr (std::is_same_v<std::remove_cvref_t<TString>, wstring_view>) {
                    return wstring{argument};
                }
                else if constexpr (std::is_move_constructible_v<TString>) {
                    return std::forward<TString>(argument);
                }
                else {
                    return argument;
                }
            }

            static constexpr const auto count_of = []<size_t N>(TString arg, const wchar_t(& __restrict of)[N]) {
                size_t count = 0;

                for (const auto c : arg) {
                    for (const auto of_c : of) {
                        if (c == of_c) {
                            ++count;
                            break;
                        }
                    }
                }

                return count;
            }; 

            static constexpr const wchar_t backslash = L'\\';

            wstring result = L"\"";
            result.reserve(argument.length() + count_of(argument, L"\\\""));

            size_t backslash_count = 0;
            for (auto i = argument.begin(); i != argument.end(); ++i) {
                for (backslash_count = 0; i != argument.end() && *i == backslash; ++i) {
                    ++backslash_count;
                }

                if (i != argument.end()) {
                    if (*i == L'"') {

                        //
                        // Escape all backslashes and the following
                        // double quotation mark.
                        //

                        backslash_count = backslash_count * 2 + 1;
                    }
                    result.append(backslash_count, backslash);
                    result.push_back(*i);
                }
            }

            if (backslash_count != 0) {
                //
                // Escape all backslashes, but let the terminating
                // double quotation mark we add below be interpreted
                // as a metacharacter.
                //

                result.append(backslash_count * 2, backslash);
            }

            result.push_back(L'"');

            return result;
        }

        template <typename TCollection>
        static no_throw wstring merge_args_impl(TCollection&& args) noexcept {
        if (args.empty()) [[unlikely]] {
            return {};
        }

        wstring result;

		size_t length = 0;
		for (const auto& arg : args) {
			length += arg.length();
		}
        length *= 2;
        length += args.size() - 1;

        result.reserve(length);

        for (const auto& arg : args) {
            result.append(escape(arg));
            result.push_back(L' ');
        }

        result.shrink_to_fit();

        return result;
        }
    }

    no_throw wstring toolchain::merge_args(const std::span<wstring> args) noexcept {
        return merge_args_impl(args);
    }

    no_throw wstring toolchain::merge_args(std::vector<wstring>&& args) noexcept {
        return merge_args_impl(std::move(args));
    }
}
