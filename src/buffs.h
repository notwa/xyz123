/* buffs.h - simplified buffered streams
 * Copyright (C) 2012 Connor Olding
 *
 * This program is licensed under the terms of the GNU General Public
 * License (version 3), and is distributed without any warranty.
 * You should have received a copy of the license in the file COPYING.
 * If not, visit <http://gnu.org/licenses/> to obtain one.
 */

/* If we're using Microsoft's compiler, set the appropriate
 * argument for freopen. If we don't do this, the program will
 * compile but not run. Thanks, M$. */

typedef struct buffs_s {
	FILE* stream;
	uint size;
	char* buffer;
} buffs_t;

buffs_t* new_buffs(uint size);
void free_buffs(buffs_t* buffs);

void buffs_open_input(buffs_t* buffs, char* filename);
void buffs_open_output(buffs_t* buffs, char* filename);

void buffs_close(buffs_t* buffs);

