Assignment 6
------------
FUSE file system.

1) readdir: enables ls
2) mkdir and rmdir
3) read and write
4) create and unlink: create and delete files
5) - limit filesystem to 16 files
   - show number of files using LEDs

os_assignment.c:	Bindings for DFS to FUSE.
os_list.c:			Simple linked list inplementation.
os_dfs.c:			Dumb file system that uses linked lists to
					store the file hierarchy.
