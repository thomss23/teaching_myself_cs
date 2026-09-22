/*
 * Starter for a read-only bird taxonomy filesystem.
 *
 * Intended layout:
 *   /<order>/<family>/<species>
 *
 * For example:
 *   /Accipitriformes/Accipitridae (Hawks, Eagles, and Kites)/Pearl Kite
 *
 * Build:
 *   gcc -Wall -Wextra birds_fs_starter.c \
 *       $(pkg-config fuse3 --cflags --libs) -o birds_fs
 *
 * Run in the foreground:
 *   mkdir -p birds_mount
 *   ./birds_fs -f birds_mount
 */

#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "birds.h"

/*
 * Optional: use init to prepare any state your filesystem needs.
 * The arrays from birds.h are already available globally.
 */
static void *birds_init(struct fuse_conn_info *conn, struct fuse_config *cfg)
{
    (void)conn;
    (void)cfg;

    /* TODO: decide whether I need initialization. */
    return NULL;
}

static int birds_getattr(const char *path, struct stat *stbuf,
                         struct fuse_file_info *fi)
{
    (void)fi;
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0555;
        stbuf->st_nlink = 2;
        return 0;
    }

    char s[256];
    if (snprintf(s, sizeof(s), "%s", path) >= (int)sizeof(s))
        return -ENAMETOOLONG;

    char *token = strtok(s, "/");
    int level = 0;
    int order_index = -1;
    int family_index = -1;
    int species_index = -1;

    while (token != NULL) {
        if (level == 0) {
            for (int i = 0; i < NUM_ORDERS; i++) {
                if (strcmp(token, Orders[i]) == 0) {
                    order_index = i;
                    break;
                }
            }
            if (order_index == -1)
                return -ENOENT;
        } else if (level == 1) {
            for (int i = 0; i < NUM_FAMILIES; i++) {
                if (strcmp(token, Families[i].name) == 0 &&
                    Families[i].order == order_index) {
                    family_index = i;
                    break;
                }
            }
            if (family_index == -1)
                return -ENOENT;
        } else if (level == 2) {
            for (int i = 0; i < NUM_SPECIES; i++) {
                if (strcmp(token, Species[i].name) == 0 &&
                    Species[i].family == family_index) {
                    species_index = i;
                    break;
                }
            }
            if (species_index == -1)
                return -ENOENT;
        } else {
            /* There is nothing below a species file. */
            return -ENOENT;
        }

        level++;
        token = strtok(NULL, "/");
    }

    if (level == 1 || level == 2) {
        /* The complete path named an order or a family. */
        stbuf->st_mode = S_IFDIR | 0555;
        stbuf->st_nlink = 2;
        return 0;
    }

    if (level == 3) {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(Species[species_index].name) + 1;
        return 0;
    }

    return -ENOENT;
}

static int birds_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi,
                         enum fuse_readdir_flags flags)
{
    (void)offset;
    (void)fi;
    (void)flags;

    if (strcmp(path, "/") == 0) {
        filler(buf, ".", NULL, 0, FUSE_FILL_DIR_DEFAULTS);
        filler(buf, "..", NULL, 0, FUSE_FILL_DIR_DEFAULTS);
        for (int i = 0; i < NUM_ORDERS; i++)
            filler(buf, Orders[i], NULL, 0, FUSE_FILL_DIR_DEFAULTS);
        return 0;
    }

    int level = 0;
    int order_index = -1;
    int family_index = -1;
    char s[256];

    if (snprintf(s, sizeof(s), "%s", path) >= (int)sizeof(s))
        return -ENAMETOOLONG;

    char *token = strtok(s, "/");
    while (token != NULL) {
        if (level == 0) {
            for (int i = 0; i < NUM_ORDERS; i++) {
                if (strcmp(token, Orders[i]) == 0) {
                    order_index = i;
                    break;
                }
            }
            if (order_index == -1)
                return -ENOENT;
        } else if (level == 1) {
            for (int i = 0; i < NUM_FAMILIES; i++) {
                if (strcmp(token, Families[i].name) == 0 &&
                    Families[i].order == order_index) {
                    family_index = i;
                    break;
                }
            }
            if (family_index == -1)
                return -ENOENT;
        } else if (level == 2) {
            int species_found = 0;
            for (int i = 0; i < NUM_SPECIES; i++) {
                if (strcmp(token, Species[i].name) == 0 &&
                    Species[i].family == family_index) {
                    species_found = 1;
                    break;
                }
            }
            if (!species_found)
                return -ENOENT;
        } else {
            return -ENOENT;
        }

        level++;
        token = strtok(NULL, "/");
    }

    if (level == 1) {
        filler(buf, ".", NULL, 0, FUSE_FILL_DIR_DEFAULTS);
        filler(buf, "..", NULL, 0, FUSE_FILL_DIR_DEFAULTS);
        for (int i = 0; i < NUM_FAMILIES; i++) {
            if (Families[i].order == order_index)
                filler(buf, Families[i].name, NULL, 0, FUSE_FILL_DIR_DEFAULTS);
        }
        return 0;
    }

    if (level == 2) {
        filler(buf, ".", NULL, 0, FUSE_FILL_DIR_DEFAULTS);
        filler(buf, "..", NULL, 0, FUSE_FILL_DIR_DEFAULTS);
        for (int i = 0; i < NUM_SPECIES; i++) {
            if (Species[i].family == family_index)
                filler(buf, Species[i].name, NULL, 0, FUSE_FILL_DIR_DEFAULTS);
        }
        return 0;
    }

    /* A three-component path is a species file, not a directory. */
    return -ENOTDIR;
}

/* TODO: Permit read-only opens of species files and reject other paths. */
static int birds_open(const char *path, struct fuse_file_info *fi)
{
    (void)fi;

    char s[256];
    char *paths[3];
    strcpy(s, path);

    char *token = strtok(s, "/");
    int level = 0;

    while(token != NULL) {

        paths[level] = strdup(token);
        level++;
        token = strtok(NULL, "/");

    }

    if (level != 3) {
        return -EISDIR;
    } else {
        
        for (int i = 0; i < NUM_SPECIES; i++) {
            if (strcmp(paths[2], Species[i].name) == 0) {
                return 0;
            }
        }
    }
    
}


static int birds_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
    (void)path;
    (void)buf;
    (void)size;
    (void)offset;
    (void)fi;

    int species_index = -1;
    int s[256];
    strcpy(s, path);

    char *token = strtok(s, "/");
    int level = 0;
    while(token != NULL) {
        if (level == 2) {
            for (int i = 0; i < NUM_SPECIES; i++) {
                if (strcmp(token, Species[i].name) == 0) {
                    species_index = i;
                    break;
                }
            }
            if (species_index == -1)
                return -ENOENT;
        }
        level++;
        token = strtok(NULL, "/");
    }

    if (species_index != -1) {
        size_t len = strlen(Species[species_index].code + 1);
        if (offset < len) {
            if (offset + size > len)
                size = len - offset;
            memcpy(buf, Species[species_index].code + offset, size);
        } else
            size = 0;
        return size;
    }

}

static const struct fuse_operations birds_operations = {
    .init = birds_init,
    .getattr = birds_getattr,
    .readdir = birds_readdir,
    .open = birds_open,
    .read = birds_read,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &birds_operations, NULL);
}
