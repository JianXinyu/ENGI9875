/*
 * Copyright 2018, 2020 Jonathan Anderson
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <stdio.h>

#include "assign4.h"

static void
assign4_init(void *userdata, struct fuse_conn_info *conn)
{
    struct backing_file *backing = userdata;
    fprintf(stderr, "%s '%s'\n", __func__, backing->bf_path);

    /*
     * This function should do some setup (e.g., open the backing file or
     * mmap(2) some memory) and prepare any metadata that you need.
     */
}

static void
assign4_destroy(void *userdata)
{
    struct backing_file *backing = userdata;
    fprintf(stderr, "%s %d\n", __func__, backing->bf_fd);

    /*
     * Finalize any metadata, close any open files, etc.
     */
}


/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L801 */
static void
assign4_create(fuse_req_t req, fuse_ino_t parent, const char *name,
               mode_t mode, struct fuse_file_info *fi)
{
    fprintf(stderr, "%s parent=%zu name='%s' mode=%d\n", __func__,
            parent, name, mode);

    /*
     * Create and open a file.
     *
     * Respond by calling fuse_reply_err() if there's an error, or else
     * fuse_reply_create(), passing it information in a fuse_entry_param:
     *
     * https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L68
     *
     * This is the meaning of the "Valid replies" comment at
     * https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L791
     */

    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L256 */
static void
assign4_getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fip)
{
    fprintf(stderr, "%s ino=%zu\n", __func__, ino);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L205 */
static void
assign4_lookup(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    fprintf(stderr, "%s parent=%zu name='%s'\n", __func__,
            parent, name);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L332 */
static void
assign4_mkdir(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode)
{
    fprintf(stderr, "%s parent=%zu name='%s' mode=%d\n", __func__,
            parent, name, mode);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L317 */
static void
assign4_mknod(fuse_req_t req, fuse_ino_t parent, const char *name,
              mode_t mode, dev_t rdev)
{
    fprintf(stderr, "%s parent=%zu name='%s' mode=%d\n", __func__,
            parent, name, mode);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L444 */
static void
assign4_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi)
{
    fprintf(stderr, "%s ino=%zu\n", __func__, ino);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L622 */
static void
assign4_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
                off_t off, struct fuse_file_info *fi)
{
    fprintf(stderr, "%s ino=%zu size=%zu off=%zu\n", __func__,
            ino, size, off);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L472 */
static void
assign4_read(fuse_req_t req, fuse_ino_t ino, size_t size,
             off_t off, struct fuse_file_info *fi)
{
    fprintf(stderr, "%s ino=%zu size=%zu off=%zu\n", __func__,
            ino, size, off);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L367 */
static void
assign4_rmdir(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    fprintf(stderr, "%s parent=%zu name='%s'\n", __func__, parent, name);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L286 */
static void
assign4_setattr(fuse_req_t req, fuse_ino_t ino, struct stat *attr,
                int to_set, struct fuse_file_info *fi)
{
    fprintf(stderr, "%s ino=%zu to_set=%d\n", __func__, ino, to_set);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L674 */
static void
assign4_statfs(fuse_req_t req, fuse_ino_t ino)
{
    fprintf(stderr, "%s ino=%zu\n", __func__, ino);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L350 */
static void
assign4_unlink(fuse_req_t req, fuse_ino_t parent, const char *name)
{
    fprintf(stderr, "%s parent=%zu name='%s'\n", __func__, parent, name);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L498 */
static void
assign4_write(fuse_req_t req, fuse_ino_t ino, const char *buf, size_t size,
              off_t off, struct fuse_file_info *fi)
{
    fprintf(stderr, "%s ino=%zu size=%zu off=%zu\n", __func__,
            ino, size, off);
    fuse_reply_err(req, ENOSYS);
}


static struct fuse_lowlevel_ops assign4_ops = {
        .init           = assign4_init,
        .destroy        = assign4_destroy,

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


#if !defined(USE_EXAMPLE) && !defined(USE_SOLUTION)
struct fuse_lowlevel_ops*
assign4_fuse_ops()
{
    return &assign4_ops;
}
#endif