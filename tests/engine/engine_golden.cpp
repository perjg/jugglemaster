// Engine golden test: hashes every object and hand position of the JuggleSaver engine over a period of time
// for a fixed set of patterns, and compares against tests/engine/golden.txt.
//
//   engine_golden check  <moresites.txt> <golden.txt>   exit 1 on any difference
//   engine_golden record <moresites.txt> <golden.txt>   rewrite the golden file
//   engine_golden dump   <pattern>                      print the positions of one pattern
#include "jugglesaver.h"
#include "js_validator.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>
#include <vector>

static const int kSamples = 400;
static const float kStart = 50.0f;
static const float kStep = 0.073f;

struct Hasher {
    uint64_t h = 1469598103934665603ULL;
    void add(const void* p, size_t n) {
        const unsigned char* b = (const unsigned char*)p;
        for (size_t i = 0; i < n; i++) { h ^= b[i]; h *= 1099511628211ULL; }
    }
    void add(const POS& p) { add(&p.x, sizeof(float)); add(&p.y, sizeof(float)); add(&p.z, sizeof(float));
                             add(&p.Rot, sizeof(float)); add(&p.Elev, sizeof(float)); }
};

static void walk(PATTERN_INFO* p, Hasher* h, FILE* dump) {
    h->add(&p->Objects, sizeof(int));
    h->add(&p->Height, sizeof(float));
    h->add(&p->Alpha, sizeof(float));
    for (int k = 0; k < kSamples; k++) {
        float t = kStart + k * kStep;
        for (int i = 0; i < p->Objects; i++) {
            POS pos;
            float handle = p->pObjectInfo[i].ObjectType == OBJECT_BALL ? 0.0f : 1.0f;
            GetObjectPosition(p, i, t, handle, &pos);
            h->add(pos);
            if (dump) printf("t=%a obj%d %a %a %a %a %a\n", t, i, pos.x, pos.y, pos.z, pos.Rot, pos.Elev);
        }
        for (int hand = 0; hand < 2; hand++) {
            POS pos;
            GetHandPosition(p, hand, t, &pos);
            h->add(&pos.x, sizeof(float));
            h->add(&pos.y, sizeof(float));
            if (dump) printf("t=%a hand%d %a %a\n", t, hand, pos.x, pos.y);
        }
    }
}

static bool hashPattern(const char* site, uint64_t* out, FILE* dump) {
    if (!JSValidator::validateJSPattern(const_cast<char*>(site))) return false;
    int len = 0;
    EXT_SITE_INFO* ext = ParsePattern(site, &len);
    if (!ext) return false;
    srand(1);
    PATTERN_INFO p;
    InitPatternInfo(&p, NULL, ext, len);
    free(ext);
    Hasher h;
    walk(&p, &h, dump);
    ReleasePatternInfo(&p);
    *out = h.h;
    return true;
}

static uint64_t hashGenerated(int len, int maxWeight, int objects) {
    int* site = Generate(len, maxWeight, objects);
    PATTERN_INFO p;
    InitPatternInfo(&p, site, NULL, len);
    Hasher h;
    h.add(site, sizeof(int) * len);
    walk(&p, &h, NULL);
    ReleasePatternInfo(&p);
    free(site);
    return h.h;
}

static std::vector<std::string> librarySites(const char* path) {
    std::vector<std::string> sites;
    FILE* f = fopen(path, "r");
    if (!f) { perror(path); exit(2); }
    char line[1024];
    while (fgets(line, sizeof line, f)) {
        if (line[0] != '"') continue;
        char* close = strchr(line + 1, '"');
        char* end = close ? strchr(close, ';') : NULL;
        if (!end) continue;
        *end = '\0';
        char* s = close + 1;
        while (*s == ' ' || *s == '\t') s++;
        sites.push_back(s);
    }
    fclose(f);
    return sites;
}

static std::map<std::string, uint64_t> computeAll(const char* moresites) {
    std::map<std::string, uint64_t> result;
    std::vector<std::string> sites = {
        "3", "531", "97531", "3b", "3b@(-0.5,0)>(1.5,0) 3b@(0.5,0)>(-1.5,0)",
        "7c@(-2,0,-20)>(1.2,0,-5)7c@(2,0,20)>(-1.2,0,5)",
        "3r@(-2.5,3,-90,80)>(2,1,90,30) 3r@(2.5,3,90,80)>(-2,1,-90,30)",
        "5c", "5c3b4r", "9b@(-2.5,0,-70,40)>(2.5,0,70)*2 1b@(1,0,10)>(-1,0,-10)", "60", "62", "6c1c61c601",
    };
    for (const std::string& s : librarySites(moresites)) sites.push_back(s);
    for (const std::string& s : sites) {
        uint64_t h;
        if (hashPattern(s.c_str(), &h, NULL)) result["site " + s] = h;
    }
    srand(1234);
    const int generated[][3] = {{60, 5, 3}, {60, 7, 4}, {60, 8, 5}, {90, 9, 6}, {90, 10, 7}, {120, 11, 7}};
    for (const auto& g : generated) {
        char key[64];
        snprintf(key, sizeof key, "generated %d %d %d", g[0], g[1], g[2]);
        result[key] = hashGenerated(g[0], g[1], g[2]);
    }
    return result;
}

int main(int argc, char** argv) {
    if (argc == 3 && strcmp(argv[1], "dump") == 0) {
        uint64_t h;
        if (!hashPattern(argv[2], &h, stdout)) { fprintf(stderr, "invalid pattern\n"); return 2; }
        return 0;
    }
    if (argc != 4) { fprintf(stderr, "usage: engine_golden check|record <moresites.txt> <golden.txt>\n"); return 2; }
    std::map<std::string, uint64_t> now = computeAll(argv[2]);

    if (strcmp(argv[1], "record") == 0) {
        FILE* f = fopen(argv[3], "w");
        if (!f) { perror(argv[3]); return 2; }
        for (const auto& e : now) fprintf(f, "%016llx %s\n", (unsigned long long)e.second, e.first.c_str());
        fclose(f);
        printf("recorded %zu patterns\n", now.size());
        return 0;
    }

    FILE* f = fopen(argv[3], "r");
    if (!f) { perror(argv[3]); return 2; }
    std::map<std::string, uint64_t> golden;
    char line[2048];
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) < 18) continue;
        golden[line + 17] = strtoull(line, NULL, 16);
    }
    fclose(f);

    int failures = 0;
    for (const auto& e : golden) {
        auto it = now.find(e.first);
        if (it == now.end()) { printf("MISSING %s\n", e.first.c_str()); failures++; }
        else if (it->second != e.second) { printf("CHANGED %s\n", e.first.c_str()); failures++; }
    }
    for (const auto& e : now)
        if (!golden.count(e.first)) { printf("NEW %s\n", e.first.c_str()); failures++; }
    printf("%zu patterns, %d differences\n", golden.size(), failures);
    return failures == 0 ? 0 : 1;
}
