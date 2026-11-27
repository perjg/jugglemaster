/* Juggler3D, Copyright (c) 2005-2008 Brian Apps <brian@jugglesaver.co.uk>
 *
 * JMLib - Portable JuggleMaster Library
 * Version 3.0
 * Copyright (C) Per Johan Groland 2025-2026
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.  No
 * representations are made about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

#include "jugglesaver.h"
#include "js_passing.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 *
 * Engine
 *
 ****************************************************************************
 *
 * The main purpose of the engine is to work out the exact position of all the
 * juggling objects and the juggler's hands at any point in time.  The motion
 * of the objects can be split into two parts: in the air and and being carried.
 *
 * While in the air, the motion is governed by a standard parabolic trajectory.
 * The only minor complication is that the engine has no fixed concept of
 * gravity, instead it using a term called Alpha that varies according to the
 * pattern (see SetHeightAndAlpha).
 *
 * The motion while an object is carried comes from fitting a spline through the
 * catch and throw points and maintaining the catch and throw velocities at
 * each end.  In the simplest case this boils down to cubic Bezier spline.  The
 * only wrinkle occurs when a ball is being carried for a long time.  The simple
 * cubic spline maths produces a curve that goes miles away -- here we do a
 * bit of reparameterisation so things stay within sensible bounds.
 * (On a related note, this scheme is _much_ simpler than the Juggle Saver
 * one.  Juggle Saver achieves 2nd order continuity and much care is taken
 * to avoid spline ringing.)
 *
 * The motion of the hands is identical to the ball carrying code. It uses two
 * splines: one while an object is being carried; and another when it moves from
 * the previous throw to the next catch.
 */

/* While a ball is thrown it twists slighty about an axis, this routine gives
 * the total about of twist for a given ball throw. */

float GetBallTwistAmount(const PATTERN_INFO* pPattern, const THROW_INFO* pThrow)
{
    if (pPattern->Jugglers > 1)
        return pThrow->TwistSign * 18.0f *
            powf((float) pThrow->TimeInAir / pPattern->Jugglers, 1.5);

    if (pThrow->FromPos.x > pThrow->ToPos.x)
        return 18.0f * powf(pThrow->TimeInAir, 1.5);
    else
        return -18.0f * powf(pThrow->TimeInAir, 1.5);
}


float NormaliseAngle(float Ang)
{
    if (Ang >= 0.0f)
    {
        int i = (int) (Ang + 180.0f) / 360;
        return Ang - 360.0f * i;
    }
    else
    {
        int i = (int)(180.0f - Ang) / 360;
        return Ang + i * 360.0f;
    }
}


/* The interpolate routine for ball carrying and hand motion.  We are given the
 * start (P0) and end (P1) points and the velocities at these points, the task
 * is to form a function P(t) such that:
 *    P(0) = P0
 *    P(TLen) = P1
 *    P'(0) = V0
 *    P'(TLen) = V1
 */

POS InterpolatePosition(
    const POS* pP0, const POS* pV0, const POS* pP1, const POS* pV1,
    float CarryTime, float TLen, float t)
{
    POS p;
    float a, b, c, d, tt, tc;

    /* The interpolation is based on a simple cubic that achieves 1st order
     * continuity at the end points.  However the spline can become too long if
     * the TLen parameter is large.  In this case we cap the curve's length (fix
     * the shape) and then reparameterise time to achieve the continuity
     * conditions. */

    tc = CarryTime;

    if (TLen > tc)
    {
        /* The reparameterisation tt(t) gives:
         *  tt(0) = 0, tt(TLen) = tc, tt'(0) = 1, tt'(TLen) = 1
         * and means we can set t = tt(t), TLen = tc and then fall through
         * to use the normal cubic spline fit.
         *
         * The reparameterisation is based on two piecewise quadratics, one
         * that goes from t = 0 to t = TLen / 2 and the other, mirrored in
         * tt and t that goes from t = TLen / 2 to t = TLen.
         * Because TLen > tc we can arrange for tt to be unique in the range if
         * we specify the quadratic in tt.  i.e. t = A * tt ^ 2 + B * tt + C.
         *
         * Considering the first piece and applying initial conditions.
         *   tt = 0 when t = 0   =>  C = 0
         *   tt' = 1 when t = 0   =>  B = 1
         *   tt = tc / 2 when t = TLen / 2  =>  A = 2 * (TLen - tc) / tc^2
         *
         * writing in terms of t
         *   tt = (-B + (B ^ 2 + 4At) ^ 0.5) / 2A
         * or
         *   tt = ((1 + 4At) ^ 0.5 - 1) / 2A */

        float A = 2.0f * (TLen - tc) / (tc * tc);

        if (t > TLen / 2.0f)
            t = tc - (sqrtf(1.0f + 4.0f * A * (TLen - t)) - 1.0f) / (2.0f * A);
        else
            t = (sqrtf(1.0f + 4.0f * A * t) - 1.0f) / (2.0f * A);

        TLen = tc;
    }

    /* The cubic spline takes the form:
     *   P(t) = p0 * a(t) + v0 * b(t) + p1 * c(t) + v1 * d(t)
     * where p0 is the start point, v0 the start velocity, p1 the end point and
     * v1 the end velocity.  a(t), b(t), c(t) and d(t) are cubics in t.
     * We can show that:
     *
     *  a(t) = 2 * (t / TLen) ^ 3 - 3 * (t / TLen) ^ 2 + 1
     *  b(t) = t ^ 3 / TLen ^ 2 - 2 * t ^ 2 / TLen + t
     *  c(t) = -2 * (t / TLen) ^ 3 + 3 * (t / TLen) ^ 2
     *  d(t) = t ^ 3 / TLen ^ 2 - t ^ 2 / TLen
     *
     * statisfy the boundary conditions:
     *    P(0) = p0, P(TLen) = p1, P'(0) = v0 and P'(TLen) = v1  */

    tt = t / TLen;

    a = tt * tt * (2.0f * tt - 3.0f) + 1.0f;
    b = t * tt * (tt - 2.0f) + t;
    c = tt * tt * (3.0f - 2.0f * tt);
    d = t * tt * (tt - 1.0f);

    p.x = a * pP0->x + b * pV0->x + c * pP1->x + d * pV1->x;
    p.y = a * pP0->y + b * pV0->y + c * pP1->y + d * pV1->y;
    p.z = a * pP0->z + b * pV0->z + c * pP1->z + d * pV1->z;

    p.Rot = a * NormaliseAngle(pP0->Rot) + b * pV0->Rot +
        c * NormaliseAngle(pP1->Rot) + d * pV1->Rot;
    p.Elev = a * NormaliseAngle(pP0->Elev) + b * pV0->Elev +
        c * NormaliseAngle(pP1->Elev) + d * pV1->Elev;

    return p;
}


/* How long the object thrown by pThrow is carried.  Adding the whole beats
 * last keeps one juggler's timing exactly as it always was; with several
 * jugglers the carry must come out exactly CarryTime for a throw without
 * holds, or InterpolatePosition's reparameterisation degenerates. */
static float CarryLength(const PATTERN_INFO* pPattern, const THROW_INFO* pThrow)
{
    if (pPattern->Jugglers == 1)
        return pPattern->CarryTime + pThrow->TotalTime - pThrow->TimeInAir;
    return pPattern->CarryTime + (pThrow->TotalTime - pThrow->TimeInAir);
}

POS InterpolateCarry(const PATTERN_INFO* pPattern,
    const THROW_INFO* pThrow, const THROW_INFO* pNext, float t)
{
    float CT = CarryLength(pPattern, pThrow);
    return InterpolatePosition(&pThrow->ToPos, &pThrow->ToVelocity,
        &pNext->FromPos, &pNext->FromVelocity, pPattern->CarryTime, CT, t);
}


/* How many slots after its index the throw from Slot is made (a time shift
 * of the hand that throws it). */
static int SlotShift(const PATTERN_INFO* pPattern, int Slot)
{
    int Hands = 2 * pPattern->Jugglers;
    return pPattern->HandShift[((Slot % Hands) + Hands) % Hands];
}

/* How long the throw from Slot of the given weight is in the air plus
 * carried: the slots between throw and catch, adjusted for the hands' shifts. */
static int FlightSlots(const PATTERN_INFO* pPattern, int Slot, int Weight)
{
    return Weight + SlotShift(pPattern, Slot + Weight) - SlotShift(pPattern, Slot);
}


/* Determine the position of the hand at a point in time. */

void GetHandPosition(
    PATTERN_INFO* pPattern, int RightHand, float Time, POS* pPos)
{
    GetHandPositionEx(pPattern, RightHand == 0 ? 0 : 1, Time, pPos);
}


/* Hand is the hand index (slot mod 2J); the position is in world space. */

void GetHandPositionEx(
    PATTERN_INFO* pPattern, int Hand, float Time, POS* pPos)
{
    OBJECT_POSITION* pObj = &pPattern->Hands[Hand];
    int Hold = 2 * pPattern->Jugglers;
    int Shift = pPattern->HandShift[Hand];
    THROW_INFO* pLastThrow;

    /* Upon entry, the throw information for the relevant hand may be out of
     * sync.  Therefore we advance through the pattern if required. */

    while (pPattern->pThrowInfo[pObj->ThrowIndex].NextForHand + pObj->TimeOffset + Shift
        <= (int) Time)
    {
        int w = pPattern->pThrowInfo[pObj->ThrowIndex].NextForHand;
        pObj->TimeOffset += w;
        pObj->ThrowIndex = (pObj->ThrowIndex + w) % pPattern->ThrowLen;
    }

    pLastThrow = &pPattern->pThrowInfo[pObj->ThrowIndex];

    /* The TimeInAir will only ever be a hold (2 for one juggler) or 0 if no
     * object is ever thrown by this hand.  In normal circumstances, holds in
     * the site swap are coalesced and added to TotalTime of the previous throw.
     * 0 is a hole and means that an object isn't there.  In this case we just
     * hold the hand still. */
    if (pLastThrow->TimeInAir == Hold || pLastThrow->TimeInAir == 0)
    {
        pPos->x = pLastThrow->FromPos.x;
        pPos->y = pLastThrow->FromPos.y;
        pPos->z = pLastThrow->FromPos.z;
    }
    else
    {
        /* The hand is either moving to catch the next object or carrying the
         * next object to its next throw position.  The way THROW_INFO is
         * structured means the relevant information for the object we're going
         * to catch is held at the point at which it was thrown
         * (pNextThrownFrom).  We can't go straight for it and instead have to
         * look at the object we've about to throw next and work out where it
         * came from. */

        THROW_INFO* pNextThrow = &pPattern->pThrowInfo[
            (pObj->ThrowIndex + pLastThrow->NextForHand) % pPattern->ThrowLen];

        THROW_INFO* pNextThrownFrom =
            &pPattern->pThrowInfo[pNextThrow->PrevThrow];

        /* tc is a measure of how long the object we're due to catch is being
         * carried for.  We use this to work out if we've actually caught it at
         * this moment in time. */

        float tc = CarryLength(pPattern, pNextThrownFrom);

        Time -= pObj->TimeOffset + Shift;

        if (Time > pLastThrow->NextForHand - tc)
        {
            /* carrying this ball to it's new location */
            *pPos = InterpolateCarry(pPattern, pNextThrownFrom, pNextThrow,
                (Time - (pLastThrow->NextForHand - tc)));
        }
        else
        {
            /* going for next catch */
            *pPos = InterpolatePosition(
                &pLastThrow->FromPos, &pLastThrow->FromVelocity,
                &pNextThrownFrom->ToPos, &pNextThrownFrom->ToVelocity,
                pPattern->CarryTime, pLastThrow->NextForHand - tc, Time);
        }
    }
}


float SinDeg(float AngInDegrees)
{
    return sinf(AngInDegrees * PI / 180.0f);
}


float CosDeg(float AngInDegrees)
{
    return cosf(AngInDegrees * PI / 180.0f);
}


/* Offset the specified position to get the centre of the object based on the
 * the handle length and the current orientation */

void OffsetHandlePosition(const POS* pPos, float HandleLen, POS* pResult)
{
    pResult->x = pPos->x + HandleLen * SinDeg(pPos->Rot) * CosDeg(pPos->Elev);
    pResult->y = pPos->y + HandleLen * SinDeg(pPos->Elev);
    pResult->z = pPos->z + HandleLen * CosDeg(pPos->Rot) * CosDeg(pPos->Elev);
    pResult->Elev = pPos->Elev;
    pResult->Rot = pPos->Rot;
}


void GetObjectPosition(
    PATTERN_INFO* pPattern, int Obj, float Time, float HandleLen, POS* pPos)
{
    OBJECT_POSITION* pObj = &pPattern->pObjectInfo[Obj];
    THROW_INFO* pThrow;

    /* Move through the pattern, if required, such that pThrow corresponds to
     * the current throw for this object. */

    while (pPattern->pThrowInfo[pObj->ThrowIndex].TotalTime + pObj->TimeOffset +
        SlotShift(pPattern, pObj->TimeOffset + pPattern->pThrowInfo[pObj->ThrowIndex].TotalTime)
        <= (int) Time)
    {
        int w = pPattern->pThrowInfo[pObj->ThrowIndex].TotalTime;
        pObj->TimeOffset += w;
        pObj->TotalTwist = NormaliseAngle(pObj->TotalTwist +
            GetBallTwistAmount(pPattern, &pPattern->pThrowInfo[pObj->ThrowIndex]));

        pObj->ThrowIndex = (pObj->ThrowIndex + w) % pPattern->ThrowLen;
    }

    pThrow = &pPattern->pThrowInfo[pObj->ThrowIndex];

    if (pThrow->TimeInAir == 2 * pPattern->Jugglers || pThrow->TimeInAir == 0)
    {
        *pPos = pThrow->FromPos;
        OffsetHandlePosition(pPos, HandleLen, pPos);
    }
    else
    {
        float tc = FlightSlots(pPattern, pObj->TimeOffset, pThrow->TimeInAir) - pPattern->CarryTime;
        float BallTwist = GetBallTwistAmount(pPattern, pThrow);
        Time -= pObj->TimeOffset + SlotShift(pPattern, pObj->TimeOffset);
        if (Time < tc)
        {
            /* object in air */
            POS From, To;
            float t, b;

            t = Time / tc;

            OffsetHandlePosition(&pThrow->FromPos, HandleLen, &From);
            OffsetHandlePosition(&pThrow->ToPos, HandleLen, &To);

            b = (To.y - From.y) / tc + pPattern->Alpha * tc;

            pPos->x = (1.0f - t) * From.x + t * To.x;
            pPos->z = (1.0f - t) * From.z + t * To.z;
            pPos->y = -pPattern->Alpha * Time * Time + b * Time + From.y;

            if (pObj->ObjectType == OBJECT_BALL)
                pPos->Rot = pObj->TotalTwist + t * BallTwist;
            else
            {
                /* We describe the rotation of a club (or ring) with an
                 * elevation and rotation but don't include a twist.
                 * If we ignore twist for the moment, the orientation at a
                 * rotation of r and an elevation of e can be also be expressed
                 * by rotating the object a further 180 degrees and sort of
                 * mirroring the rotation, e.g.:
                 *    rot = r + 180 and elev = 180 - e
                 * We can easily show that the maths holds, consider the
                 * x, y ,z position of the end of a unit length club.
                 *    y = sin(180 - e) = sin(e)
                 *    x = cos(180 - e) * sin(r + 180) = -cos(e) * - sin(r)
                 *    z = cos(180 - e) * cos(r + 180) = -cos(e) * - cos(r)
                 * When a club is thrown these two potential interpretations
                 * can produce unexpected results.
                 * The approach we adopt is that we try and minimise the amount
                 * of rotation we give a club -- normally this is what happens
                 * when juggling since it's much easier to spin the club.
                 *
                 * When we come to drawing the object the two interpretations
                 * aren't identical, one causes the object to twist a further
                 * 180 about its axis.  We avoid the issue by ensuring our
                 * objects have rotational symmetry of order 2 (e.g. we make
                 * sure clubs have an even number of stripes) this makes the two
                 * interpretations appear identical. */

                float RotAmt = NormaliseAngle(To.Rot - From.Rot);

                if (RotAmt < -90.0f)
                {
                    To.Elev += 180  - 2 * NormaliseAngle(To.Elev);
                    RotAmt += 180.0f;
                }
                else if (RotAmt > 90.0f)
                {
                    To.Elev += 180 - 2 * NormaliseAngle(To.Elev);
                    RotAmt -= 180.0f;
                }

                pPos->Rot = From.Rot + t * RotAmt;
            }

            pPos->Elev = (1.0f - t) * From.Elev + t * To.Elev;

        }
        else
        {
            THROW_INFO* pNextThrow = &pPattern->pThrowInfo[
                   (pObj->ThrowIndex + pThrow->TotalTime) % pPattern->ThrowLen];

            *pPos = InterpolateCarry(pPattern, pThrow, pNextThrow, Time - tc);

            if (pObj->ObjectType == OBJECT_BALL)
                pPos->Rot = pObj->TotalTwist + BallTwist;

            OffsetHandlePosition(pPos, HandleLen, pPos);
        }
    }
}


/* Alpha is used to represent the acceleration due to gravity (in fact
 * 2 * Alpha is the acceleration).  Alpha is adjusted according to the pattern
 * being juggled.  My preference is to slow down patterns with lots of objects
 * -- they move too fast in realtime.  Also I prefer to see a balance between
 * the size of the figure and the height of objects thrown -- juggling patterns
 * with large numbers of objects under real gravity can mean balls are lobbed
 * severe heights.  Adjusting Alpha achieves both these goals.
 *
 * Basically we pick a height we'd like to see the biggest throw reach and then
 * adjust Alpha to meet this. */

void SetHeightAndAlpha(PATTERN_INFO* pPattern,
    const int* Site, const EXT_SITE_INFO* pExtInfo, int Len, int Jugglers)
{
    float H;
    int MaxW = 5;
    int i;

    if (Jugglers > 1)
    {
        /* The same heights as a solo juggler throwing w / J beats; Alpha is
         * per slot, J slots to a beat. */
        float SoloW = 5.0f;
        float Alpha;
        for (i = 0; i < Len; i++)
            SoloW = max(SoloW, (float) (Site != NULL ? Site[i] : pExtInfo[i].Weight) / Jugglers);

        H = 8.0f * powf(SoloW / 2.0f, 0.8f) + 5.0f;
        Alpha = (2.0f * H) / powf(SoloW - CARRY_TIME, 2.0f);
        pPattern->Height = Alpha * powf((SoloW - CARRY_TIME) * 0.5f, 2);
        pPattern->Alpha = Alpha / (float) (Jugglers * Jugglers);
        return;
    }

    if (Site != NULL)
    {
        for (i = 0; i < Len; i++)
            MaxW = max(MaxW, Site[i]);
    }
    else
    {
        for (i = 0; i < Len; i++)
            MaxW = max(MaxW, pExtInfo[i].Weight);
    }

    /* H is the ideal max height we'd like our objects to reach.  The formula
     * was developed by trial and error and was simply stolen from Juggle Saver.
     * Alpha is then calculated from the classic displacement formula:
     *   s = 0.5at^2 + ut  (where a = 2 * Alpha)
     * We know u (the velocity) is zero at the peak, and the object should fall
     * H units in half the time of biggest throw weight.
     * Finally we determine the proper height the max throw reaches since this
     * may not be H because capping may be applied (e.g. for max weights less
     * than 5). */

    H = 8.0f * powf(MaxW / 2.0f, 0.8f) + 5.0f;
    pPattern->Alpha = (2.0f * H) / powf(max(5, MaxW) - CARRY_TIME, 2.0f);
    pPattern->Height = pPattern->Alpha * powf((MaxW - CARRY_TIME) * 0.5f, 2);
}


/* Where positions and spin info is not specified, generate suitable default
 * values. */

int GetDefaultSpins(int Weight)
{
    if (Weight < 3)
        return 0;
    else if (Weight < 4)
        return 1;
    else if (Weight < 7)
        return 2;
    else
        return 3;
}


void GetDefaultFromPosition(unsigned char Side, int Weight, POS* pPos)
{
    if (Weight > 4 && Weight % 2 != 0)
        pPos->x = Side ?  -0.06f : 0.06f;
    else if (Weight == 0 || Weight == 2)
        pPos->x = Side ? 1.6f :  -1.6f;
    else
        pPos->x = Side? 0.24f :  -0.24f;

    pPos->y = (Weight == 2 || Weight == 0) ? -0.25f : 0.0f;

    pPos->Rot = (Weight % 2 == 0 ? -23.5f : 27.0f) * (Side ? -1.0f : 1.0f);

    pPos->Elev = Weight == 1 ? -30.0f : 0.0f;
    pPos->z = 0.0f;
}


void GetDefaultToPosition(unsigned char Side, int Weight, POS* pPos)
{
    if (Weight == 1)
        pPos->x = Side ?  -1.0f : 1.0f;
    else if (Weight % 2 == 0)
        pPos->x = Side ? 2.8f :  -2.8f;
    else
        pPos->x = Side?  -3.1f : 3.1f;

    pPos->y = -0.5f;

    pPos->Rot = (Side ? -35.0f : 35.0f) * (Weight % 2 == 0 ? -1.0f : 1.0f);

    if (Weight < 2)
        pPos->Elev = -30.0f;

    else if (Weight < 4)
        pPos->Elev = 360.0f - 50.0f;
    else if (Weight < 7)
        pPos->Elev = 720.0f - 50.0f;
    else
        pPos->Elev = 360.0f * GetDefaultSpins(Weight) - 50.0f;
    pPos->z = 0.0f;
}


/* Passing defaults, in the thrower's and the catcher's own frame: passes leave
 * from in front of the shoulder and are caught slightly outside it. */

#define PASS_DISTANCE 9.0f       /* between the hand planes of facing jugglers */
#define PASS_CATCH_DAMPING 0.3f  /* share of a pass's horizontal speed the hands follow */
#define PASS_KEEP_RIGHT 0.4f     /* passes keep to the thrower's right, so passes in
                                  * opposite directions along one lane miss */

static int SlotSide(int Slot, int Jugglers)
{
    return (Slot % (2 * Jugglers)) / Jugglers;
}

static int SlotJuggler(int Slot, int Jugglers)
{
    return (Slot % (2 * Jugglers)) % Jugglers;
}

int GetDefaultPassSpins(int Weight, int Jugglers)
{
    int Beats = Weight / Jugglers;
    if (Beats < 4)
        return 1;
    else if (Beats < 5)
        return 2;
    else
        return 3;
}

static int GetDefaultThrowSpins(int Weight, int Jugglers)
{
    return Weight % Jugglers == 0 ? GetDefaultSpins(Weight / Jugglers)
                                  : GetDefaultPassSpins(Weight, Jugglers);
}

/* Default throw (in the thrower's frame) and catch (in the catcher's frame)
 * positions for the throw at Slot, which stays Flight slots in the air.  With
 * one juggler these are the classic Juggle Saver defaults. */
static void GetDefaultThrowPositions(int Slot, int Weight, int Flight, int Jugglers, POS* pFrom, POS* pTo)
{
    int Side = SlotSide(Slot, Jugglers);

    if (Weight % Jugglers == 0)
    {
        /* A self looks like a solo throw as long as its flight, odd when it
         * crosses: with shifted hands (sync) a crossing 4x throws like a 5
         * from either hand.  Without shifts this is Weight / Jugglers. */
        int Crossing = SlotSide(Slot + Weight, Jugglers) != Side;
        int Solo = Flight / Jugglers;
        if (Solo > 0 && (Solo % 2 == 1) != Crossing)
            Solo++;
        GetDefaultFromPosition(Side, Solo, pFrom);
        GetDefaultToPosition(Side, Solo, pTo);
        return;
    }

    /* A straight pass (right hand to the partner's left, say) flies down the
     * lane on that side and is caught high; a crossing pass goes diagonally
     * through the middle and is caught low.  Tuned so that the passes of the
     * library patterns keep apart in the air (tests/engine/passing_test.cpp). */
    int CatchSide = SlotSide(Slot + Weight, Jugglers);
    int Crossing = CatchSide == Side;

    pFrom->x = (Side ? 1.0f : -1.0f) * (Crossing ? 0.4f : 1.2f);
    pFrom->y = 0.0f;
    pFrom->z = 0.3f;
    pFrom->Rot = 0.0f;
    pFrom->Elev = 0.0f;

    pTo->x = (CatchSide ? 1.0f : -1.0f) * (Crossing ? 0.4f : 1.4f);
    pTo->y = Crossing ? -0.7f : 0.6f;
    pTo->z = 0.3f;
    pTo->Rot = 0.0f;
    pTo->Elev = 360.0f * GetDefaultPassSpins(Flight, Jugglers) - 50.0f;
}

/* Between neighbours on the arc of a feed, from 6 to 8.5 units in steps of
 * 0.5.  A shoulder width (1.9 units) is about 0.4 m, so a unit is about 0.21 m
 * and the widths run from about 1.3 m to 1.8 m. */
#define FEED_SPACING_NARROWEST 6.0f
#define FEED_SPACING_STEP 0.5f
#define METRES_PER_UNIT (0.4f / 1.9f)

static int FeedWidth = FEED_WIDTH_DEFAULT;
#define FeedSpacing (FEED_SPACING_NARROWEST + FEED_SPACING_STEP * FeedWidth)

void SetFeedWidth(int Width)
{
    FeedWidth = Width < 0 ? 0 : Width >= FEED_WIDTHS ? FEED_WIDTHS - 1 : Width;
}

int GetFeedWidth(void)
{
    return FeedWidth;
}

float FeedWidthMetres(int Width)
{
    return (FEED_SPACING_NARROWEST + FEED_SPACING_STEP * Width) * METRES_PER_UNIT;
}

static void ApplyFormation(PATTERN_INFO* pPattern, const PASSING_LAYOUT* pLayout)
{
    int J = pPattern->Jugglers;
    int j;

    memset(pPattern->Placement, 0, sizeof(pPattern->Placement));
    if (pLayout->Formation == FORMATION_FEED && J > 2)
    {
        /* the others stand on an arc PASS_DISTANCE from the feeder, each
         * facing the feeder, FeedSpacing apart; the whole feed is turned a
         * little so the default camera does not see the arc end on */
        const float Turn = 35.0f;
        float Step = 2.0f * asinf(FeedSpacing / (2.0f * PASS_DISTANCE)) * 180.0f / PI;
        for (j = 0; j < J; j++)
        {
            int InLine = j < pLayout->Feeder ? j : j - 1;
            float Ang = Step * (InLine - (J - 2) / 2.0f);
            float x = -PASS_DISTANCE / 2.0f, z = 0.0f, Facing = 90.0f;
            if (j != pLayout->Feeder)
            {
                x += PASS_DISTANCE * CosDeg(Ang);
                z = PASS_DISTANCE * SinDeg(Ang);
                Facing = -90.0f - Ang;
            }
            pPattern->Placement[j].x = x * CosDeg(Turn) + z * SinDeg(Turn);
            pPattern->Placement[j].z = -x * SinDeg(Turn) + z * CosDeg(Turn);
            pPattern->Placement[j].Facing = Facing + Turn;
        }
    }
    else if (J == 2)
    {
        /* face each other across x, so the default camera sees both in profile */
        pPattern->Placement[0].x = -PASS_DISTANCE / 2.0f;
        pPattern->Placement[0].Facing = 90.0f;
        pPattern->Placement[1].x = PASS_DISTANCE / 2.0f;
        pPattern->Placement[1].Facing = -90.0f;
    }
    else if (J > 2)
    {
        /* a circle facing its centre with neighbours PASS_DISTANCE apart;
         * juggler 0 stands at the back facing the camera */
        float R = PASS_DISTANCE / (2.0f * sinf(PI / J));
        for (j = 0; j < J; j++)
        {
            float Ang = 180.0f + 360.0f * j / J;
            pPattern->Placement[j].x = R * SinDeg(Ang);
            pPattern->Placement[j].z = R * CosDeg(Ang);
            pPattern->Placement[j].Facing = Ang + 180.0f;
        }
    }
}

static void JugglerToWorld(const PATTERN_INFO* pPattern, int Juggler, POS* pPos)
{
    const JUGGLER_PLACEMENT* pPlace = &pPattern->Placement[Juggler];
    float s = SinDeg(pPlace->Facing);
    float c = CosDeg(pPlace->Facing);
    float x = pPos->x;
    float z = pPos->z;

    pPos->x = pPlace->x + x * c + z * s;
    pPos->z = pPlace->z - x * s + z * c;
    pPos->Rot += pPlace->Facing;
}

void WorldToJuggler(const PATTERN_INFO* pPattern, int Juggler, const POS* pWorld, POS* pLocal)
{
    const JUGGLER_PLACEMENT* pPlace = &pPattern->Placement[Juggler];
    float s = SinDeg(pPlace->Facing);
    float c = CosDeg(pPlace->Facing);
    float dx = pWorld->x - pPlace->x;
    float dz = pWorld->z - pPlace->z;

    *pLocal = *pWorld;
    pLocal->x = dx * c - dz * s;
    pLocal->z = dx * s + dz * c;
    pLocal->Rot = pWorld->Rot - pPlace->Facing;
}

int JugglerHand(const PATTERN_INFO* pPattern, int Juggler, int Side)
{
    return Juggler + pPattern->Jugglers * Side;
}

/* Overrides the fields of a default position that the pattern gave. */
static void ApplyGivenFields(POS* pPos, const POS* pGiven, unsigned char Fields)
{
    if (Fields & 1) pPos->x = pGiven->x;
    if (Fields & 2) pPos->y = pGiven->y;
    if (Fields & 4) pPos->Rot = pGiven->Rot;
    if (Fields & 8) pPos->Elev = pGiven->Elev;
}

/* First-pass setup of one throw when there are several jugglers: positions
 * from the defaults or the pattern, in the thrower's and catcher's frames,
 * then moved into world space before the velocities are worked out. */
static void InitPassingThrow(PATTERN_INFO* pPattern, int Slot, int Weight,
    const EXT_SITE_INFO* pExt, float Spread)
{
    THROW_INFO* pThrow = &pPattern->pThrowInfo[Slot];
    int J = pPattern->Jugglers;
    int Thrower = SlotJuggler(Slot, J);
    int Catcher = SlotJuggler(Slot + Weight, J);
    POS From, To, LocalTo;
    float t1, RotAmt, FlightElev;
    int Flight = FlightSlots(pPattern, Slot, Weight);

    GetDefaultThrowPositions(Slot, Weight, Flight, J, &From, &To);
    if (pExt != NULL && (pExt->Flags & HAS_FROM_POS) != 0)
        ApplyGivenFields(&From, &pExt->FromPos, pExt->FromFields);
    if (pExt != NULL && (pExt->Flags & HAS_TO_POS) != 0)
        ApplyGivenFields(&To, &pExt->ToPos, pExt->ToFields);
    if (pExt != NULL && (pExt->Flags & HAS_SPINS) != 0)
        To.Elev = 360.0f * pExt->Spins + NormaliseAngle(To.Elev);
    To.x += To.x < 0.0f ? -Spread : Spread;

    pThrow->IsPass = Weight % J != 0;

    /* Twist the way the solo rule would, seen from the thrower */
    LocalTo = To;
    JugglerToWorld(pPattern, Catcher, &LocalTo);
    WorldToJuggler(pPattern, Thrower, &LocalTo, &LocalTo);
    pThrow->TwistSign = From.x > LocalTo.x ? 1.0f : -1.0f;

    JugglerToWorld(pPattern, Thrower, &From);
    JugglerToWorld(pPattern, Catcher, &To);
    if (pThrow->IsPass && SlotSide(Slot, J) != SlotSide(Slot + Weight, J))
    {
        /* the thrower's right is -x in its own frame */
        float Facing = pPattern->Placement[Thrower].Facing;
        float RightX = -CosDeg(Facing) * PASS_KEEP_RIGHT;
        float RightZ = SinDeg(Facing) * PASS_KEEP_RIGHT;
        From.x += RightX;
        From.z += RightZ;
        To.x += RightX;
        To.z += RightZ;
    }
    pThrow->FromPos = From;
    pThrow->ToPos = To;

    if (Weight == 0 || Weight == 2 * J)
        return;

    /* See SetHeightAndAlpha and InitPatternInfo for the parabola */
    t1 = Flight - pPattern->CarryTime;
    pThrow->FromVelocity.y = pPattern->Alpha * t1 + (To.y - From.y) / t1;
    pThrow->ToVelocity.y = pThrow->FromVelocity.y - 2.0f * pPattern->Alpha * t1;
    pThrow->FromVelocity.x = pThrow->ToVelocity.x = (To.x - From.x) / t1;
    pThrow->FromVelocity.z = pThrow->ToVelocity.z = (To.z - From.z) / t1;

    /* Spin rates of the flight GetObjectPosition draws, which turns the club
     * the short way round; a pass ends with the catch absorbing the spin. */
    RotAmt = NormaliseAngle(To.Rot - From.Rot);
    FlightElev = To.Elev;
    if (RotAmt < -90.0f || RotAmt > 90.0f)
    {
        FlightElev += 180.0f - 2.0f * NormaliseAngle(To.Elev);
        RotAmt += RotAmt < 0.0f ? 180.0f : -180.0f;
    }
    pThrow->FromVelocity.Rot = RotAmt / t1;
    pThrow->FromVelocity.Elev = (FlightElev - From.Elev) / t1;

    if (pThrow->IsPass)
    {
        pThrow->FromVelocity.x *= PASS_CATCH_DAMPING;
        pThrow->FromVelocity.z *= PASS_CATCH_DAMPING;
        pThrow->ToVelocity.x *= PASS_CATCH_DAMPING;
        pThrow->ToVelocity.z *= PASS_CATCH_DAMPING;
        pThrow->ToVelocity.Rot = 0.0f;
        pThrow->ToVelocity.Elev = 0.0f;
    }
    else
    {
        pThrow->ToVelocity.Rot = pThrow->FromVelocity.Rot;
        pThrow->ToVelocity.Elev = pThrow->FromVelocity.Elev;
    }

    if (pExt != NULL && (pExt->Flags & HAS_SNATCH) != 0)
    {
        /* the snatch is sideways in the catcher's frame */
        float Facing = pPattern->Placement[Catcher].Facing;
        pThrow->ToVelocity.x = pExt->SnatchX * CosDeg(Facing);
        pThrow->ToVelocity.z = -pExt->SnatchX * SinDeg(Facing);
        pThrow->ToVelocity.y = pExt->SnatchY;
    }
}

/* The closest two objects in flight get over one period of the pattern, and
 * the slots they were thrown from.  Leaves the pattern's animation state as it
 * found it. */
static float ClosestInFlight(PATTERN_INFO* pPattern, int* pSlotA, int* pSlotB)
{
    const int Steps = 16;
    size_t Size = sizeof(OBJECT_POSITION) * pPattern->Objects;
    OBJECT_POSITION* pSaved = (OBJECT_POSITION*) malloc(Size);
    POS* pPos = (POS*) malloc(sizeof(POS) * pPattern->Objects);
    unsigned char* pAir = (unsigned char*) malloc(pPattern->Objects);
    float Best = 1e9f;
    int k, i, j;

    memcpy(pSaved, pPattern->pObjectInfo, Size);
    for (k = 0; k < pPattern->ThrowLen * Steps; k++)
    {
        float Time = (float) pPattern->MaxWeight + (float) k / Steps;
        for (i = 0; i < pPattern->Objects; i++)
        {
            const OBJECT_POSITION* pObj = &pPattern->pObjectInfo[i];
            const THROW_INFO* pThrow;
            GetObjectPosition(pPattern, i, Time, 0.0f, &pPos[i]);
            pThrow = &pPattern->pThrowInfo[pObj->ThrowIndex];
            pAir[i] = pThrow->TimeInAir != 0 && pThrow->TimeInAir != 2 * pPattern->Jugglers &&
                Time - (pObj->TimeOffset + SlotShift(pPattern, pObj->TimeOffset)) <
                    FlightSlots(pPattern, pObj->TimeOffset, pThrow->TimeInAir) - pPattern->CarryTime;
        }
        for (i = 0; i < pPattern->Objects; i++)
        {
            for (j = i + 1; j < pPattern->Objects; j++)
            {
                float dx, dy, dz, d;
                if (!pAir[i] || !pAir[j])
                    continue;
                dx = pPos[i].x - pPos[j].x;
                dy = pPos[i].y - pPos[j].y;
                dz = pPos[i].z - pPos[j].z;
                d = sqrtf(dx * dx + dy * dy + dz * dz);
                if (d < Best)
                {
                    Best = d;
                    *pSlotA = pPattern->pObjectInfo[i].ThrowIndex;
                    *pSlotB = pPattern->pObjectInfo[j].ThrowIndex;
                }
            }
        }
    }
    memcpy(pPattern->pObjectInfo, pSaved, Size);
    free(pAir);
    free(pPos);
    free(pSaved);
    return Best;
}

/* Widen the catches of passes that pass too close to another object in flight
 * until every pair of objects in the air stays apart (or we give up). */
static void SeparatePasses(PATTERN_INFO* pPattern, const EXT_SITE_INFO* pExtInfo, int Len)
{
    const float MinDistance = 0.68f;  /* two ball radii */
    const float Step = 0.15f;
    const float MaxSpread = 0.3f;  /* further out and the catcher cannot reach */
    size_t Size = sizeof(float) * pPattern->ThrowLen;
    float* pSpread = (float*) calloc(pPattern->ThrowLen, sizeof(float));
    float* pBest = (float*) calloc(pPattern->ThrowLen, sizeof(float));
    float Best = -1.0f;
    int Iteration, s;

    for (Iteration = 0; Iteration < 16; Iteration++)
    {
        int Slots[2] = {0, 0};
        int n;
        float d = ClosestInFlight(pPattern, &Slots[0], &Slots[1]);
        if (d > Best)
        {
            Best = d;
            memcpy(pBest, pSpread, Size);
        }
        if (d >= MinDistance)
            break;
        for (n = 0; n < 2; n++)
        {
            s = Slots[n];
            if (!pPattern->pThrowInfo[s].IsPass || pSpread[s] >= MaxSpread)
                continue;
            pSpread[s] += Step;
            InitPassingThrow(pPattern, s, pPattern->pThrowInfo[s].TimeInAir,
                pExtInfo != NULL ? &pExtInfo[s % Len] : NULL, pSpread[s]);
        }
    }

    /* Nudging can make things worse; keep the best layout found */
    if (memcmp(pBest, pSpread, Size) != 0)
    {
        for (s = 0; s < pPattern->ThrowLen; s++)
        {
            if (pPattern->pThrowInfo[s].IsPass)
                InitPassingThrow(pPattern, s, pPattern->pThrowInfo[s].TimeInAir,
                    pExtInfo != NULL ? &pExtInfo[s % Len] : NULL, pBest[s]);
        }
    }
    free(pBest);
    free(pSpread);
}

static int Gcd(int a, int b)
{
    while (b != 0)
    {
        int t = a % b;
        a = b;
        b = t;
    }
    return a;
}


/* Update the members of PATTERN_INFO for a given juggling pattern.  The pattern
 * can come from an ordinary siteswap (Site != NULL) or from a Juggle Saver
 * compatible pattern that contains, position and object info etc.
 * We assume that patterns are valid and have at least one object (a site of
 * zeros is invalid).  The ones we generate randomly are safe. */

void InitPatternInfo(PATTERN_INFO* pPattern,
    const int* Site, const EXT_SITE_INFO* pExtInfo, int Len)
{
    PASSING_LAYOUT Layout;
    memset(&Layout, 0, sizeof(Layout));
    Layout.Jugglers = 1;
    InitPatternInfoEx(pPattern, Site, pExtInfo, Len, &Layout);
}

void InitPatternInfoEx(PATTERN_INFO* pPattern,
    const int* Site, const EXT_SITE_INFO* pExtInfo, int Len, const PASSING_LAYOUT* pLayout)
{
    int Jugglers = pLayout->Jugglers;
    /* Repeat the site until it is a whole number of hand cycles (2J slots),
     * so every entry is always thrown by the same hand; for one juggler this
     * doubles odd-length sites so even indices are on one side and odds on
     * the other. */
    int Hold = 2 * Jugglers;
    int InfoLen = Len / Gcd(Len, Hold) * Hold;
    int Shifted = 0;
    int i;
    THROW_INFO* pInfo = (THROW_INFO*) calloc(InfoLen, sizeof(THROW_INFO));
    int Objects = 0;
    unsigned char* pUsed;

    pPattern->MaxWeight = 0;
    pPattern->ThrowLen = InfoLen;
    pPattern->pThrowInfo = pInfo;
    pPattern->Jugglers = Jugglers;
    pPattern->CarryTime = CARRY_TIME * Jugglers;
    memcpy(pPattern->HandShift, pLayout->HandShift, sizeof(pPattern->HandShift));
    pPattern->Formation = Jugglers > 2 ? pLayout->Formation : FORMATION_AUTO;
    for (i = 0; i < Hold; i++)
        Shifted |= pLayout->HandShift[i] != 0;
    ApplyFormation(pPattern, pLayout);

    SetHeightAndAlpha(pPattern, Site, pExtInfo, Len, Jugglers);

    /* First pass through we assign the things we know about for sure just by
     * looking at the throw weight at this position.  This includes TimeInAir;
     * the throw and catch positions; and throw and catch velocities.
     * Other information, like the total time for the throw (i.e. when the
     * object is thrown again) relies on how the rest of the pattern is
     * structured and we defer this task for successive passes and just make
     * guesses at this stage. */

    for (i = 0; i < InfoLen; i++)
    {
        float t1;
        int w = pExtInfo != NULL ? pExtInfo[i % Len].Weight : Site[i % Len];

        pInfo[i].TotalTime = pInfo[i].TimeInAir = w;

        /* several jugglers, or hands that throw together, set up as passing */
        if (Jugglers > 1 || Shifted)
        {
            pInfo[(w + i) % InfoLen].PrevThrow = i;
            InitPassingThrow(pPattern, i, w, pExtInfo != NULL ? &pExtInfo[i % Len] : NULL, 0.0f);
            Objects += w;
            if (w > pPattern->MaxWeight)
                pPattern->MaxWeight = w;
            continue;
        }

        pInfo[(w + i) % Len].PrevThrow = i;

        /* work out where we are throwing this object from and where it's going
         * to land. */

        if (pExtInfo == NULL || (pExtInfo[i % Len].Flags & HAS_FROM_POS) == 0)
            GetDefaultFromPosition(i % 2, w, &pInfo[i].FromPos);
        else
            pInfo[i].FromPos = pExtInfo[i % Len].FromPos;

        if (pExtInfo == NULL || (pExtInfo[i % Len].Flags & HAS_TO_POS) == 0)
            GetDefaultToPosition(i % 2, w, &pInfo[i].ToPos);
        else
            pInfo[i].ToPos = pExtInfo[i % Len].ToPos;

        /* calculate the velocity the object is moving at the start and end
         * points -- this information is used to interpolate the hand position
         * and to determine how the object is moved while it's carried to the
         * next throw position.
         *
         * The throw motion is governed by a parabola of the form:
         *   y(t) = a * t ^ 2 + b * t + c
         * Assuming at the start of the throw y(0) = y0; when it's caught
         * y(t1) = y1; and the accelation is -2.0 * alpha the equation can be
         * rewritten as:
         *   y(t) = -alpha * t ^ 2 + (alpha * t1 + (y1 - y0) / t1) * t + y0
         * making the velocity:
         *   y'(t) = -2.0 * alpha * t + (alpha * t1 + (y1 - y0) / t1)
         * To get the y component of velocity first we determine t1, which is
         * the throw weight minus the time spent carrying the object.  Then
         * perform the relevant substitutions into the above.
         * (note: y'(t) = y'(0) - 2.0 * alpha * t)
         *
         * The velocity in the x direction is constant and can be simply
         * obtained from:
         *   x' = (x1 - x0) / t1
         * where x0 and x1 are the start and end x-positions respectively.
         */

        t1 = w - CARRY_TIME;

        pInfo[i].FromVelocity.y = pPattern->Alpha * t1 +
            (pInfo[i].ToPos.y - pInfo[i].FromPos.y) / t1;
        pInfo[i].ToVelocity.y =
            pInfo[i].FromVelocity.y - 2.0f * pPattern->Alpha * t1;
        pInfo[i].FromVelocity.x = pInfo[i].ToVelocity.x =
            (pInfo[i].ToPos.x - pInfo[i].FromPos.x) / t1;
        pInfo[i].FromVelocity.z = pInfo[i].ToVelocity.z =
            (pInfo[i].ToPos.z - pInfo[i].FromPos.z) / t1;
        pInfo[i].FromVelocity.Rot = pInfo[i].ToVelocity.Rot =
            (pInfo[i].ToPos.Rot - pInfo[i].FromPos.Rot) / t1;
        pInfo[i].FromVelocity.Elev = pInfo[i].ToVelocity.Elev =
            (pInfo[i].ToPos.Elev - pInfo[i].FromPos.Elev) / t1;


        if (pExtInfo != NULL && (pExtInfo[i % Len].Flags & HAS_SNATCH) != 0)
        {
            pInfo[i].ToVelocity.x = pExtInfo[i % Len].SnatchX;
            pInfo[i].ToVelocity.y = pExtInfo[i % Len].SnatchY;
        }

        if (pExtInfo != NULL && (pExtInfo[i % Len].Flags & HAS_SPINS) != 0)
        {
            pInfo[i].ToPos.Elev = 360.0f * pExtInfo[i % Len].Spins +
                NormaliseAngle(pInfo[i].ToPos.Elev);
        }

        Objects += w;
        if (w > pPattern->MaxWeight)
            pPattern->MaxWeight = w;
    }

    Objects /= InfoLen;

    /* Now we go through again and work out exactly how long it is before the
     * object is thrown again (ie. the TotalTime) typically this is the same
     * as the time in air, however when we have a throw weight of '2' it's
     * treated as a hold and we increase the total time accordingly. */

    for (i = 0; i < InfoLen; i++)
    {
        if (pInfo[i].TimeInAir != Hold)
        {
            int Next = pInfo[i].TimeInAir + i;
            while (pInfo[Next % InfoLen].TimeInAir == Hold)
            {
                Next += Hold;
                pInfo[i].TotalTime += Hold;
            }

            /* patch up the Prev index.  We don't bother to see if this
             * is different from before since it's always safe to reassign it */
            pInfo[Next % InfoLen].PrevThrow = i;
        }
    }

    /* then we work our way through again figuring out where the hand goes to
     * catch something as soon as it has thrown the current object. */

    for (i = 0; i < InfoLen; i++)
    {
        if (pInfo[i].TimeInAir != 0 && pInfo[i].TimeInAir != Hold)
        {
            /* what we're trying to calculate is how long the hand that threw
             * the current object has to wait before it throws another.
             * Typically this is two beats later.  However '0' in the site swap
             * represents a gap in a catch, and '2' represents a hold.  We skip
             * over these until we reach the point where a ball is actually
             * thrown. */
            int Wait = Hold;
            while (pInfo[(i + Wait) % InfoLen].TimeInAir == Hold ||
                pInfo[(i + Wait) % InfoLen].TimeInAir == 0)
            {
                Wait += Hold;
            }
            pInfo[i].NextForHand = Wait;
        }
        else
        {
            /* Be careful to ensure the current weight isn't one we're trying
             * to step over; otherwise we could potentially end up in an
             * infinite loop.  The value we assign may end up being used
             * in patterns with infinite gaps (e.g. 60) or infinite holds
             * (e.g. 62) in both cases, setting a wait of 2 ensures things
             * are well behaved. */
            pInfo[i].NextForHand = Hold;
        }
    }

    /* Now work out the starting positions for the objects.  To do this we
     * unweave the initial throws so we can pick out the individual threads. */

    pUsed = (unsigned char*)
        malloc(sizeof(unsigned char) * pPattern->MaxWeight);
    pPattern->Objects = Objects;
    pPattern->pObjectInfo = (OBJECT_POSITION*) calloc(
        Objects, sizeof(OBJECT_POSITION));

    for (i = 0; i < pPattern->MaxWeight; i++)
        pUsed[i] = 0;

    for (i = 0; i < pPattern->MaxWeight; i++)
    {
        int w = pInfo[i % InfoLen].TimeInAir;
        if (pUsed[i] == 0 &&  w != 0)
        {
            Objects--;
            pPattern->pObjectInfo[Objects].TimeOffset = i;
            pPattern->pObjectInfo[Objects].ThrowIndex = i % InfoLen;
            pPattern->pObjectInfo[Objects].TotalTwist = 0.0f;

            if (pExtInfo != NULL &&
                pExtInfo[i % Len].ObjectType != OBJECT_DEFAULT)
            {
                pPattern->pObjectInfo[Objects].ObjectType =
                    pExtInfo[i % Len].ObjectType;
            }
            else
            {
                pPattern->pObjectInfo[Objects].ObjectType = (1 + rand() % 3);
            }
        }

        if (w + i < pPattern->MaxWeight)
            pUsed[w + i] = 1;

    }

    for (i = 0; i < Hold; i++)
    {
        pPattern->Hands[i].TimeOffset = pPattern->Hands[i].ThrowIndex = i;
        pPattern->Hands[i].TotalTwist = 0.0f;
        pPattern->Hands[i].ObjectType = OBJECT_DEFAULT;
    }

    if (Jugglers > 1)
        SeparatePasses(pPattern, pExtInfo, Len);

    free(pUsed);
}


void ReleasePatternInfo(PATTERN_INFO* pPattern)
{
    free(pPattern->pObjectInfo);
    free(pPattern->pThrowInfo);
}


/*****************************************************************************
 *
 * Sites
 *
 ****************************************************************************/

/* Generate a random site swap.  We assume that MaxWeight >= ObjCount and
 * Len >= MaxWeight. */

int* Generate(int Len, int MaxWeight, int ObjCount)
{
    int* Weight = (int*) calloc(Len, sizeof(int));
    int* Used = (int*) calloc(Len, sizeof(int));
    int* Options = (int*) calloc(MaxWeight + 1, sizeof(int));
    int nOpts;
    int i, j;

    for (i = 0; i < Len; i++)
        Weight[i] = Used[i] = -1;

    /* Pick out a unique the starting position for each object.  -2 is put in
     * the Used array to signify this is a starting position. */

    while (ObjCount > 0)
    {
        nOpts = 0;
        for (j = 0; j < MaxWeight; j++)
        {
            if (Used[j] == -1)
                Options[nOpts++] = j;
        }

        Used[Options[rand() % nOpts]] = -2;
        ObjCount--;
    }

    /* Now work our way through the pattern moving throws into an available
     * landing positions. */
    for (i = 0; i < Len; i++)
    {
        if (Used[i] == -1)
        {
            /* patch up holes in the pattern to zeros */
            Used[i] = 1;
            Weight[i] = 0;
        }
        else
        {
            /* Work out the possible places where a throw can land and pick a
             * weight at random. */
            int w;
            nOpts = 0;

            for (j = 0 ; j <= MaxWeight; j++)
            {
                if (Used[(i + j) % Len] == -1)
                    Options[nOpts++] = j;
            }

            w = Options[rand() % nOpts];
            Weight[i] = w;

            /* For starting throws make position available for a throw to land.
             * Because Len >= MaxWeight these positions will only be filled when
             * a throw wraps around the end of the site swap and therefore we
             * can guarantee the all the object threads will be tied up. */
            if (Used[i] == -2)
                Used[i] = -1;

            Used[(i + w) % Len] = 1;
        }
    }

    free(Options);
    free(Used);
    return Weight;
}


/* Routines to parse the Juggle Saver patterns.  These routines are a bit yucky
 * and make the big assumption that the patterns are well formed.  This is fine
 * as it stands because only 'good' ones are used but if the code is ever
 * extended to read arbitrary patterns (say from a file) then these routines
 * need to be beefed up. */

/* The position text looks something like (x,y,z[,rot[,elev]])
 * where the stuff in square brackets is optional
 *
 * There are two types:
 *  (x,y[,rot[,elev]]) for throw and catch position
 *  (x,y) for snatch
 */
unsigned char ParsePositionText(const char** ppch, POS* pPos, int min, int max,
    unsigned char* pFields)
{
    const char* pch = *ppch;
    unsigned char OK;
    *pFields = 0;
    char szTemp[32];
    char* pOut;
    float* Nums[4];
    int i;
    int counter = 0;

    Nums[0] = &pPos->x;
    Nums[1] = &pPos->y;
    Nums[2] = &pPos->Rot;
    Nums[3] = &pPos->Elev;


    while (*pch == ' ')
        pch++;

    OK = *pch == '(';

    if (OK)
        pch++;

    for (i = 0; OK && i < 4; i++)
    {
        pOut = szTemp;
        while (*pch == ' ')
            pch++;
        while (*pch != ',' && *pch != '\0' && *pch != ')' && *pch != ' ')
        {
            if (pOut == szTemp + sizeof(szTemp) - 1)
                return 0; // a number longer than any position
            *pOut++ = *pch++;
        }
        *pOut = '\0';

        if (szTemp[0] != '\0')
        {
            *Nums[i] = (float) atof(szTemp);
            *pFields |= (unsigned char) (1 << i);
            counter++;
        }

        while (*pch == ' ')
            pch++;

        if (i < 3)
        {
            if (*pch == ',')
                pch++;
            else if (*pch == ')')
                break;
            else
                OK = 0;
        }
    }

    if (OK)
    {
        while (*pch == ' ')
            pch++;
        if (*pch == ')')
            pch++;
        else
            OK = 0;
    }

    *ppch = pch;

    if (counter < min || counter > max) OK = 0;

    return OK;
}

/* helper macros for pattern parsing */
#define IS_VALID_THROW(t) ( ((t) >= '0' && (t) <= '9') || ((t) >= 'a' && (t) <= 'z') || ((t) >= 'A' && (t) <= 'Z') )

/* store the current siteswap being parsed with all style info stripped */
static char* vss = NULL;

char* GetCurrentSite() { return strdup(vss); }

EXT_SITE_INFO* ParsePattern(const char* Site, int* pLen)
{
    PASSING_LAYOUT Layout;
    EXT_SITE_INFO* pInfo = ParsePatternEx(Site, pLen, &Layout);
    int i, Shifted = 0;
    for (i = 0; i < JS_MAX_HANDS; i++)
        Shifted |= Layout.HandShift[i] != 0;
    if (pInfo != NULL && (Layout.Jugglers != 1 || Shifted))
    {
        free(pInfo);
        pInfo = NULL;
        *pLen = 0;
    }
    return pInfo;
}

static EXT_SITE_INFO* ParseSite(const char* Site, int* pLen, int* pJugglers, int Generated);

/* Like ParsePattern, but also accepts passing patterns: a site "J<n>:<site>"
 * shared by n jugglers (four-handed siteswap for n = 2), and the notations of
 * js_passing.h (Prechac, synchronous passing, solo sync), which are turned
 * into that form.  pLayout says how the jugglers and hands share the site. */
EXT_SITE_INFO* ParsePatternEx(const char* Site, int* pLen, PASSING_LAYOUT* pLayout)
{
    PASSING_PATTERN* pPassing = (PASSING_PATTERN*) malloc(sizeof(PASSING_PATTERN));
    EXT_SITE_INFO* pInfo;

    memset(pLayout, 0, sizeof(*pLayout));
    pLayout->Feeder = -1;
    if (ParsePassing(Site, pPassing))
    {
        char* pSite = PassingToSite(pPassing, pLayout);
        int Jugglers;
        free(pPassing);
        if (pSite == NULL)
        {
            if (vss != NULL) free(vss);
            vss = strdup("");
            *pLen = 0;
            pLayout->Jugglers = 1;
            return NULL;
        }
        pInfo = ParseSite(pSite, pLen, &Jugglers, 1);
        free(pSite);
        return pInfo;
    }
    free(pPassing);
    return ParseSite(Site, pLen, &pLayout->Jugglers, 0);
}

/* Generated: the site was written by PassingToSite, so its throw values are
 * slots rather than what the throws look like. */
static EXT_SITE_INFO* ParseSite(const char* Site, int* pLen, int* pJugglers, int Generated)
{
    if (vss != NULL) free(vss);
    vss = (char*)malloc (strlen(Site)+1 * sizeof(char));
    memset(vss, 0, strlen(Site)+1);
    char* cur_throw = vss;

    int J = 1;
    if (Site[0] == 'J' && Site[1] >= '0' && Site[1] <= '9')
    {
        const char* p = Site + 1;
        int n = 0;
        while (*p >= '0' && *p <= '9' && n <= JS_MAX_JUGGLERS)
            n = n * 10 + *p++ - '0';
        if (*p == ':')
        {
            if (n < 1 || n > JS_MAX_JUGGLERS)
            {
                *pLen = 0;
                *pJugglers = 1;
                return NULL;
            }
            J = n;
            Site = p + 1;
        }
    }
    *pJugglers = J;

    const char* pch = Site;
    int Len = 0;
    EXT_SITE_INFO* pInfo = NULL;
    unsigned char OK = 1;

    while (OK && *pch != 0)
    {
        EXT_SITE_INFO Info;
        Info.Flags = 0;
        Info.FromFields = Info.ToFields = 0;

        while (*pch == ' ') pch++;

        OK = *pch != '\0';

        /* parse throw */
        if (OK) {
          if (IS_VALID_THROW(*pch))
          {
              *cur_throw++ = *pch;
              Info.Weight = *pch >= 'A' ? *pch + 10 - 'A' : *pch - '0';
          }
          else
          {
              OK = 0;
          }
        }

        /* parse object type */
        if (OK)
        {
            pch++;
            while (*pch == ' ') pch++;

            if (*pch == 'b' || *pch == 'B')
            {
                Info.ObjectType = OBJECT_BALL;
                pch++;
            }
            else if (*pch == 'c' || *pch == 'C')
            {
                Info.ObjectType = OBJECT_CLUB;
                pch++;
            }
            else if (*pch == 'r' || *pch == 'R')
            {
                Info.ObjectType = OBJECT_RING;
                pch++;
            }
            else if (*pch == 'd' || *pch == 'D')
            {
                Info.ObjectType = OBJECT_DEFAULT;
                pch++;
            }
            else
            {
                /* No object found, next item must be either site digit or @ */
                if (IS_VALID_THROW(*pch) || *pch == '@')
                    Info.ObjectType = OBJECT_DEFAULT;
                else if (*pch != 0)
                    OK = 0;
            }
        }

        int HasFromPosition = 0;

        /* Parse from position */
        if (OK)
        {
            while (*pch == ' ') pch++;
            if (*pch == '@')
            {
                HasFromPosition = 1;
                pch++;
                GetDefaultThrowPositions(Len, Info.Weight, Info.Weight, J, &Info.FromPos, &Info.ToPos);
                Info.Flags |= HAS_FROM_POS;
                OK = ParsePositionText(&pch, &Info.FromPos, 2, 4, &Info.FromFields);
            }
        }

        /* Parse to position (must be present if there is a from position) */
        if (OK)
        {
            while (*pch == ' ') pch++;
            if (*pch == '>')
            {
                if (!HasFromPosition)
                {
                    OK = 0;
                }
                else
                {
                    pch++;
                    Info.Flags |= HAS_TO_POS;
                    OK = ParsePositionText(&pch, &Info.ToPos, 2, 4, &Info.ToFields);
                }
            }
            else if (HasFromPosition)
            {
                OK = 0;
            }
        }

        /* Parse snatch (optional) */
        if (OK)
        {
            while (*pch == ' ') pch++;
            if (*pch == '/')
            {
                POS Snatch;
                unsigned char Fields;
                pch++;
                Info.Flags |= HAS_SNATCH;
                OK = ParsePositionText(&pch, &Snatch, 2, 2, &Fields);
                Info.SnatchX = Snatch.x;
                Info.SnatchY = Snatch.y;
            }
        }

        /* Parse Spins (optional) */
        if (OK)
        {
            int GivenSpins = 0;
            while (*pch == ' ') pch++;
            if (*pch == '*')
            {
                GivenSpins = 1;
                pch++;
                OK = 0;
                Info.Spins = 0;
                while (*pch >= '0' && *pch <= '9')
                {
                    OK = 1;
                    Info.Spins = Info.Spins * 10 + *pch - '0';
                    pch++;
                }
            }
            else
                Info.Spins = GetDefaultThrowSpins(Info.Weight, J);

            /* with several jugglers or shifted hands the spins follow the
             * flight, worked out in InitPassingThrow, unless the pattern says */
            if (GivenSpins || (J == 1 && !Generated))
                Info.Flags |= HAS_SPINS;
        }

        if (OK)
        {
            if (pInfo == NULL)
                pInfo = (EXT_SITE_INFO*) malloc(sizeof(EXT_SITE_INFO));
            else
                pInfo = (EXT_SITE_INFO*) realloc(pInfo, (Len + 1) * sizeof(EXT_SITE_INFO));

            pInfo[Len] = Info;
            Len++;
        }
    }

    if (!OK && pInfo != NULL)
    {
        free(pInfo);
        pInfo = NULL;
    }

    *pLen = Len;

    return pInfo;
}

#ifdef __cplusplus
}
#endif
