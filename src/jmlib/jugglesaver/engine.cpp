/* Juggler3D, Copyright (c) 2005-2008 Brian Apps <brian@jugglesaver.co.uk>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.  No
 * representations are made about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

#include "jugglesaver.h"

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
 
float GetBallTwistAmount(const THROW_INFO* pThrow)
{
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
    float TLen, float t)
{
    POS p;
    float a, b, c, d, tt, tc;
    
    /* The interpolation is based on a simple cubic that achieves 1st order
     * continuity at the end points.  However the spline can become too long if
     * the TLen parameter is large.  In this case we cap the curve's length (fix
     * the shape) and then reparameterise time to achieve the continuity
     * conditions. */

    tc = CARRY_TIME;
    
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


POS InterpolateCarry(
    const THROW_INFO* pThrow, const THROW_INFO* pNext, float t)
{
    float CT = CARRY_TIME + pThrow->TotalTime - pThrow->TimeInAir;
    return InterpolatePosition(&pThrow->ToPos, &pThrow->ToVelocity,
        &pNext->FromPos, &pNext->FromVelocity, CT, t);
}


/* Determine the position of the hand at a point in time. */

void GetHandPosition(
    PATTERN_INFO* pPattern, int RightHand, float Time, POS* pPos)
{
    OBJECT_POSITION* pObj = 
        RightHand == 0 ? &pPattern->LeftHand : &pPattern->RightHand;
    THROW_INFO* pLastThrow;
    
    /* Upon entry, the throw information for the relevant hand may be out of
     * sync.  Therefore we advance through the pattern if required. */

    while (pPattern->pThrowInfo[pObj->ThrowIndex].NextForHand + pObj->TimeOffset 
        <= (int) Time)
    {
        int w = pPattern->pThrowInfo[pObj->ThrowIndex].NextForHand;
        pObj->TimeOffset += w;
        pObj->ThrowIndex = (pObj->ThrowIndex + w) % pPattern->ThrowLen;
    }

    pLastThrow = &pPattern->pThrowInfo[pObj->ThrowIndex];

    /* The TimeInAir will only ever be 2 or 0 if no object is ever thrown by
     * this hand.  In normal circumstances, 2's in the site swap are coalesced
     * and added to TotalTime of the previous throw.  0 is a hole and means that
     * an object isn't there.  In this case we just hold the hand still. */
    if (pLastThrow->TimeInAir == 2 || pLastThrow->TimeInAir == 0)
    {
        pPos->x = pLastThrow->FromPos.x;
        pPos->y = pLastThrow->FromPos.y;
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
        
        float tc = CARRY_TIME + 
            pNextThrownFrom->TotalTime - pNextThrownFrom->TimeInAir;
        
        Time -= pObj->TimeOffset;

        if (Time > pLastThrow->NextForHand - tc)
        {
            /* carrying this ball to it's new location */
            *pPos = InterpolateCarry(pNextThrownFrom,
                pNextThrow, (Time - (pLastThrow->NextForHand - tc)));
        }
        else
        {
            /* going for next catch */
            *pPos = InterpolatePosition(
                &pLastThrow->FromPos, &pLastThrow->FromVelocity, 
                &pNextThrownFrom->ToPos, &pNextThrownFrom->ToVelocity,
                pLastThrow->NextForHand - tc, Time);
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

    while (pPattern->pThrowInfo[pObj->ThrowIndex].TotalTime + pObj->TimeOffset
        <= (int) Time)
    {
        int w = pPattern->pThrowInfo[pObj->ThrowIndex].TotalTime;
        pObj->TimeOffset += w;
        pObj->TotalTwist = NormaliseAngle(pObj->TotalTwist + 
            GetBallTwistAmount(&pPattern->pThrowInfo[pObj->ThrowIndex]));
        
        pObj->ThrowIndex = (pObj->ThrowIndex + w) % pPattern->ThrowLen;
    }

    pThrow = &pPattern->pThrowInfo[pObj->ThrowIndex];

    if (pThrow->TimeInAir == 2 || pThrow->TimeInAir == 0)
    {
        *pPos = pThrow->FromPos;
        OffsetHandlePosition(pPos, HandleLen, pPos);
    }
    else
    {
        float tc = pThrow->TimeInAir - CARRY_TIME;
        float BallTwist = GetBallTwistAmount(pThrow);
        Time -= pObj->TimeOffset;
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

            *pPos = InterpolateCarry(pThrow, pNextThrow, Time - tc);

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
    const int* Site, const EXT_SITE_INFO* pExtInfo, int Len)
{
    float H;
    int MaxW = 5;
    int i;
    
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


/* Update the members of PATTERN_INFO for a given juggling pattern.  The pattern
 * can come from an ordinary siteswap (Site != NULL) or from a Juggle Saver
 * compatible pattern that contains, position and object info etc. 
 * We assume that patterns are valid and have at least one object (a site of
 * zeros is invalid).  The ones we generate randomly are safe. */

void InitPatternInfo(PATTERN_INFO* pPattern,
    const int* Site, const EXT_SITE_INFO* pExtInfo, int Len)
{
    /* Double up on the length of the site if it's of an odd length. 
     * This way we can store position information: even indices are on one
     * side and odds are on the other. */
    int InfoLen = Len % 2 == 1 ? Len * 2 : Len;
    int i;
    THROW_INFO* pInfo = (THROW_INFO*) calloc(InfoLen, sizeof(THROW_INFO));
    int Objects = 0;
    unsigned char* pUsed;
    
    pPattern->MaxWeight = 0;
    pPattern->ThrowLen = InfoLen;
    pPattern->pThrowInfo = pInfo;
    
    SetHeightAndAlpha(pPattern, Site, pExtInfo, Len);

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
        if (pInfo[i].TimeInAir != 2)
        {
            int Next = pInfo[i].TimeInAir + i;
            while (pInfo[Next % InfoLen].TimeInAir == 2)
            {
                Next += 2;
                pInfo[i].TotalTime += 2;
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
        if (pInfo[i].TimeInAir != 0 && pInfo[i].TimeInAir != 2)
        {
            /* what we're trying to calculate is how long the hand that threw
             * the current object has to wait before it throws another.
             * Typically this is two beats later.  However '0' in the site swap
             * represents a gap in a catch, and '2' represents a hold.  We skip
             * over these until we reach the point where a ball is actually
             * thrown. */
            int Wait = 2;
            while (pInfo[(i + Wait) % InfoLen].TimeInAir == 2 || 
                pInfo[(i + Wait) % InfoLen].TimeInAir == 0)
            {
                Wait += 2;
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
            pInfo[i].NextForHand = 2;
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

    pPattern->LeftHand.TimeOffset = pPattern->LeftHand.ThrowIndex = 0;
    pPattern->RightHand.TimeOffset = pPattern->RightHand.ThrowIndex = 1;
    
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
unsigned char ParsePositionText(const char** ppch, POS* pPos, int min, int max)
{
    const char* pch = *ppch;
    unsigned char OK;
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
            *pOut++ = *pch++;
        *pOut = '\0';

        if (szTemp[0] != '\0')
        {
            *Nums[i] = (float) atof(szTemp);
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
    if (vss != NULL) free(vss);
    vss = (char*)malloc (strlen(Site)+1 * sizeof(char));
    memset(vss, 0, strlen(Site)+1);
    char* cur_throw = vss;

    const char* pch = Site;
    int Len = 0;
    EXT_SITE_INFO* pInfo = NULL;
    unsigned char OK = 1;

    while (OK && *pch != 0)
    {
        EXT_SITE_INFO Info;
        Info.Flags = 0;

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
                else if (*pch != NULL)
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
                GetDefaultFromPosition(Len % 2, Info.Weight, &Info.FromPos);
                Info.Flags |= HAS_FROM_POS;
                OK = ParsePositionText(&pch, &Info.FromPos, 2, 4);
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
                    GetDefaultToPosition(Len % 2, Info.Weight, &Info.ToPos);
                    Info.Flags |= HAS_TO_POS;
                    OK = ParsePositionText(&pch, &Info.ToPos, 2, 4);
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
                pch++;
                Info.Flags |= HAS_SNATCH;
                OK = ParsePositionText(&pch, &Snatch, 2, 2);
                Info.SnatchX = Snatch.x;
                Info.SnatchY = Snatch.y;
            }
        }

        /* Parse Spins (optional) */
        if (OK)
        {
            while (*pch == ' ') pch++;
            if (*pch == '*')
            {
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
                Info.Spins = GetDefaultSpins(Info.Weight);

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
