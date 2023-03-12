// Copyright Spinochi Andreea && Chesches Iulia, 312CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "utils.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

// create the tree structure
FileTree createFileTree(char* rootFolderName) {
    FileTree fileTree;
    TreeNode *root = malloc(sizeof(TreeNode));
    DIE(!root, "Malloc failed\n");

    root->parent = NULL;
    root->name = rootFolderName;
    root->type = FOLDER_NODE;

    root->content = malloc(sizeof(FolderContent));
    DIE(!root->content, "Malloc failed\n");

    ((FolderContent *)root->content)->children = list_create();

    fileTree.root = root;
    return fileTree;
}

// destroy the tree structure
void freeTree(FileTree fileTree) {
    // create a tree node for the root parent in order to free the root
    TreeNode *root_parent = malloc(sizeof(TreeNode));
    DIE(!root_parent, "Malloc failed\n");

    root_parent->parent = NULL;
    root_parent->name = malloc(sizeof(char) * 20);
    DIE(!root_parent->name, "Malloc failed\n");
    strcpy(root_parent->name, "root_parent");
    root_parent->type = 1;

    root_parent->content = malloc(sizeof(FolderContent));
    DIE(!root_parent->content, "Malloc failed\n");

    ((FolderContent *)root_parent->content)->children = list_create();
    list_add_node(((FolderContent *)root_parent->content)
                  ->children, fileTree.root->name);
    free(((FolderContent *)root_parent->content)->children->head->info);
    ((FolderContent *)root_parent->content)->children->head
    ->info = fileTree.root;
    fileTree.root->parent = root_parent;

    // destroy the tree from the root parent down
    rmrec(root_parent, fileTree.root->name);

    // free the root parent
    free(((FolderContent *)root_parent->content)->children);
    free((FolderContent *)root_parent->content);
    free(root_parent->name);
    free(root_parent);
}

// display the files and folders from the current directory
void ls(TreeNode* currentNode, char* arg) {
    if (arg[0] == 0) {
        ListNode *aux = ((FolderContent *)currentNode->content)
                        ->children->head;

        while (aux) {
            if (aux->info->name)
                printf("%s\n", aux->info->name);
            aux = aux->next;
        }
    } else {
        // if the content inside the directory is anoter directory, then
        // go inside it and display the files and folders
        TreeNode *aux = get_dir(currentNode, arg);
        if (aux) {
            ListNode *curr = ((FolderContent *)aux->content)->children
                                ->head;

            while (curr) {
                printf("%s\n", curr->info->name);
                curr = curr->next;
            }
        } else {
            // if the content inside the directory is a file, then display
            // only its name
            TreeNode *aux = get_file(currentNode, arg);

            if (aux) {
                if (aux->name != NULL) {
                    printf("%s", aux->name);
                    if (((FileContent *)aux->content)->text)
                        printf(": %s\n",
                                ((FileContent *)aux->content)->text);
                }
            } else {
                printf("ls: cannot access '%s': ", arg);
                printf("No such file or directory\n");
                return;
            }
        }
    }
}

// display the current directory
void pwd(TreeNode* treeNode) {
    char **pwd_name = malloc(50 * sizeof(char *));
    DIE(!pwd_name, "Malloc failed\n");
    TreeNode *aux = treeNode;

    // make a vector of words to store the path of the current directory
    int i = 0;
    while (aux) {
        pwd_name[i] = aux->name;
        aux = aux->parent;
        i++;
    }

    // print the path of the current directory
    int j;
    for (j = i - 1; j >= 0; j--) {
        printf("%s", pwd_name[j]);
        if (j != 0) {
            printf("/");
        }
    }
}

// go to the specified folder and return a pointer to it
TreeNode* cd(TreeNode* currentNode, char* path) {
    TreeNode *aux = generic_cd(currentNode, path);
    if (aux) {
        return aux;
    } else {
        printf("cd: ");
        printf("no such file or directory: %s\n", path);
    }
    return currentNode;
}

// display all the files and folders in the tree
void tree(TreeNode* currentNode, char* arg) {
    int files = 0, directories = 0;

    TreeNode *aux = currentNode;
    aux = generic_cd(aux, arg);

    if (!aux) {
        printf("%s [error opening dir]\n\n0 directories, 0 files\n", arg);
        return;
    }
    int ok = 0;

    if (arg[0] == 0) {
        print_rec(currentNode, 0, &files, &directories, &ok);
    } else {
        print_rec(aux, 0, &files, &directories, &ok);
    }
    printf("%d directories, %d files\n", directories, files);
}

// create a new directory
void mkdir(TreeNode* currentNode, char* folderName) {
    // parse the current directory to verify if the folder already exists
    ListNode * aux = ((FolderContent *)currentNode->content)->children->head;
    while (aux)
    {
        if (aux->info->type == 1)
        {
            if (strcmp(folderName, aux->info->name) == 0)
            {
                printf("mkdir: cannot create directory '%s': ", folderName);
                printf("File exists\n");
                return;
            }
        }
        aux = aux->next;
    }

    // add the new folder to the current directory
    list_add_node(((FolderContent *)currentNode->content)->children,
                  folderName);
    ((FolderContent *)currentNode->content)->children->head->info
    ->type = 1;
    ((FolderContent *)currentNode->content)->children->head->info
    ->parent = currentNode;
    ((FolderContent *)currentNode->content)->children->head->info
    ->content = malloc(sizeof(FolderContent));
    DIE(!((FolderContent *)currentNode->content)->children->head->info
        ->content, "Malloc failed\n");

    ((FolderContent *)((FolderContent *)currentNode->content)->children
    ->head->info->content)->children = list_create();
}

// delete the files and folders recursively
void rmrec(TreeNode* currentNode, char* resourceName) {
    // check if the resource is a file or a directory
    TreeNode *aux = get_dir(currentNode, resourceName);
    ListNode *prev = NULL;

    if (aux) {
        // parse the directory to delete all the files and folders inside
        ListNode *aux2 = ((FolderContent *)aux->content)->children->head;
        prev = aux2;
        while (aux2) {
            prev = aux2;
            aux2 = aux2->next;
            rmrec(aux, prev->info->name);
        }
        free(list_remove_dir(((FolderContent *)currentNode->content)->children,
                        aux->name));
    } else {
        aux = get_file(currentNode, resourceName);
        if (aux) {
            free(list_remove_file(((FolderContent *)currentNode
                                    ->content)->children, resourceName));
        } else {
            printf("rmrec: failed to remove '%s': ", resourceName);
            printf("No such file or directory\n");
        }
    }
}

// delete a file
void rm(TreeNode* currentNode, char* fileName) {
    // check if the file we want to delete exists
    TreeNode *aux = get_file(currentNode, fileName);

    if (aux) {
        free(list_remove_file(((FolderContent *)currentNode->content)->children,
                         fileName));
    } else {
        aux = get_dir(currentNode, fileName);
        if (aux) {
            printf("rm: cannot remove '%s': Is a directory\n", fileName);
        } else {
            printf("rm: failed to remove '%s': ", fileName);
            printf("No such file or directory\n");
        }
    }
}

// delete a directory
void rmdir(TreeNode* currentNode, char* folderName) {
    // check if the folder we want to delete exists
    TreeNode *aux = get_dir(currentNode, folderName);

    // only delete the folder if it is empty
    if (aux) {
        if (!((FolderContent*)aux->content)->children->head) {
            free(list_remove_dir(((FolderContent *)currentNode->content)
                 ->children, folderName));
        } else {
            printf("rmdir: failed to remove '%s': ", folderName);
            printf("Directory not empty\n");
        }
    } else {
        aux = get_file(currentNode, folderName);
        if (aux) {
            printf("rmdir: failed to remove '%s': ", folderName);
            printf("Not a directory\n");
        } else {
            printf("rmdir: failed to remove '%s': ", folderName);
            printf("No such file or directory\n");
        }
    }
}

// create a file and add it to the current directory
void touch(TreeNode* currentNode, char* fileName, char* fileContent) {
    // parse the current directory to verify if the file already exists
    ListNode * aux = ((FolderContent *)currentNode->content)->children->head;
    while (aux)
    {
        if (aux->info->type == 0)
        {
            if (strcmp(fileName, aux->info->name) == 0)
            {
                return;
            }
        }
        aux = aux->next;
    }

    // add the new file to the current directory
    list_add_node(((FolderContent *)currentNode->content)->children,
                  fileName);
    ((FolderContent *)currentNode->content)->children->head->info
    ->type = 0;
    ((FolderContent *)currentNode->content)->children->head->info
    ->parent = currentNode;
    ((FolderContent *)currentNode->content)->children->head->info
    ->content = malloc(sizeof(FileContent *));
    DIE(!((FolderContent *)currentNode->content)->children->head->info
        ->content, "Malloc failed\n");

    ((FileContent *)((FolderContent *)currentNode->content)->children
    ->head->info->content)->text = fileContent;
}

// copy the content of a file to another file
void cp(TreeNode* currentNode, char* source, char* destination) {
    // find the path of the source file
    char *source_copy = malloc(strlen(source) + 1);
    DIE(!source_copy, "Malloc failed\n");
    memcpy(source_copy, source, strlen(source) + 1);
    strcat(source_copy, "/");

    char *token = strtok(source_copy   , "/");
    char *token_source = malloc(sizeof(char) * 2);
    DIE(!token_source, "Malloc failed\n");

    while (token) {
        strcpy(token_source, token);
        token = strtok(NULL, "/");
    }

    // find the path of the destination file
    char *destination_copy = malloc(strlen(destination) + 1);
    DIE(!destination_copy, "Malloc failed\n");
    memcpy(destination_copy, destination, strlen(destination) + 1);
    strcat(destination_copy, "/");

    token = strtok(destination_copy   , "/");
    char *token_destination = malloc(sizeof(char) * 2);
    DIE(!token_destination, "Malloc failed\n");

    while (token) {
        strcpy(token_destination, token);
        token = strtok(NULL, "/");
    }
    memcpy(destination_copy, destination, strlen(destination) + 1);

    TreeNode* aux2;

    // if the source is inside a directory we need to go to that directory
    // to get the file
    if (strcmp(token_source, source_copy) != 0) {
        strcat(source_copy, "/");
        TreeNode *token_source_dir = generic_cd(currentNode, source_copy);
        aux2 = get_file(token_source_dir, token_source);
    } else {
        aux2 = get_file(currentNode, token_source);
    }

    TreeNode* aux = generic_cd(currentNode, destination_copy);
    TreeNode* aux3 = generic_cd2(currentNode, destination_copy);

    if (aux && aux2) {
            touch(aux, strdup(aux2->name), strdup(((FileContent *)aux2->
            content)->text));
    } else {
        if (!aux2) {
            printf("cp: -r not specified; omitting directory '%s'\n",
                        source);
            free(destination_copy);
            free(source_copy);
            return;
        }
        if (!aux && aux2) {
            aux = get_file(aux3, token_destination);
            if (aux) {
                free(((FileContent *)aux->content)->text);
                ((FileContent *)aux->content)
                ->text = strdup(((FileContent *)aux2->content)->text);
            } else {
                printf("cp: failed to access '%s': Not a directory\n",
                        destination);
            }
        }
    }
    free(destination_copy);
    free(source_copy);
}

// move the content of a file or directory to another file or directory
void mv(TreeNode* currentNode, char* source, char* destination) {
    // find the path of the source file
    char *source_copy = malloc(strlen(source) + 1);
    DIE(!source_copy, "Malloc failed\n");

    memcpy(source_copy, source, strlen(source) + 1);
    strcat(source_copy, "/");

    char *token = strtok(source_copy   , "/");
    char *token_source = malloc(sizeof(char) * 2);
    DIE(!token, "Mallloc failed");

    while (token) {
        strcpy(token_source, token);
        token = strtok(NULL, "/");
    }

    // find the path of the destination file
    char *destination_copy = malloc(strlen(destination) + 1);
    DIE(!destination_copy, "Malloc failed");
    memcpy(destination_copy, destination, strlen(destination) + 1);
    strcat(destination_copy, "/");

    token = strtok(destination_copy   , "/");
    char *token_destination = malloc(sizeof(char) * 2);
    DIE(!token_destination, "Malloc failed");

    while (token) {
        strcpy(token_destination, token);
        token = strtok(NULL, "/");
    }
    memcpy(destination_copy, destination, strlen(destination) + 1);

    TreeNode* is_file;
    TreeNode *token_source_dir = NULL;

    // if the source is inside a directory we need to go to that directory
    // to get the file
    if (strcmp(token_source, source_copy) != 0) {
        strcat(source_copy, "/");
        token_source_dir = generic_cd(currentNode, source_copy);
        is_file = get_file(token_source_dir, token_source);
    } else {
        is_file = get_file(currentNode, source);
    }

    if (is_file) {
        if (!token_source_dir) {
            TreeNode *is_dir = generic_cd(currentNode, destination_copy);
            if (!is_dir) {
                printf("mv: failed to access '%s': Not a directory\n",
                        destination);
                return;
            }
            cp(currentNode, source, destination);
            rm(currentNode, source);
        } else {
            cp(currentNode, source, destination);
            rm(token_source_dir, token_source);
        }
    } else {
        TreeNode *is_dir = get_dir(currentNode, source);
        if (is_dir) {
            TreeNode *aux = generic_cd(currentNode, destination);

            if (aux) {
                TreeNode *source_dir = generic_cd2(currentNode, source);
                TreeNode *aux_source = source_dir;
                TreeNode *destination_dir = generic_cd(currentNode,
                                                    destination_copy);

                list_add_node(((FolderContent *)destination_dir->content)
                                ->children, aux_source->name);
                ((FolderContent *)destination_dir->content)->children->head
                ->info->type = 1;
                ((FolderContent *)destination_dir->content)->children->head->
                info->parent = destination_dir;

                free(((FolderContent *)destination_dir->content)->children->
                        head->info);
                ((FolderContent *)destination_dir->content)->children->head
                ->info = aux_source;

                TreeNode *parent = aux_source->parent;

                ListNode *aux_list = ((FolderContent *)parent->content)
                            ->children->head;

                // change the parent of the node
                int i = 0;
                ListNode *prev;
                prev = aux_list;
                while (aux_list) {
                    if (strcmp(aux_list->info->name,
                            aux_source->name) == 0) {
                                if (i == 0) {
                            ((FolderContent *)parent->content)
                            ->children->head = aux_list->next;
                        } else {
                            prev->next = aux_list->next;
                        }
                    }
                    i++;
                    prev = aux_list;
                    aux_list = aux_list->next;
                }
            }
        }
    }
}
