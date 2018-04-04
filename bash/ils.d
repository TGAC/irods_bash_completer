_irods_completer() 
{
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    opts=`/home/billy/Projects/irods_bash_completion/Debug/irods_bash_completion ${cur}`

#echo "cur: _${cur}_ prev: _${prev}_ opts: _${opts}_"

        COMPREPLY=( $(compgen -W "${opts}" ${cur}) )
        return 0
}
complete -F _irods_completer ils
