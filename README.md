# iRODS Bash Completer

This is a tool to allow the iRODS client icommands to have auto-complete functionality within Bash.

# Installation

The installation consists of two parts; building the command-line tool and configuring the bash auto-completion 
script.

## Building the command line tool

The tool is built with the given makefile. You will need to have the iRODS header files and runtime libraries to build it.
By default, these will be in ```/usr/include/irods/``` and ```/usr/lib/``, however, if you have installed iRODS to a non-standard
location, simply adjust the ```IRODS_HOME``` variable in the makefile to the appropraite value.

You can then build the tool by running

~~~
make all
~~~

which will create the tool at ```build/irods\_bash\_completer```. You can copy this executeable to wherever you want to run it from.

## Configuring the Bash auto-completion



# Credits

iRODS Bash Completer is written by [Simon Tyrrell](https://github.com/billyfish). For questions or support, contact me directly or 
via the [Earlham Institute](http://www.earlham.ac.uk/contact-us/) page.v


## License ##

Copyright (c) 2018, Earlham Institute.
