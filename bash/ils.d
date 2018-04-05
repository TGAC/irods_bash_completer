_irods_completer() 
{
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    opts=`/home/billy/Projects/irods_bash_completer/build/irods_bash_completer ${cur}`

    COMPREPLY=( $(compgen -W "${opts}" -- ${cur}) )

		[[ ${COMPREPLY} = */ ]] && compopt -o nospace
    return 0
}

complete -F _irods_completer ils
complete -F _irods_completer icp
complete -F _irods_completer imv
complete -F _irods_completer imkdir
complete -F _irods_completer irm
