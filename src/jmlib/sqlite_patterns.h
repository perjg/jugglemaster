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
#include "sqlite/sqlite3.h"

class SearchResult;

class PatternLoader {
  friend class SearchResult;
public:
  PatternLoader(char* filename);
  
  void search(char* name);
  void search(char* name, char* site, char* style, int balls); 


	/*
	 * @param out   File pointer for writing the sqlite database
	 * @param inJM  JuggleMaster file
	 * @param inJS  JuggleSaver file
	 */
	static void initializeDatabase(FILE* out, FILE* inJM, FILE* inJS);
private:
  char* filename_;
  void init();
  
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
