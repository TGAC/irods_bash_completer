_irods_completer() 
{
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    opts=`/home/billy/bin/irods_bash_completer ${cur}`


# The completion of the local files for iput, iget and irsync are 
# taken from the irods legacy repo at 
#
# https://github.com/irods/irods-legacy/blob/master/iRODS/irods_completion.bash
#
# Thanks to Terrell Russell for the heads up :-)

  # Case of "iput", first arg is a local file
  if [ $1 = "iput" -a $COMP_CWORD -eq 1 ]; then
    COMPREPLY=( $(compgen -o default ${cur}) )

  # Case of "iget", second arg is a local file
  elif [ $1 = "iget" -a $COMP_CWORD -eq 2 ]; then
    COMPREPLY=( $(compgen -o default ${cur}) )

  # Case of "ireg", second arg is a local file
  elif [ $1 = "ireg" -a $COMP_CWORD -eq 2 ]; then
    COMPREPLY=( $(compgen -o default ${cur}) )
    
  # Case of "irsync", manage i: prefix
  elif [ $1 = "irsync" ]; then

    if [[ $cur == i:* ]]; then
      base=${cur:2}
      COMPREPLY=( $(compgen -W "$list \ " ${base} ) )
    else
      COMPREPLY=( $(compgen -P i: -W "$list \ " ${cur} ) )
      COMPREPLY+=( $(compgen -o default ${cur}) )
    fi

  # General case, back to our code
  else

    COMPREPLY=( $(compgen -W "${opts}" -- ${cur}) )
		[[ ${COMPREPLY} = */ ]] && compopt -o nospace

	fi

  return 0
}

complete -F _irods_completer icp
complete -F _irods_completer iget
complete -F _irods_completer ils
complete -F _irods_completer imkdir
complete -F _irods_completer imv
complete -F _irods_completer iput
complete -F _irods_completer ireg
complete -F _irods_completer irm
complete -F _irods_completer irmdir

