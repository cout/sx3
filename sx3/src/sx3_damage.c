// File: sx3_damageCalc.c
// Author: Jonathan Brannan, Daniel Ott
//
// The following functions calculate the damage of
// explosion on the shields and tanks.
// NO HEADER Created as of 2001-07-01

#include <math.h>
#include <stdio.h>

#include "sx3_collision.h"
#include "sx3_tanks.h"
#include "sx3_weapons.h"

// Distance Between Two Points
float distBtwTwoPts(float fx1, float fx2, float fy1, float fy2) {
	return (float) sqrt( (fx1 - fx2)^2 + (fy1 - fy2)^2 )
}

// Area of Intersecting Circles
float areaOfIntersectingCircles(Vector pos1, Vector pos2, float fRadius1, 
							float radius2) {

    float fDistTankExplosion;	// Distance between Tank & Explosion
    float fAngleOfSector1;		// in radians
	float fAngleOfSector2;		// in radians
	
	//Calculate Area of Intersection between Two Circles
	fDistTankExplosion = distBtwTwoPts(pos1[0], pos2[0], pos1[1],
                         pos2[1]);

	// Not sure if sectors are in radians, and this doesn't check for
	// complex results
	// CHECK ME!!!
	fAngleOfSector1 = 2 * acos( ( (fRadius1)^2 + fDistTankExplosion^2 -
		                        (fRadius2)^2 ) / (2 * fRadius1^2 *
                                fDistTankExplosion) );

	fAngleOfSector2 = 2 * acos( ( (fRadius2)^2 + fDistTankExplosion^2 -
                                (fRadius1)^2 ) / (2 * fRadius2^2 *
                                fDistTankExplosion) );


    return ( (0.5)*(fAngleOfSector1)*(fRadius1)^2 - (0.5)*(fRadius1)^2*(sin(fAngleOfSector1))
			+ (0.5)*(fAngleOfSector2)*(fRadius2)^2 - (0.5)*(fRadius2)^2*(sin(fAngleOfSector2)) );
}

// Percent of Shield Affected by Intersecting Blast
float percentShieldAffect(float fShieldRadius, float fAreaOfIntersectingCircles) {
    float fTotalShieldArea;	

    fTotalArea = PI*(fRadius)^2;

    return (fAreaOfIntersectingCircles / fTotalArea);
}

// Percent of Explosion Damage
float percentExplosionDamage ( float fShieldRadius, float fExplosionRadius, 
                         float fDistBtwTankProjectile) {

	float fShieldOverlapByExplosion;

	fShieldOverlapByExplosion = ( (fShieldRadius + fExplosionRadius) -
                                 fDistBtwTankProjectile );

	return (fShieldOverlapByExplosion / fExplosionRadius);
}

// Percent of Explosion Force that reaches the shield.  
float powerOfExplosionHit(float fPercentExplosionDamage, float fPowerOfExplosion) {
    return (fPercentExplosionDamage * fPowerOfExplosion);
}

// Power of Explosion on Shield
float powerOfExplosionOnShield(float fPercentShieldAffect, float fPowerOfExplosionHit) {
	return (fPercentShieldAffect * fPowerOfExplosion);
}

// Power of Explosion on Tank
float powerOfExplosionOnTank(float fRemaingShieldPower, float fPowerOfExplosionShield) {
    return (1.00 - (fRemainingShieldPower * 0.01) * fPowerOfExplosion);
}

