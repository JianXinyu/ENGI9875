#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "assign4.h"

// Some hard-coded inode numbers:
#define	ROOT_DIR	1
#define	ASSIGN_DIR	2
#define	USERNAME_FILE	3
#define FEATURES_FILE   4
#define	FILE_COUNT	4

// Hard-coded content of the "assignment/username" file:
static const char UsernameContent[] = "xjian\n";
// Hard-coded content of the "assignment/features" file:
static const char FeaturesContent[] = "I have implemented the following optional features:\n "
                                      "- Directory listing\n "
                                      "- File modification\n "
                                      "- File creation\n "
                                      "- Directory creation\n";

// Hard-coded stat(2) information for each directory and file:
struct stat *file_stats;

typedef struct {
    char* name;
    fuse_ino_t ino;
    fuse_ino_t parent;
    char* fileData;
    size_t fileDataLen;
    char isDir;
    fuse_ino_t *children;
    int numChildren;
} inode_data_t;

inode_data_t *data;
int file_count = 0;
int generation = 0;

static const int AllRead = S_IRUSR | S_IRGRP | S_IROTH;
static const int AllExec = S_IXUSR | S_IXGRP | S_IXOTH;

static void
assign4_init(void *userdata, struct fuse_conn_info *conn)
{
    struct backing_file *backing = userdata;
    printf("%s '%s'\n", __func__, backing->bf_path);
    file_stats = calloc(FILE_COUNT + 1, sizeof(struct stat));

    file_stats[ROOT_DIR].st_ino = ROOT_DIR;
    file_stats[ROOT_DIR].st_mode = S_IFDIR | AllRead | AllExec;
    file_stats[ROOT_DIR].st_nlink = 1;
    file_stats[ROOT_DIR].st_uid = geteuid();

    file_stats[ASSIGN_DIR].st_ino = ASSIGN_DIR;
    file_stats[ASSIGN_DIR].st_mode = S_IFDIR | AllRead | AllExec;
    file_stats[ASSIGN_DIR].st_nlink = 1;
    file_stats[ASSIGN_DIR].st_uid = geteuid();

    file_stats[USERNAME_FILE].st_ino = USERNAME_FILE;
    file_stats[USERNAME_FILE].st_mode = S_IFREG | AllRead;
    file_stats[USERNAME_FILE].st_size = sizeof(UsernameContent);
    file_stats[USERNAME_FILE].st_nlink = 1;
    file_stats[USERNAME_FILE].st_uid = geteuid();

    file_stats[FEATURES_FILE].st_ino = FEATURES_FILE;
    file_stats[FEATURES_FILE].st_mode = S_IFREG | AllRead;
    file_stats[FEATURES_FILE].st_size = sizeof(FeaturesContent);
    file_stats[FEATURES_FILE].st_nlink = 1;
    file_stats[FEATURES_FILE].st_uid = geteuid();

    file_count = FILE_COUNT;

    data = malloc(sizeof(inode_data_t)*(file_count + 1));

    data[1].name = strdup("tmp");
    data[1].ino = 1;
    data[1].parent = 0;
    data[1].fileData = NULL;
    data[1].fileDataLen = 0;
    data[1].isDir = 1;
    data[1].children = malloc(sizeof(fuse_ino_t));
    data[1].children[0] = 2;
    data[1].numChildren = 1;

    data[2].name = strdup("assignment");
    data[2].ino = 2;
    data[2].parent = 1;
    data[2].fileData = NULL;
    data[2].fileDataLen = 0;
    data[2].isDir = 1;
    data[2].children = malloc(sizeof(fuse_ino_t)*2);
    data[2].children[0] = 3;
    data[2].children[1] = 4;
    data[2].numChildren = 2;


    data[3].name = strdup("username");
    data[3].ino = 3;
    data[3].parent = 2;
    data[3].fileData = strdup(UsernameContent);
    data[3].fileDataLen = strlen(data[3].fileData);
    data[3].isDir = 0;
    data[3].children = NULL;
    data[3].numChildren = 0;

    data[4].name = strdup("features");
    data[4].ino = 4;
    data[4].parent = 2;
    data[4].fileData = strdup(FeaturesContent);
    data[4].fileDataLen = strlen(data[4].fileData);
    data[4].isDir = 0;
    data[4].children = NULL;
    data[4].numChildren = 0;
}

static void
assign4_destroy(void *userdata)
{
    struct backing_file *backing = userdata;
    printf("%s %d\n", __func__, backing->bf_fd);
    free(file_stats);
    for (int i = 1; i <= file_count; i++) {
        free(data[i].name);
        free(data[i].children);
        free(data[i].fileData);
    }
    free(data);
}


static void
assign4_access(fuse_req_t req, fuse_ino_t ino, int mask)
{
    printf("%s ino=%zu mask=%d\n", __func__, ino, mask);
    fuse_reply_err(req, ENOSYS);
}

static void
assign4_create(fuse_req_t req, fuse_ino_t parent, const char *name,
               mode_t mode, struct fuse_file_info *fi)
{
    printf("%s parent=%zu name='%s' mode=%d\n", __func__,
           parent, name, mode);

    //fuse_reply_create();
    fuse_reply_err(req, ENOSYS);
}

static void
assign4_getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fip)
{
    printf("%s ino=%zu\n", __func__, ino);

    if (ino > file_count) {
        fuse_reply_err(req, ENOENT);
        return;
    }

    int result = fuse_reply_attr(req, file_stats+ino, 1);
    if (result != 0) {
        fprintf(stderr, "Failed to send attr reply: %d\n", result);
    }
}

static void
assign4_lookup(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    printf("%s parent=%zu name='%s'\n", __func__,
           parent, name);

    for (int i = 0; i < data[parent].numChildren; i++) {
        if (strcmp(data[data[parent].children[i]].name, name) == 0) {
            fuse_ino_t ino = data[parent].children[i];
            struct fuse_entry_param entry = {
                    .ino = ino,
                    .generation = ++generation,
                    .attr = file_stats[ino],
                    .attr_timeout = 1,
                    .entry_timeout = 1,
            };
            int result = fuse_reply_entry(req, &entry);
            if (result != 0) {
                fprintf(stderr, "Failed to send dirent reply: %d\n", result);
            }
            return;
        }
    }

    fuse_reply_err(req, ENOENT);
}

static void
assign4_mkdir(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode)
{
    printf("%s parent=%zu name='%s' mode=%d\n", __func__,
           parent, name, mode);

    file_count++;
    file_stats = realloc(file_stats, sizeof(struct stat)*(file_count + 1));
    file_stats[file_count].st_ino = file_count;
    file_stats[file_count].st_mode = S_IFDIR | AllRead | AllExec;
    file_stats[file_count].st_nlink = 1;
    file_stats[file_count].st_uid = geteuid();

    data[parent].numChildren++;
    data[parent].children = realloc(data[parent].children, sizeof(fuse_ino_t)*data[parent].numChildren);
    data[parent].children[data[parent].numChildren - 1] = file_count;

    data = realloc(data, sizeof(inode_data_t)*(file_count + 1));
    data[file_count].name = strdup(name);
    data[file_count].isDir = 1;
    data[file_count].parent = parent;
    data[file_count].children = NULL;
    data[file_count].numChildren = 0;

    struct fuse_entry_param entry;

    entry.ino = file_count;
    entry.generation = ++generation;
    entry.attr = file_stats[file_count];
    entry.attr_timeout = 1;
    entry.entry_timeout = 1;
    int result = fuse_reply_entry(req, &entry);
    if (result != 0) {
        fprintf(stderr, "Failed to send mkdir reply: %d\n", result);
    }
}

static void
assign4_mknod(fuse_req_t req, fuse_ino_t parent, const char *name,
              mode_t mode, dev_t rdev)
{
    printf("%s parent=%zu name='%s' mode=%d\n", __func__,
           parent, name, mode);
    file_count++;
    file_stats = realloc(file_stats, sizeof(struct stat)*(file_count + 1));
    file_stats[file_count].st_ino = file_count;
    file_stats[file_count].st_mode = S_IFREG | AllRead | AllExec;
    file_stats[file_count].st_nlink = 1;
    file_stats[file_count].st_uid = geteuid();

    data[parent].numChildren++;
    data[parent].children = realloc(data[parent].children, sizeof(fuse_ino_t)*data[parent].numChildren);
    data[parent].children[data[parent].numChildren - 1] = file_count;

    data = realloc(data, sizeof(inode_data_t)*(file_count + 1));
    data[file_count].name = strdup(name);
    data[file_count].isDir = 0;
    data[file_count].parent = parent;
    data[file_count].children = NULL;
    data[file_count].numChildren = 0;
    data[file_count].fileData = NULL;
    data[file_count].fileDataLen = 0;

    struct fuse_entry_param entry;

    entry.ino = file_count;
    entry.generation = ++generation;
    entry.attr = file_stats[file_count];
    entry.attr_timeout = 1;
    entry.entry_timeout = 1;

    int result = fuse_reply_entry(req, &entry);
    if (result != 0) {
        fprintf(stderr, "Failed to send mknod reply: %d\n", result);
    }
}

static void
assign4_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
    printf("%s ino=%zu\n", __func__, ino);
    fuse_reply_err(req, ENOSYS);
}

static void
assign4_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
                off_t off, struct fuse_file_info *fi)
{
    printf("%s ino=%zu size=%zu off=%zu\n", __func__,
           ino, size, off);

    if (off > data[ino].numChildren + 1) {
        fuse_reply_buf(req, NULL,0);
        return;
    }

    if (!data[ino].isDir) {
        fuse_reply_err(req, ENOTDIR);
        return;
    }

    size_t len = 0;
    char buf[size];
    int next = 0;
    if (off < 1) {
        len += fuse_add_direntry(req, buf+len, sizeof(buf) - len, ".", file_stats + ino, ++next);
    }

    if (off < 2) {
        len += fuse_add_direntry(req, buf+len, sizeof(buf) - len, "..", file_stats + data[ino].parent, ++next);
    }

    for (int i = 0; i < data[ino].numChildren; i++) {
        len += fuse_add_direntry(req, buf+len, sizeof(buf) - len, data[data[ino].children[i]].name, file_stats + data[ino].children[i], ++next);
    }


    int result = fuse_reply_buf(req, buf, len);
    if (result != 0) {
        fprintf(stderr, "Failed to send readdir reply: %d\n", result);
    }
}

static void
assign4_read(fuse_req_t req, fuse_ino_t ino, size_t size,
             off_t off, struct fuse_file_info *fi)
{
    printf("%s ino=%zu size=%zu off=%zu\n", __func__,
           ino, size, off);

    if (data[ino].isDir) {
        fuse_reply_err(req, EISDIR);
        return;
    }

    if (data[ino].fileDataLen == 0) {
        fuse_reply_buf(req, NULL, 0);
        return;
    }

    const char* buf = data[ino].fileData + off;
    size_t len = data[ino].fileDataLen - off;
    if (len > size) {
        len = size;
    }
    printf("INO:: %ld, CONTENT:: %s, LENGTH:: %ld\n", ino, buf, len);

    int result = fuse_reply_buf(req, buf, len);
    if (result != 0) {
        fprintf(stderr, "Failed to send read reply: %d\n", result);
    }
    else {
        printf("READ SUCCESS, APPARENTLY\n");
    }
}

static void
assign4_rmdir(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    printf("%s parent=%zu name='%s'\n", __func__, parent, name);
    fuse_reply_err(req, ENOSYS);
}

static void
assign4_setattr(fuse_req_t req, fuse_ino_t ino, struct stat *attr,
                int to_set, struct fuse_file_info *fi)
{
    fuse_reply_err(req, ENOSYS);
}

static void
assign4_statfs(fuse_req_t req, fuse_ino_t ino)
{
    printf("%s ino=%zu\n", __func__, ino);
    fuse_reply_err(req, ENOSYS);
}

static void
assign4_unlink(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    printf("%s parent=%zu name='%s'\n", __func__, parent, name);
    fuse_reply_err(req, ENOSYS);
}

static void
assign4_write(fuse_req_t req, fuse_ino_t ino, const char *buf, size_t size,
              off_t off, struct fuse_file_info *fi)
{
    printf("%s ino=%zu size=%zu off=%zu\n", __func__,
           ino, size, off);

    free(data[ino].fileData);
    data[ino].fileData = strdup(buf);
    data[ino].fileDataLen = strlen(buf) + 1;
    file_stats[ino].st_size = size;

    fuse_reply_write(req, size);

}


static struct fuse_lowlevel_ops assign4_ops = {
        .init           = assign4_init,
        .destroy        = assign4_destroy,

        .access         = assign4_access,
        .create         = assign4_create,
        .getattr        = assign4_getattr,
        .lookup         = assign4_lookup,
        .mkdir          = assign4_mkdir,
        .mknod          = assign4_mknod,
        .open           = assign4_open,
        .read           = assign4_read,
        .readdir        = assign4_readdir,
        .rmdir          = assign4_rmdir,
        .setattr        = assign4_setattr,
        .statfs         = assign4_statfs,
	    .unlink         = assign4_unlink,
        .write          = assign4_write,
};



struct fuse_lowlevel_ops*
assign4_fuse_ops()
{
    return &assign4_ops;
}

