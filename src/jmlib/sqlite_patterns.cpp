/*
 * JMLib - Portable JuggleMaster Library
 * Version 2.1
 * (C) Per Johan Groland 2000-2008, Gary Briggs 2003
 *
 * Based on JuggleMaster Version 1.60
 * Copyright (c) 1995-1996 Ken Matsuoka
 *
 * JuggleSaver support based on Juggler3D
 * Copyright (c) 2005-2008 Brian Apps <brian@jugglesaver.co.uk>
 *
 * You may redistribute and/or modify JMLib under the terms of the
 * Modified BSD License as published in various places online or in the
 * COPYING.jmlib file in the package you downloaded.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the
 * Modified BSD License for more details.
 */ 

#include "jmlib.h"
#include "sqlite_patterns.h"
#include "patterns.h"
#include "jugglesaver/js_patterns.h"

/*
 * pattern: name, data, style, author, hr, dr
 * style: name, data, length
 * camera placement: see SetCamera for variables
 *
 */

// create an empty sqlite database
void JMPatterns::createDB() {
  const char* sql = "DROP TABLE IF EXISTS CameraData;"
                    "CREATE TABLE CameraData (id PRIMARY KEY);"
                    "DROP TABLE IF EXISTS Category;"
                    "CREATE TABLE Category (name PRIMARY KEY);"
                    "DROP TABLE IF EXISTS Pattern;"
                    "CREATE TABLE Pattern (name PRIMARY KEY, data NOT NULL, category REFERENCES Category, style NOT NULL,"
                      "author, hr, dr, cameradata REFERENCES CameraData);"
                    "DROP TABLE IF EXISTS Style;"
                    "CREATE TABLE Style (name PRIMARY KEY, data NOT NULL, length NOT NULL);";
  
  const char* dbname = "test.db";
  char* zErr;
  int rc;

  rc = sqlite3_open(dbname, &db_);
  if(rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_));
    sqlite3_close(db_);
    return;
  }

  rc = sqlite3_exec(db_, sql, NULL, NULL, &zErr);
  if(rc != SQLITE_OK) {
    if (zErr != NULL) {
      fprintf(stderr, "SQL error: %s\n", zErr);
      sqlite3_free(zErr);
    }
  }
}

void JMPatterns::queryDB(const char* query) {
  char* zErr;
  int rc = sqlite3_exec(db_, query, NULL, NULL, &zErr);
  if(rc != SQLITE_OK) {
    if (zErr != NULL) {
      fprintf(stderr, "SQL error: %s\n", zErr);
      fprintf(stderr, "\tStatement: '%s'\n", query);
      sqlite3_free(zErr);
    }
  }
}

void JMPatterns::addCategory(pattern_group_t* group) {
  char* sql = sqlite3_mprintf("INSERT INTO Category VALUES (%Q)", group->name);
  queryDB(sql);  
  sqlite3_free(sql);
}

void JMPatterns::addPattern(pattern_t* patt, pattern_group_t* group) {
  char* sql = sqlite3_mprintf("INSERT INTO Pattern VALUES (%Q, %Q, %Q, %Q, %Q, %f, %f, NULL)",
          patt->name, patt->data, group->name, patt->style, patt->author, patt->hr, patt->dr);
  queryDB(sql);
  sqlite3_free(sql);
}

void JMPatterns::addStyle(style_t* style) {
  char* sql = sqlite3_mprintf("INSERT INTO Style VALUES (%Q, %Q, %d)", style->name, style->data, style->length);
  queryDB(sql);
  sqlite3_free(sql);
}

void JMPatterns::closeDB() {
  if (db_) sqlite3_close(db_);
}

// initializes an sql database from JuggleMaster and/or JuggleSaver pattern files
void JMPatterns::initializeDatabase(FILE* out, FILE* inJM, FILE* inJS) {
	groups_t groups;
  groups.first = NULL;
	styles_t styles;
  styles.first = NULL;
  int result = false;

  createDB();

	//if (!out) return;
	if (!inJM && !inJS) return;

	if (inJM && !inJS) {
		result = ParsePatterns(inJM, &groups, &styles);
  }
  else if (inJS && !inJM) {
    result = ParseJSPatterns(inJS, &groups);
  }
  else { // both
    result = ParseAllPatterns(inJM, inJS, &groups, &styles);
  }

	pattern_group_t* cur_group = groups.first;
	pattern_t* cur_pattern = NULL;

	while (cur_group) {
    addCategory(cur_group);
  
		cur_pattern = cur_group->first_patt;

		while (cur_pattern) {
      addPattern(cur_pattern, cur_group);
			cur_pattern = cur_pattern->next;
		}

		cur_group = cur_group->next;
	}

	style_t* cur_style = styles.first;

	while (cur_style) {
    addStyle(cur_style);  
		cur_style = cur_style->next;
	}
}

/*
#include "sqlite/sqlite3.h"
#include "unicode.h"
#include "db.h"

#include <stdio.h>

void zidict_version(sqlite3_context* ctx, int nargs, sqlite3_value** values) {
  sqlite3_result_int(ctx, 1);
}

int test_collation(void* data, int len1, const void* d1, int len2, const void* d2) {
  static int tests = 0;
  unichar cur1, cur2;
  int read1, read2;
  u8char* str1 = (u8char*)d1;
  u8char* str2 = (u8char*)d2;

  *(str1+len1) = 0;
  *(str2+len2) = 0;

  do {
    cur1 = u8_iterate(str1, &read1);
    str1 += read1;

    cur2 = u8_iterate(str2, &read2);
    str2 += read2;

    if (cur1 != cur2) return (cur2-cur1);
  } while (read1 > 0 && read2 > 0);

  if (read1 == 0 && read2 != 0)
    return -1;
  if (read2 == 0 && read1 != 0)
    return 1;
  return 0;
  //printf("Comparing %s with %s (%d)\n", (char*)str1, (char*)str2, tests++);
  //return 1;
}

void db_register_extensions(sqlite3 *db) {
  sqlite3_create_function(db, "zidict_version", 0, SQLITE_ANY , NULL,
                          zidict_version, NULL, NULL);

  sqlite3_create_collation(db, "TEST", SQLITE_UTF8, NULL, test_collation);
}

int execute(sqlite3 *db, const char* sql, ...) {
  int rc;
  char *err, *tmp;

  va_list ap;
  va_start(ap, sql);
  tmp = sqlite3_vmprintf(sql, ap);
  va_end(ap);

  rc = sqlite3_exec(db, tmp, NULL, NULL, &err);
  if(rc != SQLITE_OK) {
    if (err != NULL) {
      //fprintf(stdout, "execute() : Error %i : %s\n", rc, err);
      sqlite3_free(err);
    }
  }
  sqlite3_free(tmp);
  return rc;
}

*/

