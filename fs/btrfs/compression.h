/*
 * Copyright (C) 2008 Oracle.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License v2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 021110-1307, USA.
 */

#ifndef __BTRFS_COMPRESSION_
#define __BTRFS_COMPRESSION_

struct compressed_bio {
	/* number of bios pending for this compressed extent */
	atomic_t pending_bios;

	/* the pages with the compressed data on them */
	struct page **compressed_pages;

	/* inode that owns this data */
	struct inode *inode;

	/* starting offset in the inode for our pages */
	u64 start;

	/* number of bytes in the inode we're working on */
	unsigned long len;

	/* number of bytes on disk */
	unsigned long compressed_len;

	/* the compression algorithm for this bio */
	int compress_type;

	/* number of compressed pages in the array */
	unsigned long nr_pages;

	/* IO errors */
	int errors;
	int mirror_num;

	/* for reads, this is the bio we are copying the data into */
	struct bio *orig_bio;

	/*
	 * the start of a variable length array of checksums only
	 * used by reads
	 */
	u32 sums;
};

void btrfs_init_compress(void);
void btrfs_exit_compress(void);

int btrfs_compress_pages(int type, struct address_space *mapping,
			 u64 start, unsigned long len,
			 struct page **pages,
			 unsigned long nr_dest_pages,
			 unsigned long *out_pages,
			 unsigned long *total_in,
			 unsigned long *total_out,
			 unsigned long max_out);
int btrfs_decompress(int type, unsigned char *data_in, struct page *dest_page,
		     unsigned long start_byte, size_t srclen, size_t destlen);
int btrfs_decompress_buf2page(char *buf, unsigned long buf_start,
			      unsigned long total_out, u64 disk_start,
			      struct bio_vec *bvec, int vcnt,
			      unsigned long *pg_index,
			      unsigned long *pg_offset);

int btrfs_submit_compressed_write(struct inode *inode, u64 start,
				  unsigned long len, u64 disk_start,
				  unsigned long compressed_len,
				  struct page **compressed_pages,
				  unsigned long nr_pages);
int btrfs_submit_compressed_read(struct inode *inode, struct bio *bio,
				 int mirror_num, unsigned long bio_flags);
void btrfs_clear_biovec_end(struct bio_vec *bvec, int vcnt,
				   unsigned long pg_index,
				   unsigned long pg_offset);
struct btrfs_compress_op {
	struct list_head *(*alloc_workspace)(void);

	void (*free_workspace)(struct list_head *workspace);

	int (*compress_pages)(struct list_head *workspace,
			      struct address_space *mapping,
			      u64 start, unsigned long len,
			      struct page **pages,
			      unsigned long nr_dest_pages,
			      unsigned long *out_pages,
			      unsigned long *total_in,
			      unsigned long *total_out,
			      unsigned long max_out);

	int (*decompress_biovec)(struct list_head *workspace,
				 struct page **pages_in,
				 u64 disk_start,
				 struct bio_vec *bvec,
				 int vcnt,
				 size_t srclen);

	int (*decompress)(struct list_head *workspace,
			  unsigned char *data_in,
			  struct page *dest_page,
			  unsigned long start_byte,
			  size_t srclen, size_t destlen);
};

extern const struct btrfs_compress_op btrfs_zlib_compress;
extern const struct btrfs_compress_op btrfs_lzo_compress;
extern const struct btrfs_compress_op btrfs_zstd_compress;

#endif
