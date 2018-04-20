#ifndef SUPPORT_H
#define SUPPORT_H
#include "provided.h"
bool operator== (const GeoCoord &lhs, const GeoCoord &rhs)
{
	if (lhs.latitude != rhs.latitude || lhs.longitude != rhs.longitude)
		return false;
	return true;
}
#endif