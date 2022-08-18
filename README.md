# iRODS Bash Completer

This is a tool to allow the iRODS client icommands to have auto-complete functionality within Bash.

# Installation

The installation consists of two parts; building the command-line tool and configuring the bash auto-completion 
script.

## Building the command line tool

The tool is built with the given makefile. Copy the default_user.prefs file to user.prefs to allow it to be accessed by the makefile.

~~~
cp default_user.prefs user.prefs
~~~


By default, iRODS will be installed so that  ```/usr/include/irods/``` and ```/usr/lib/``` contain  the iRODS header files and runtime libraries which are part of the irods-dev package. However, if you have installed iRODS to a non-standard
location, simply adjust the ```IRODS_HOME``` variable in ```user.prefs``` to the correct location. Similarly if you are running iRODS 4.3.x or above, uncomment the CFLAGS line in ```user.prefs``` too.  

You can then build the tool by running

~~~
make all
~~~

which will create the tool at ```build/irods_bash_completer```. You can copy this executable to wherever you want to run it from.

## Configuring the Bash auto-completion

The Bash auto-completion script is at ```bash/icommands.d```. You will need to edit the ```opts``` line in this file to match the 
location of where you copied the executable ```irods_bash_completer``` to. For example if you installed it in ```/usr/bin```, then
you would need to change the line from

~~~
 opts=`/home/billy/Projects/irods_bash_completer/build/irods_bash_completer ${cur}`
~~~

to 

~~~
 opts=`/usr/bin/irods_bash_completer ${cur}`
~~~

At the bottom of the file it contains the auto-completion configurations for a number of the iRODS icommands. If you wish to add more 
entries then just add them in the format of:

~~~
complete -F _irods_completer <icommand to add>
~~~

Once you have and this needs to be copied into the directory into ```/etc/bash_completion.d/```. 


# Credits

iRODS Bash Completer is written by [Simon Tyrrell](https://github.com/billyfish). For questions or support, contact me directly or 
via the [Earlham Institute](http://www.earlham.ac.uk/contact-us/) page.


## License ##

Copyright (c) 2018-22, Earlham Institute under the Apache License Version 2.0.
