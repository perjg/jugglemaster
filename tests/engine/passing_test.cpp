// Engine tests for several jugglers sharing one site (J<n>: prefix).
#include "jugglesaver.h"
#include "js_validator.h"
#include "js_passing.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond, ...)                                    \
    do {                                                    \
        if (!(cond)) {                                      \
            printf("FAIL %s:%d: ", __FILE__, __LINE__);     \
            printf(__VA_ARGS__);                            \
            printf("\n");                                   \
            failures++;                                     \
        }                                                   \
    } while (0)

struct Pattern {
    PATTERN_INFO info;
    explicit Pattern(const char* site) {
        int len = 0;
        PASSING_LAYOUT layout;
        EXT_SITE_INFO* ext = ParsePatternEx(site, &len, &layout);
        if (!ext) { printf("FAIL cannot parse %s\n", site); exit(1); }
        srand(1);
        InitPatternInfoEx(&info, NULL, ext, len, &layout);
        free(ext);
    }
    ~Pattern() { ReleasePatternInfo(&info); }
};

static float NormaliseRot(float a) {
    while (a > 180.0f) a -= 360.0f;
    while (a <= -180.0f) a += 360.0f;
    return a;
}

static float dist(const POS& a, const POS& b) {
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

static void axis(const POS& p, float* v) {
    float r = p.Rot * PI / 180.0f, e = p.Elev * PI / 180.0f;
    v[0] = sinf(r) * cosf(e);
    v[1] = sinf(e);
    v[2] = cosf(r) * cosf(e);
}

static int shiftOf(const PATTERN_INFO* p, int slot) {
    return p->HandShift[slot % (2 * p->Jugglers)];
}

static bool inFlight(const PATTERN_INFO* p, int obj, float t) {
    const OBJECT_POSITION* o = &p->pObjectInfo[obj];
    const THROW_INFO* th = &p->pThrowInfo[o->ThrowIndex];
    if (th->TimeInAir == 0 || th->TimeInAir == 2 * p->Jugglers) return false;
    int shiftFrom = shiftOf(p, o->TimeOffset);
    int flight = th->TimeInAir + shiftOf(p, o->TimeOffset + th->TimeInAir) - shiftFrom;
    return t - (o->TimeOffset + shiftFrom) < flight - p->CarryTime;
}

static void testValidator() {
    const char* valid[] = {"J2:966", "J2:7", "J2:7c", "J2:6", "J2:86277", "J2:786", "J2:975", "J3:8", "J1:3"};
    for (const char* s : valid)
        CHECK(JSValidator::validateJSPattern(const_cast<char*>(s)), "%s should be valid", s);
    const char* invalid[] = {"J2:965", "J2:31", "J9:3", "J0:3", "J2:", "J2:9x", "J3:2"};
    for (const char* s : invalid)
        CHECK(!JSValidator::validateJSPattern(const_cast<char*>(s)), "%s should be invalid", s);
    CHECK(JSValidator::jugglerCount("J3:8") == 3, "J3:8 has 3 jugglers");
    CHECK(JSValidator::jugglerCount("5c") == 1, "5c has 1 juggler");

    int len;
    CHECK(ParsePattern("J2:966", &len) == NULL, "ParsePattern rejects passing sites");
    PASSING_LAYOUT layout;
    free(ParsePatternEx("J2:9c6c6c", &len, &layout));
    char* site = GetCurrentSite();
    CHECK(strcmp(site, "966") == 0 && layout.Jugglers == 2 && len == 3, "J2:9c6c6c strips to 966");
    free(site);
}

static void testStructure() {
    struct { const char* site; int objects; } counts[] = {
        {"J2:966", 7}, {"J2:7", 7}, {"J2:6", 6}, {"J2:86277", 6}, {"J2:786", 7}, {"J2:975", 7}, {"J3:8", 8}};
    for (const auto& c : counts) {
        Pattern p(c.site);
        CHECK(p.info.Objects == c.objects, "%s has %d objects, not %d", c.site, p.info.Objects, c.objects);
        CHECK(p.info.ThrowLen % (2 * p.info.Jugglers) == 0, "%s: whole hand cycles", c.site);
        for (int s = 0; s < p.info.ThrowLen; s++) {
            const THROW_INFO* t = &p.info.pThrowInfo[s];
            CHECK(t->IsPass == (t->TimeInAir % p.info.Jugglers != 0), "%s slot %d pass flag", c.site, s);
        }
    }

    Pattern seven("J2:7");
    for (int s = 0; s < seven.info.ThrowLen; s++)
        CHECK(seven.info.pThrowInfo[s].IsPass, "every throw of J2:7 is a pass");

    // Two cascades side by side: each juggler's objects stay on its side
    Pattern six("J2:6");
    for (int k = 0; k < 600; k++) {
        float t = 50.0f + k * 0.05f;
        for (int i = 0; i < six.info.Objects; i++) {
            POS pos;
            GetObjectPosition(&six.info, i, t, 0.0f, &pos);
            int side = six.info.pObjectInfo[i].TimeOffset % 2;  // slot parity is the juggler
            CHECK(side == 0 ? pos.x < 0.0f : pos.x > 0.0f, "J2:6 object %d crossed over at %g", i, t);
        }
    }

    // Each juggler's hands stay on their own side of the formation
    Pattern three("J3:8");
    for (int k = 0; k < 400; k++) {
        float t = 50.0f + k * 0.05f;
        for (int h = 0; h < 6; h++) {
            POS pos, local;
            int j = h % 3;
            GetHandPositionEx(&three.info, h, t, &pos);
            WorldToJuggler(&three.info, j, &pos, &local);
            CHECK(fabsf(local.x) < 4.0f && local.z > -1.0f && local.z < 2.5f,
                  "J3:8 hand %d strays from its juggler at %g (%g, %g)", h, t, local.x, local.z);
        }
    }
}

// Hands and objects move smoothly (a looser limit where solo patterns with the
// same high throws, such as 8c4c, already jump 0.22 at the throw)
static void testContinuity(const char* site, float limit = 0.15f) {
    Pattern p(site);
    const float dt = 0.002f;
    const int steps = (int)(p.info.ThrowLen * 2 / dt);
    float maxHand = 0.0f, maxObj = 0.0f, minDot = 1.0f;

    for (int h = 0; h < 2 * p.info.Jugglers; h++) {
        POS prev;
        GetHandPositionEx(&p.info, h, 50.0f, &prev);
        for (int k = 1; k < steps; k++) {
            POS pos;
            GetHandPositionEx(&p.info, h, 50.0f + k * dt, &pos);
            maxHand = fmaxf(maxHand, dist(pos, prev));
            prev = pos;
        }
    }
    for (int i = 0; i < p.info.Objects; i++) {
        POS prev;
        float handle = p.info.pObjectInfo[i].ObjectType == OBJECT_BALL ? 0.0f : 1.0f;
        GetObjectPosition(&p.info, i, 50.0f, handle, &prev);
        for (int k = 1; k < steps; k++) {
            POS pos;
            GetObjectPosition(&p.info, i, 50.0f + k * dt, handle, &pos);
            maxObj = fmaxf(maxObj, dist(pos, prev));
            if (handle > 0.0f) {
                float a[3], b[3];
                axis(prev, a);
                axis(pos, b);
                minDot = fminf(minDot, a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
            }
            prev = pos;
        }
    }
    CHECK(maxHand < limit, "%s: hand jumps %g", site, maxHand);
    CHECK(maxObj < limit, "%s: object jumps %g", site, maxObj);
    CHECK(minDot > 0.95f, "%s: club orientation jumps (dot %g)", site, minDot);
}

// Every hand stays within arm's reach of its shoulder (plus slack: solo
// patterns as old as 5c already overreach by 0.1)
static void testReach(const char* site, float slack = 0.05f) {
    Pattern p(site);
    const float reach = 1.9f + 2.3f + slack;
    float worst = 0.0f;
    for (int h = 0; h < 2 * p.info.Jugglers; h++) {
        int j = h % p.info.Jugglers, side = h / p.info.Jugglers;
        for (int k = 0; k < p.info.ThrowLen * 50; k++) {
            POS pos, local;
            GetHandPositionEx(&p.info, h, 50.0f + k * 0.02f, &pos);
            WorldToJuggler(&p.info, j, &pos, &local);
            float dx = local.x - (side ? 0.95f : -0.95f), dy = local.y - 2.1f, dz = local.z + 1.7f;
            worst = fmaxf(worst, sqrtf(dx * dx + dy * dy + dz * dz));
        }
    }
    CHECK(worst <= reach, "%s: a hand is %g from its shoulder", site, worst);
}

// Objects in the air keep two ball radii apart (a near miss allowed for the hardest patterns)
static void testSeparation(const char* site, float minimum = 0.68f) {
    Pattern p(site);
    float minDist = 1e9f;
    for (int k = 0; k < p.info.ThrowLen * 40; k++) {
        float t = 50.0f + k * 0.025f;
        POS pos[64];
        bool air[64];
        for (int i = 0; i < p.info.Objects; i++) {
            GetObjectPosition(&p.info, i, t, 0.0f, &pos[i]);
            air[i] = inFlight(&p.info, i, t);
        }
        for (int i = 0; i < p.info.Objects; i++)
            for (int j = i + 1; j < p.info.Objects; j++)
                if (air[i] && air[j]) minDist = fminf(minDist, dist(pos[i], pos[j]));
    }
    CHECK(minDist >= minimum, "%s: objects in flight come %g apart", site, minDist);
}

// The interleaved site and hand shifts a passing notation becomes
static void expectSite(const char* text, const char* site, int jugglers, const int* shifts) {
    int len;
    PASSING_LAYOUT layout;
    EXT_SITE_INFO* ext = ParsePatternEx(text, &len, &layout);
    char* got = GetCurrentSite();
    CHECK(ext != NULL && strcmp(got, site) == 0 && layout.Jugglers == jugglers, "%s -> %s (%d jugglers), not %s",
          text, got, layout.Jugglers, site);
    for (int h = 0; shifts && h < 2 * jugglers; h++)
        CHECK(layout.HandShift[h] == shifts[h], "%s: hand %d shift %d, not %d", text, h, layout.HandShift[h], shifts[h]);
    free(got);
    free(ext);
}

static void testNotations() {
    // every juggler starts with the right hand, which may shift its throws
    const int none[16] = {0};
    const int half[4] = {0, 2, 0, 2};
    expectSite("3.5p", "77", 2, none);
    expectSite("4.5p 3 3", "7B6666", 2, half);
    expectSite("4.5pc 3c 3c", "7B6666", 2, half);
    expectSite("J2:3.5p", "77", 2, none);
    expectSite("3.33p", "AAA", 3, none);

    // the second juggler's throws are shifted so both throw together
    const int second[4] = {0, 1, 0, 1};
    // whole-beat Prechac that does not work staggered is read with everyone throwing together
    expectSite("3p 3p 3 3", "36366969", 2, (const int[4]){0, 3, 0, 3});
    const int sync[4] = {0, 3, 0, 3};
    expectSite("4p 3", "7966", 2, second);
    expectSite("<3p|3p>", "39", 2, sync);
    expectSite("<3p 3 3 3|3p 3 3 3>", "36666966", 2, sync);

    // solo sync: the left hand is shifted back to throw with the right
    const int left[2] = {0, -1};
    expectSite("(4c,4c)", "44", 1, left);
    expectSite("(4c,2cx)(2cx,4c)", "4134", 1, left);

    PASSING_PATTERN* p = (PASSING_PATTERN*) malloc(sizeof(PASSING_PATTERN));
    CHECK(ParsePassing("966", p) == 0, "plain sites are not passing notation");
    CHECK(ParsePassing("J2:966", p) == 0, "four-handed sites are not passing notation");
    CHECK(ParsePassing("3b@(-0.5,0)>(1.5,0) 3b@(0.5,0)>(-1.5,0)", p) == 0, "positions are not Prechac");
    CHECK(ParsePassing("3.5p", p) == 1 && p->Jugglers == 2 && p->Rows[0].Count == 1, "3.5p has 2 jugglers");
    CHECK(ParsePassing("3.33p 3 3", p) == 1 && p->Jugglers == 3, "3.33p means 3 jugglers");
    CHECK(ParsePassing("4.5p 3 3", p) == 1 && p->Rows[1].Tokens[1].Start == 5 && p->Rows[1].Tokens[1].Stop == 6,
          "token positions");
    CHECK(ParsePassing("<3p 3|3p 3 3 3>", p) == 1 && p->Rows[1].TextStart == 6 && p->Rows[1].TextStop == 14,
          "bar positions");
    CHECK(ParsePassing("<3p2 3p3|3p1 3|3 3p1>", p) == 1 && p->Formation == FORMATION_FEED && p->Feeder == 0,
          "a feed is recognised");
    CHECK(ParsePassing("4.5pc*2 3c 3c", p) == 1 && strcmp(p->Rows[0].Tokens[0].Suffix, "c*2") == 0,
          "throw syntax after the pass is kept");
    free(p);

    const char* valid[] = {"3.5p", "4.5p 3 3", "4.5pc 3c 3c", "3.67p 3", "4p 3", "<3p|3p>", "<3p 3|3p 3>",
                           "<3p 3 3 3|3p 3 3 3>", "<3p2 3p3|3p1 3|3 3p1>", "(4c,4c)", "(4c,2cx)(2cx,4c)",
                           "4.5pc*2 3c 3c", "4.5pc@(-1,0)>(1,0.5) 3c 3c",
                           "3p 3", "3p 3 3 3", "3p 3p 3 3", "3.5p 4.5p 2.5p"};
    for (const char* s : valid)
        CHECK(JSValidator::validateJSPattern(const_cast<char*>(s)), "%s should be valid", s);
    const char* invalid[] = {"3.5p 3.5p 3 3 3", "4.5 3 3", "<3p|3p 3 3>", "<3p>", "(3c,4c)", "(0x,4)",
                             "<3p9|3p>", "4.5pq 3 3"};
    for (const char* s : invalid)
        CHECK(!JSValidator::validateJSPattern(const_cast<char*>(s)), "%s should be invalid", s);
    CHECK(JSValidator::jugglerCount("<3p2 3p3|3p1 3|3 3p1>") == 3, "the feed has 3 jugglers");

    // solo sync takes throw syntax after the value and its x
    CHECK(JSValidator::validateJSPattern(const_cast<char*>("(4c@(-1,0)>(1,0),4c@(1,0)>(-1,0))")), "positions in sync");
    CHECK(JSValidator::validateJSPattern(const_cast<char*>("(4c*2,4c*2)")), "spins in sync");
    CHECK(JSValidator::validateJSPattern(const_cast<char*>("(4xc,2c)(2c,4xc)")), "x before the object");

    // jugglers who never pass are not a feed
    p = (PASSING_PATTERN*) malloc(sizeof(PASSING_PATTERN));
    CHECK(ParsePassing("<3|3|3>", p) == 1 && p->Formation == FORMATION_AUTO, "no passes, no feed");
    CHECK(ParsePassing("<3p2 3|3p1 3|3 3>", p) == 1 && p->Formation == FORMATION_AUTO, "a feeder feeds everyone");

    // a period too long to lay out is refused, not overflowed
    char longSync[2048] = "<";
    const int counts[] = {61, 59, 53, 47, 43, 41, 37, 31};
    for (int j = 0; j < 8; j++) {
        for (int k = 0; k < counts[j]; k++) strcat(longSync, k == 0 ? "3" : " 3");
        strcat(longSync, j < 7 ? "|" : ">");
    }
    CHECK(ParsePassing(longSync, p) == 1, "long sync parses");
    PASSING_LAYOUT layout;
    CHECK(PassingToSite(p, &layout) == NULL, "long sync is refused");
    free(p);

    // mirror-image sync throws look the same from either hand: in the box the
    // right 4 (slot 0) mirrors the left 4 (slot 3), the right 2x (2) the left 2x (1)
    struct { const char* site; int a, b; } mirrors[] = {
        {"(4cx,4cx)", 0, 1}, {"(4c,2cx)(2cx,4c)", 0, 3}, {"(4c,2cx)(2cx,4c)", 2, 1}};
    for (const auto& m : mirrors) {
        Pattern sync(m.site);
        const THROW_INFO* r = &sync.info.pThrowInfo[m.a];
        const THROW_INFO* l = &sync.info.pThrowInfo[m.b];
        CHECK(fabsf(r->FromPos.x + l->FromPos.x) < 1e-4f && fabsf(r->ToPos.x + l->ToPos.x) < 1e-4f &&
                  fabsf(r->ToPos.Elev - l->ToPos.Elev) < 0.01f,
              "%s: slots %d and %d are not mirror images", m.site, m.a, m.b);
    }
    CHECK(!JSValidator::validateJSPattern(const_cast<char*>("<3p1 3|3p2 3>")), "a pass to oneself is refused");
    CHECK(!JSValidator::validateJSPattern(const_cast<char*>("1e1p 3")), "only plain numbers");

    // passes of the same length get the same spins
    {
        Pattern every("<3pc|3pc>");
        float elev[2];
        for (int s = 0; s < 2; s++) {
            POS to;
            WorldToJuggler(&every.info, s % 2, &every.info.pThrowInfo[s].ToPos, &to);
            elev[s] = to.Elev;
        }
        CHECK(fabsf(elev[0] - elev[1]) < 0.01f, "<3p|3p> spins the same both ways (%g, %g)", elev[0], elev[1]);
    }
    CHECK(JSValidator::jugglerCount("(4c,4c)") == 1, "solo sync has 1 juggler");
}

// Fields a pattern leaves out come from each slot's own defaults, and a
// snatch is sideways for the catcher
static void testGivenPositions() {
    Pattern p("J2:6c@(0,0)>(0,0)/(5,0)");
    for (int s = 0; s < 4; s++) {
        const THROW_INFO* t = &p.info.pThrowInfo[s];
        POS from;
        WorldToJuggler(&p.info, s % 2, &t->FromPos, &from);
        float rot = NormaliseRot(from.Rot);
        CHECK(fabsf(rot - (s < 2 ? 27.0f : -27.0f)) < 0.01f, "slot %d keeps its side's rotation (%g)", s, rot);
        CHECK(fabsf(from.x) < 1e-4f, "slot %d throws from the given x", s);
        CHECK(fabsf(t->ToVelocity.x) < 1e-4f && fabsf(fabsf(t->ToVelocity.z) - 5.0f) < 1e-4f,
              "slot %d snatch turned with the catcher", s);
    }
}

// Every pattern in the Passing and Synchronous sections of the library juggles cleanly
static void testLibrary(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) { printf("FAIL cannot open %s\n", path); failures++; return; }
    char line[1024];
    bool inPassing = false, inSync = false;
    int count = 0;
    while (fgets(line, sizeof line, f)) {
        if (line[0] == '$') {
            if (strncmp(line, "$ Passing", 9) == 0) inPassing = true, inSync = false;
            else if (strncmp(line, "$ Synchronous", 13) == 0) inSync = true, inPassing = false;
            else if (line[2] != '-' && line[2] != '(') inPassing = inSync = false;
            continue;
        }
        char* close = line[0] == '"' ? strchr(line + 1, '"') : NULL;
        char* end = close ? strchr(close, ';') : NULL;
        if ((!inPassing && !inSync) || !end) continue;
        *end = '\0';
        char* site = close + 1;
        while (*site == ' ') site++;
        CHECK(JSValidator::validateJSPattern(site), "library pattern %s is valid", site);
        CHECK(inSync || JSValidator::jugglerCount(site) > 1, "library pattern %s has several jugglers", site);
        if (JSValidator::validateJSPattern(site)) {
            // library patterns may touch in the air and stretch as far as the
            // solo 5c does; the fixed test patterns above keep the strict limits
            testContinuity(site, inSync ? 0.25f : 0.15f);
            testReach(site, inSync ? 0.25f : 0.15f);
            if (inPassing) testSeparation(site, 0.6f);
        }
        count++;
    }
    fclose(f);
    CHECK(count > 0, "the library has passing patterns");
}

// Feeds stand 6 to 8.5 units apart (about 1.3 m to 1.8 m) and keep their clubs apart at every width
static void testFeedWidths() {
    const char* feed = "<3p2c 3p3c 3p4c|3p1c 3c 3c|3c 3p1c 3c|3c 3c 3p1c>";
    CHECK(GetFeedWidth() == FEED_WIDTH_DEFAULT && fabsf(FeedWidthMetres(FEED_WIDTH_DEFAULT) - 1.58f) < 0.01f,
          "feeds start about 1.6 m apart");
    for (int width = 0; width < FEED_WIDTHS; width++) {
        SetFeedWidth(width);
        Pattern p(feed);
        CHECK(p.info.Formation == FORMATION_FEED, "the feed formation is used");
        float dx = p.info.Placement[1].x - p.info.Placement[2].x;
        float dz = p.info.Placement[1].z - p.info.Placement[2].z;
        float gap = sqrtf(dx * dx + dz * dz);
        CHECK(fabsf(gap - (6.0f + 0.5f * width)) < 0.01f, "width %d: neighbours %g apart", width, gap);
        CHECK(fabsf(FeedWidthMetres(width) - gap * 0.4f / 1.9f) < 0.01f, "width %d in metres", width);
        testSeparation(feed, 0.6f);
        testSeparation("<3p2c 3p3c|3p1c 3c|3c 3p1c>", 0.6f);
    }
    SetFeedWidth(-1);
    CHECK(GetFeedWidth() == 0, "widths below the narrowest clamp");
    SetFeedWidth(99);
    CHECK(GetFeedWidth() == FEED_WIDTHS - 1, "widths above the widest clamp");
    SetFeedWidth(FEED_WIDTH_DEFAULT);
}

int main(int argc, char** argv) {
    testValidator();
    if (argc > 1) testLibrary(argv[1]);
    testFeedWidths();
    testNotations();
    testGivenPositions();
    testStructure();
    const char* sites[] = {"J2:966", "J2:9c6c6c", "J2:7", "J2:7c", "J2:6", "J2:86277", "J2:786c", "J2:975",
                           "J3:8", "J3:8c", "4.5p 3 3", "4.5pc 3c 3c", "3.5pc", "3.67p 3",
                           "J2:5", "J2:9", "J2:3", "J3:4",
                           "4p 3", "<3p|3p>", "<3p 3 3 3|3p 3 3 3>", "<3p2 3p3|3p1 3|3 3p1>",
                           "(4c,4c)", "(4c,2cx)(2cx,4c)"};
    for (const char* s : sites) {
        testContinuity(s);
        testReach(s);
        if (JSValidator::jugglerCount(s) > 1) testSeparation(s, strcmp(s, "J2:9") == 0 ? 0.6f : 0.68f);
    }
    // Given positions: the pattern's own (colliding) layout, checked for movement only
    testContinuity("J2:6c@(0,0)>(0,0)/(5,0)");
    testReach("J2:6c@(0,0)>(0,0)/(5,0)");
    printf("%s (%d failures)\n", failures == 0 ? "passing tests OK" : "passing tests FAILED", failures);
    return failures == 0 ? 0 : 1;
}
