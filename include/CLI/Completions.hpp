// Copyright (c) 2017-2022, University of Cincinnati, developed by Henry Schreiner
// under NSF AWARD 1414736 and by the respective contributors.
// All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

// [CLI11:public_includes:set]
#include <string>
#include <set>

// CLI library includes
#include "Error.hpp"

namespace CLI {

static const std::string _bash_completion = R"(
_cli11_complete()
{
    # Get cmdline while ignoring the last word
    cmdline=${COMP_LINE}
    if [[ "${cmdline:${cmdlinex}-1}" != " " ]]; then
        cmdline=${COMP_WORDS[@]:0:${COMP_CWORD}}
    fi

    # Get complete candidates
    words=$(${cmdline} --_autocomplete "${COMP_WORDS[${COMP_CWORD}]}")

    # Remove candidates that begins with `-` if `-` is not passed as the first character
    if [[ ! ${COMP_WORDS[${COMP_CWORD}]} = -* ]]; then
        words=(${words})
        for index in "${!words[@]}" ; do [[ ${words[$index]} =~ ^- ]] && unset -v 'words[$index]' ; done
        words="${words[@]}"
    fi

    # Get matches
    COMPREPLY=($(compgen -W "${words}" -- "${COMP_WORDS[${COMP_CWORD}]}"))
}

complete -F _cli11_complete -o bashdefault -o default )";

static const std::string _zsh_completion = R"(
_cli11_complete() {
    # Get the command line while ignoring the last word
    local cmdline="$words"

    if [[ "${#words[@]}" -gt 1 && "${(e)words[1]}" != " " ]]; then
        cmdline="${(j: :)words[1,$CURRENT]}"
    fi

    # Get complete candidates
    local candidates
    candidates=($(eval "$cmdline --_autocomplete '$words[$CURRENT]'" 2>/dev/null))

    # Remove candidates that begin with `-` if `-` is not passed as the first character
    if [[ ! "$words[$CURRENT]" = -* ]]; then
        candidates=("${(@)candidates:#-*}")
    fi

    # Output matches
    _wanted arguments expl 'candidates' compadd -a candidates
}

compdef _cli11_complete )";

static const std::set<std::string> supported_completion_shells = {"bash", "zsh"};

static std::string generate_completion_script(std::string shell_type, std::string app_name)
{
    if (shell_type == "bash")
        return _bash_completion + app_name + "\n";

    if (shell_type == "zsh")
        return "#compdef " + app_name + "\n\n" + _zsh_completion + app_name + "\n";

    throw Error("UnsupportedShellType", "Unsupported shell type", ExitCodes::InvalidError);
}

} // namespace CLI
