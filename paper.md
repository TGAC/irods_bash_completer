---
title: 'iRODS Bash Completer'
tags:
  - iRODS
  - Bash shell
authors:
 - name: Simon Tyrrell
   orcid: 0000-0002-7789-6875
   affiliation: 1
 - name: Robert P. Davey
   orcid: 0000-0002-5589-7754
   affiliation: 1
 - name: Xingdong Bian
   orcid: 0000-0002-4719-3220
   affiliation: 1
affiliations:
 - name: Earlham Institute, Norwich Research Park, Norwich, NR4 7UZ, UK.
   index: 1
date: 1 May 2019
bibliography: paper.bib
---

# Summary

iRODS [@iRODS] is an open source data management system which allows files and folders to be abstracted into a set of data objects and collections, respectively, which are stored in arbitrary sets called Zones. The key advantages of iRODS are: Zones are multiple independent administrative units, which can exist both locally and remotely and can be federated together to be presented to users transparently as a single hierarchy; data objects and collections can have rich metadata added to them. iRODS ships with client command-line tools, called icommands, and allows users to interact with iRODS abstracted storage through typical file, folder, and metadata operations. For users on Linux, the common command-line terminal is Bash [@Bash]. An extremely useful feature of Bash is the ability to complete any text that has been typed in the shell up to that point by pressing the TAB key. This tab-completion is application-specific, and the iRODS client icommands do not come with this functionality. As the number of nested levels within the iRODS zones increases, the likelihood of mistyping a path when using these commands becomes greater, and the usefulness of auto-completion is lost.

```iRODS Bash Completer``` is designed to fill this gap by implementing tab-complete in the various iRODS client icommands. It creates an executable that parses all the available iRODS zones that the current user has access to and produces a list of data objects and collections that match an initial piece of text or, if the initial text is empty, can be used to traverse through the entire hierarchy of the iRODS-hosted data. Then by installing and configuring the supplied ```icommmands.d``` file, the Bash shell can take advantage of this executable to auto-complete paths for the data hosted in iRODS in the standard way as it does for other mounted file systems.

# References
