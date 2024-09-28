#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <zlib.h>

#include "logka.h"

static const char *query = "SELECT decompress(content) AS readme FROM blob, (SELECT max(fid) AS max_fid FROM filename, mlink WHERE filename.fnid = mlink.fnid AND filename.name = 'README.md') WHERE max_fid = blob.rid;";

void sqlite_decompress(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    if (argc == 1) {
        unsigned char *compressed_data = (unsigned char *)sqlite3_value_blob(argv[0]);

        if (compressed_data == NULL) {
            error("Compressed data was null!");
            sqlite3_result_error(ctx, "decompress: Received NULL to decompress.", -1);
            return;
        }

        int compressed_size = sqlite3_value_bytes(argv[0]);
        info("Compressed data size: %d bytes", compressed_size);

        // Taken from the fossil source code src/blob.c line 1403 in `blob_uncompress()`
        int uncompressed_size = (compressed_data[0]<<24) +
                                (compressed_data[1]<<16) +
                                (compressed_data[2]<<8 ) +
                                (compressed_data[3]    );

        info("Uncompressed data size: %d bytes", uncompressed_size);
        unsigned char *uncompressed_data = calloc(uncompressed_size+1, 1);

        // Zlib wants a pointer to this as long, so this second variable is required
        unsigned long int uncompressed_size_long = (unsigned long int)uncompressed_size;

        int uncompress_result = uncompress(
            uncompressed_data,
            &uncompressed_size_long,
            &compressed_data[4],
            compressed_size - 4
        );

        if (uncompress_result != Z_OK) {
            error("Data was not decompressed successfully. Zlib error code: %d", uncompress_result);
            sqlite3_result_error(ctx, "decompress: Failed to decompress data.", -1);
            return;
        }

        sqlite3_result_text(ctx, (char*)uncompressed_data, uncompressed_size, SQLITE_TRANSIENT);

        free(uncompressed_data);
    } else {
        sqlite3_result_error(ctx, "decompress: This function expects one argument.", -1);
    }
}

int main(int argc, char *argv[]) {
    int result = 0;

    if (argc != 2) {
        error("This program expects one argument for the input file name.");
        return 1;
    }

    info("Zlib version: %s", zlib_version);

    const char *filename = argv[1];

    sqlite3      *ctx       = NULL;
    sqlite3_stmt *statement = NULL;
    int rc;

    info("Opening database");
    rc = sqlite3_open(filename, &ctx);

    if (rc != SQLITE_OK) {
        error("%s", sqlite3_errmsg(ctx));
        result = 1;
        goto finished;
    }
    ok("Database opened successfully!");

    info("Creating function 'decompress'");
    rc = sqlite3_create_function(ctx, "decompress", 1, SQLITE_UTF8, NULL, sqlite_decompress, NULL, NULL);

    if (rc != SQLITE_OK) {
        error("%s", sqlite3_errmsg(ctx));
        result = 1;
        goto finished;
    }
    ok("Successfully created function 'decompress'");

    info("Preparing query");
    rc = sqlite3_prepare_v2(ctx, query, -1, &statement, NULL);
    if (rc != SQLITE_OK) {
        error("%s", sqlite3_errmsg(ctx));
        result = 1;
        goto finished;
    }
    ok("Query preparedy successfully!");

    rc = sqlite3_step(statement);

    if (rc != SQLITE_ROW) {
        error("%s", sqlite3_errmsg(ctx));
        error("Did not get any rows from the database!");
        result = 1;
        goto finished;
    }

    const unsigned char *response = sqlite3_column_text(statement, 0);
    ok("Got response from database:");
    printf("%s\n", response);

    sqlite3_finalize(statement);

finished:
    sqlite3_close(ctx);
    info("Done.");
    return result;
}
