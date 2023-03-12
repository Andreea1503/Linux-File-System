**Chesches Iulia-Stefana && Spinochi Andreea**
                **312CA**

# Simple Linux File System

## Brief Description:

* The purpose of the program is to implement a simple Linux File System that
accepts bash commands using the tree data structure. Every file / directory
is a tree node that has 4 attributes: parent (root / another directory),
name, type and content. In case of a file, the content is simply a string
that saves the text of the file. In case of a directory, the content is a
list. Every node of the list points to another tree node, which in turn
could be a file / directory. We have completed all the functions that the
homework provided and added a few more just for organising the code better
and making it easier to read.

### Our functions:

tree:
* createFileTree: initializes the tree and its root
* touch: creates file in current directory. If a text is given
as an argument, the function makes it the content of the created
file
* ls: lists resources of a directory or prints text of a file
based on what arguments it gets
* mkdir: creates a new directory in the current one
* cd: changes directories
* tree: prints all the resources recursively
* pwd: prints the path to the current directory
* rm: removes file
* rmdir: removes empty directory
* rmrec: removes directory recursively making use of rm and rmdir
* cp: copies source to destination
* mv: moves source to destination
* freeTree: frees all the resources making use of rmrec

utils:
* list_create: initializes list
* list_add_node: adds node to list on the first position and allocates
memory for a tree node into the info field
* list_remove_file/list_remove_dir: remove file / directory from list.
Free the specific resources for each type
* list_free: frees exmpty list
* get_file/get_dir: these functions return a file / directory, if found in the
current node
* generic_cd/generic_cd2: return directory / parent directory of a path
that is given as an argument
* generic_strtok: parses path to file / directory received as an argument
