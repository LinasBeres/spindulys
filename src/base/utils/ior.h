#ifndef INDEX_OF_REFRACTION_H
#define INDEX_OF_REFRACTION_H

#include <string>
#include <sstream>

#include "../spindulysBase.h"

BASE_NAMESPACE_OPEN_SCOPE

struct IOREntry {
	const char *name;
	float value;
};

/**
 * Many values are taken from Hecht, Optics,
 * Fourth Edition.
 *
 * The IOR values are from measurements between
 * 0 and 20 degrees celsius at ~589 nm.
 */
const static IOREntry IORData[] =
{
	{ "vacuum",                1.0f },
	{ "helium",                1.000036f },
	{ "hydrogen",              1.000132f },
	{ "air",                   1.000277f },
	{ "carbon dioxide",        1.00045f },
	//////////////////////////////////////
	{ "water",                 1.3330f },
	{ "acetone",               1.36f },
	{ "ethanol",               1.361f },
	{ "carbon tetrachloride",  1.461f },
	{ "glycerol",              1.4729f },
	{ "benzene",               1.501f },
	{ "silicone oil",          1.52045f },
	{ "bromine",               1.661f },
	//////////////////////////////////////
	{ "water ice",             1.31f },
	{ "fused quartz",          1.458f },
	{ "pyrex",                 1.470f },
	{ "acrylic glass",         1.49f },
	{ "polypropylene",         1.49f },
	{ "bk7",                   1.5046f },
	{ "sodium chloride",       1.544f },
	{ "amber",                 1.55f },
	{ "pet",                   1.5750f },
	{ "diamond",               2.419f },

	{ nullptr,                 0.0f }
};

static float LookupIOR(const std::string& name)
{
	std::string lowerCase;
	lowerCase.resize(name.size());
	for (size_t i = 0; i < name.size(); ++i)
		lowerCase[i] = std::tolower(name[i]);

	const IOREntry *ior = IORData;

	while (ior->name)
	{
		if (lowerCase == ior->name)
			return ior->value;
		++ior;
	}

	std::ostringstream oss;
	oss << "Unable to find an IOR value for \"" << lowerCase
		<< "\"!\n Valid choices are: ";

	// Unable to find the IOR value by name -- print an error
	// message that lists all possible options
	for (ior = IORData; ior->name != NULL; ++ior)
	{
		oss << ior->name;
		if ((ior + 1)->name)
			oss << ", ";
	}

	spdlog::warn(oss.str());
	return 0.f;
}

BASE_NAMESPACE_CLOSE_SCOPE

#endif // INDEX_OF_REFRACTION_H
