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

#include "patterns.h"
extern "C" {
#include "sqlite/sqlite3.h"
}

class SearchResult;

class JMPatterns {
  friend class SearchResult;
public:
  JMPatterns() : filename_(NULL), db_(NULL) {}
  ~JMPatterns();
  
  /** Search for a pattern
   * Matches any category name, pattern name, style name or site
   */
  pattern_t* search(const char* item);

  /** Search for a pattern 
   * @param name Matches category or pattern name
   * @param site Matches site
   * @param style Matches style
   * @param minBalls Minimum amount of balls in pattern, 0 for any
   * @param maxBalls Maximum amount of balls in pattern, 0 for any
   */
  pattern_t* search(const char* name, const char* site, const char* style, int minBalls, int maxBalls);

  /** Returns patterns for the named category */
  pattern_t* getCategory(const char* category);
  
  /** Returns the most recent count patterns loaded */
  pattern_t* getRecentPatterns(int count);

  /** Gets a random pattern.
   * Patterns can contain a list of categories to include or exclude in the selection
   */
  pattern_t* getRandomPattern(const char** categories, bool exclude = true);
  
  void loadPattern(const pattern_t* patt, JMLib* jm);
  void loadNextPattern(const pattern_t* patt, JMLib* jm);
  void loadPrevPattern(const pattern_t* patt, JMLib* jm);
  void loadRandomPattern(JMLib* jm, const char** categories, bool exclude = true);
  
	void freeSearchResult(pattern_t* patterns);

	/*
	 * @param out   File pointer for writing the sqlite database
	 * @param inJM  JuggleMaster file
	 * @param inJS  JuggleSaver file
	 */
	void initializeDatabase(FILE* out, FILE* inJM, FILE* inJS);
  
  void loadDatabase(FILE* db);
private:
  void createDB();
  void closeDB();
  void queryDB(const char* query);
  void addCategory(pattern_group_t* group);
  void addPattern(pattern_t* patt, pattern_group_t* group);
  void addStyle(style_t* style);
	pattern_t* searchQuery(const char* query);
  

  char* filename_;
  void init();
  sqlite3* db_;
};

class SearchResult {
public:
  int getHits();
  pattern_t* getPattern();
  style_t*   getStyle();
  pattern_group_t* getCategory();
  void loadCurrent();
  void first();
  void next();
  void previous();
  void last();
private:

};

/*

- sqlite pattern loader
 * search by pattern type / name / style

 JMLib -> initPatternLoader(filename);
 (loads pattern file, creates sql db if not exists or out of date)

 DB: * general info table (gravity etc.)
     * categories -> name, description
     * pattern -> name, site, style

 JMSearchResult* beginSearch(hits, name, category, style);
 JMPattern* getFirst(JMSearchResult*)
 JMPattern* getNext(JMSearchResult*)
 JMPattern* getAt(JMSearchResult*)
 void endSearch(JMSearchResult*)

 loadPattern(JMSearchResult*)
 loadPattern(JMSearchResult*, pos)

*/
