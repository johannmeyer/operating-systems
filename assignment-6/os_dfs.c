//
// os_dfs.c
//
// The Dumb File System
// This file contains a very simple and dumb directory and file structure
// which is basically a tree structure. It contains many functions
// for adding/removing directories/files, and for searching for directories/files.
// It depends heavily on a singly-linked-list implementation in os_list.c.
// This is NOT how you should build a file system.
//
// Feel free to read, modify or improve this, althrough you 
// really dont't have to touch it for the assignment.
//

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include "os_list.c"

// Basic file type, with a name, contents, and parent dir.
typedef struct file_t {
    char* name;
    char* contents;
    struct dir_t* parent;
} file_t;

// Basic directory, with a name, files, subdirectories, and parent dirs.
typedef struct dir_t {
    char* name;
    list_t* files;
    list_t* dirs;
    struct dir_t* parent;
} dir_t;

/**
 * Allocate memory and fill fields for a new file.
 * - name: filename to identify file by.
 * - contents: file contents to be stored.
 * - parent: optional containing directory.
 * Returns pointer to allocated file.
 */
file_t* create_file(const char* name, const char* contents) {
    file_t* file = malloc(sizeof(file_t));
    file->name = malloc(strlen(name) + 1);
    file->contents = malloc(strlen(contents) + 1);
    strcpy(file->name, name);
    strcpy(file->contents, contents);
    file->parent = NULL;
    return file;
}

/**
 * Allocate memory and fill fields for a new directory.
 * - name: name to identify dir by.
 * - parent: optional containing directory.
 * Returns pointer to allocated directory.
 */
dir_t* create_dir(const char* name) {
    dir_t* dir = malloc(sizeof(dir_t));
    dir->name = malloc(strlen(name) + 1);
    strcpy(dir->name, name);
    dir->dirs = create_list();
    dir->files = create_list();
    dir->parent = NULL;
    return dir;
}

/**
 * Add a file to a directory.
 * - parent: parent directory to add this directory to.
 * - file: the file to add.
 */
void add_file_to(dir_t* parent, file_t* file) {
    push_back_p(parent->files, (void *) file);
    file->parent = parent;
}

/**
 * Add a directory to another directory.
 * - parent: parent directory to add this directory to.
 * - dir: the directory to add.
 */
void add_dir_to(dir_t* parent, dir_t* dir) {
    push_back_p(parent->dirs, (void *) dir);
    dir->parent = parent;
}

/**
 * Destroy a file and its contents.
 * Also removes this file from its parent.
 * - file: file to destroy.
 */
void destroy_file(file_t* file) {
    if (file->parent != NULL) {
        for (int i = 0; i < file->parent->files->length; i++) {
            file_t* self = (file_t *) get_p(file->parent->files, i);
            if (self == file) {
                remove_p(file->parent->files, i);
                break;
            }
        }
    }
    free(file->contents);
    free(file->name);
    free(file);
}

/**
 * Destroy a directory and all contained data.
 * This destroys this directory, all subdirectories, and all files recursively.
 * Also removes this directory from its parent.
 * - dir: directory to destroy.
 */
void destroy_dir(dir_t* dir) {
    // Clean up all contained directories
    void* ptr = pop_back_p(dir->dirs);
    while (ptr != NULL) {
        dir_t* del = (dir_t *) ptr;
        destroy_dir(del);
        ptr = pop_back_p(dir->dirs);
    }
   
    // Clean up all contained files
    ptr = pop_back_p(dir->files);
    while (ptr != NULL) {
        file_t* del = (file_t *) ptr;\
        destroy_file(del);
        ptr = pop_back_p(dir->files);
    }

    // Detach self from parent
    if (dir->parent != NULL) {
        for (int i = 0; i < dir->parent->dirs->length; i++) {
            dir_t* self = (dir_t *) get_p(dir->parent->dirs, i);
            if (self == dir) {
                remove_p(dir->parent->dirs, i);
                break;
            }
        }
    }

    // Clean up remains
    destroy_list(dir->dirs);
    destroy_list(dir->files);
    free(dir->name);
    free(dir);
}

/**
 * Take a path string, split on '/', an turn it into a list.
 * - path_s: the path string.
 * Returns a list of strings
 */
list_t* parse_path(const char* path_s) {
    char* path = malloc(strlen(path_s) + 1);
    strcpy(path, path_s);
    list_t* list = create_list();

    char* tok = strtok(path, "/");
    while (tok != NULL) {
        char* tok_cpy = malloc(strlen(tok) + 1);
        strcpy(tok_cpy, tok);
        push_back_p(list, (void *) tok_cpy);
        tok = strtok(NULL, "/");
    }

    free(path);
    return list;
}

/**
 * Take a list_t of strings, and turn these into a single string, with
 * every string split on '/'.
 * - path_l: the path list to unparse.
 * Returns a path string.
 */
char* unparse_path(list_t* path_l) {
    if (path_l->length == 0) {
        char* path_s = malloc(1);
        strcpy(path_s, "");
        return path_s;
    }
    size_t len = 0;
    for (unsigned int i = 0; i < path_l->length; i++) {
        char* ptr = (char *) get_p(path_l, i);
        len += strlen(ptr) + 1;
    }
    char* path_s = malloc(len + 1);
    strcpy(path_s, "");
    for (unsigned int i = 0; i < path_l->length; i++) {
        char* ptr = (char *) get_p(path_l, i);
        strcat(path_s, "/");
        strcat(path_s, ptr);
    }
    return path_s;
}

/**
 * Gets last part of a path string, which is the 'name' of a file or directory.
 * - path_s: the path string to parse.
 * Returns char pointer to this name string.
 */
char* get_name_from_path(const char* path_s) {
    list_t* path_l = parse_path(path_s);
    char* name = (char *) pop_back_p(path_l); // last item in path is name
    destroy_list(path_l);
    return name;
}

/**
 * Gets all but the last part of a path string, which is the 'path' to a file
 * or directory without that file or directory included.
 * This can be used to get a parent directory for nonexistent (future) files or directories!
 * - path_s: the path string to parse.
 * Returns char pointer to shorter path string.
 */
char* get_dirs_from_path(const char* path_s) {
    list_t* path_l = parse_path(path_s);
    char* ret;
    pop_back_p(path_l); // last item in path is name
    ret = unparse_path(path_l);
    destroy_list(path_l);
    return ret;
}

/**
 * Find and return a pointer to a dir located directly in dirt.
 * - dir: the containing directory.
 * - name: wanted directory name.
 * Returns NULL if not found, and a valid pointer otherwise.
 */
dir_t* find_dir_direct(dir_t* dir, const char* name) {
    for (unsigned int i = 0; i < dir->dirs->length; i++) {
        dir_t* candidate = (dir_t *) get_p(dir->dirs, i);
        if (strcmp(candidate->name, name) == 0) {
            return candidate;
        }
    }
    return NULL;
}

/**
 * Find and return a pointer to a file located directly in dir
 * - dir: the containing directory.
 * - name: wanted file name.
 * Returns NULL if not found, and a valid pointer otherwise.
 */
file_t* find_file_direct(dir_t* dir, const char* name) {
    for (unsigned int i = 0; i < dir->files->length; i++) {
        file_t* candidate = (file_t *) get_p(dir->files, i);
        if (strcmp(candidate->name, name) == 0) {
            return candidate;
        }
    }
    return NULL;
}

/**
 * Find and return a pointer to file located at a certain path,
 * starting from a top directory. Only use this if you know a file is here!
 * - dir: the top directory.
 * - path_s: path string.
 * Returns NULL if the file is not found, and a valid (?) pointer otherwise.
 */
file_t* find_file(dir_t* dir, const char* path_s) {
    if (strlen(path_s) == 0 || strcmp(path_s, "/") == 0) {
        return NULL;
    }

    list_t* path = parse_path(path_s);
    file_t* ptr = NULL;

    // N-1 parameters in path are definitely directories
    dir_t* location = dir;
    while (path->length > 1) {
        char* name = (char *) pop_front_p(path);
        location = find_dir_direct(location, name);
        free(name);
        if (location == NULL) {
            destroy_list(path);
            return ptr;
        }
    }
    
    char* name = (char *) pop_front_p(path);
    ptr = find_file_direct(location, name);
    free(name);
    destroy_list(path);
    return ptr;
}

/**
 * Find and return a pointer to dir located at a certain path,
 * starting from a top directory. Only use this if you know a dir is here!
 * - dir: the top directory.
 * - path_s: path string.
 * Returns NULL if the dir is not found, and a valid (?) pointer otherwise.
 */
dir_t* find_dir(dir_t* dir, const char* path_s) {
    if (strlen(path_s) == 0 || strcmp(path_s, "/") == 0) {
        return dir;
    }

    list_t* path = parse_path(path_s);
    dir_t* ptr = NULL;

    // N-1 parameters in path are definitely directories
    dir_t* location = dir;
    while (path->length > 1) {
        char* name = (char *) pop_front_p(path);
        location = find_dir_direct(location, name);
        free(name);
        if (location == NULL) {
            destroy_list(path);
            return ptr;
        }
    }
    
    char* name = (char *) pop_front_p(path);
    ptr = find_dir_direct(location, name);
    free(name);
    destroy_list(path);
    return ptr;
}

/**
 * Find and return a pointer to a subdirectory/file in dir.
 * - dir: the containing directory.
 * - name: wanted file/dir name.
 * Returns NULL if not found, and valid pointer otherwise.
 */
// void* find_ptr_in(dir_t* dir, const char* name) {
//     for (unsigned int i = 0; i < dir->dirs->length; i++) {
//         dir_t* candidate = (dir_t *) get_p(dir->dirs, i);
//         if (strcmp(candidate->name, name) == 0) {
//             return (void *) candidate;
//         }
//     }
//     for (unsigned int i = 0; i < dir->files->length; i++) {
//         file_t* candidate = (file_t *) get_p(dir->files, i);
//         if (strcmp(candidate->name, name) == 0) {
//             return (void *) candidate;
//         }
//     }
//     return NULL;
// }

/**
 * Find and return a pointer to what is located at a given path,
 * starting from a top directory. This can be a file or directory,
 * so you'll have to cast it before using it!
 * - dir: the top directory.
 * - path_s: path string.
 * Returns NULL if the item is not found, and a valid (?) pointer otherwise.
 */
// void* find_ptr(dir_t* dir, const char* path_s) {
//     if (strlen(path_s) == 0 || strcmp(path_s, "/") == 0) {
//         return NULL;
//     }

//     list_t* path = parse_path(path_s);
//     void* ptr = NULL;

//     // N-1 parameters in path are definitely directories
//     dir_t* location = dir;
//     while (path->length > 1) {
//         char* name = (char *) pop_front_p(path);
//         location = find_dir_direct(location, name);
//         free(name);
//         if (location == NULL) {
//             destroy_list(path);
//             return ptr;
//         }
//     }
    
//     char* name = (char *) pop_front_p(path);
//     ptr = find_ptr_in(location, name);
//     free(name);
//     destroy_list(path);
//     return ptr;
// }

/**
 * Returns whether something on a path is:
 * 1) a file.
 * 0) a directory.
 * -1) not found.
 */
// int find_type(dir_t* dir, const char* path_s) {
//     if (find_file(dir, path_s) != NULL) {
//         return 1;
//     } else if (find_dir(dir, path_s) != NULL) {
//         return 0;
//     } else {
//         return -1;
//     }
// }