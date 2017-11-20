#include "gridRemap.hpp"

namespace sphereRemap {

Coord readPole(std::istream &is)
{
	double lon, lat;
	is >> lon >> lat;
	return (lon<=380 && lat<=380) ? xyz(lon, lat) : ORIGIN;
}

CRemapGrid srcGrid;
#pragma omp threadprivate(srcGrid)

CRemapGrid tgtGrid;
#pragma omp threadprivate(tgtGrid)

}
