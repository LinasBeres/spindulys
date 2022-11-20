#include "cpuDielectric.h"

#include <spindulys/fresnel.h>

#include "../../utils/interaction.h"

CPU_NAMESPACE_OPEN_SCOPE

CPUSmoothDielectric::CPUSmoothDielectric(float interiorIOR, float exteriorIOR,
				const Col3f& specularReflectance, const Col3f& specularTransmittance, const std::string& id)
	: SmoothDielectric(interiorIOR, exteriorIOR, specularReflectance, specularTransmittance, id)
{
}

std::pair<BSDFSample, Col3f>
CPUSmoothDielectric::Sample(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		float sample1,
		const Vec2f& sample2,
		uint32_t active) const
{
	bool hasReflection   = ctx.IsEnabled(BSDFFlags::DeltaReflection, 0);
	bool hasTransmission = ctx.IsEnabled(BSDFFlags::DeltaTransmission, 1);

	float cosThetaI = si.wi.z;

	auto [rI, cosThetaT, etaIt, etaTi] = fresnel(cosThetaI, GetETA());
	float tI = 1.f - rI;

	active &= cosThetaI > 0.f;

	BSDFSample bs;
	bool selectedR;
	if (likely(hasReflection && hasTransmission))
	{
		selectedR = sample1 <= rI && active;
		bs.pdf = select(selectedR, rI, tI);
	}
	else
	{
		if (hasReflection || hasTransmission)
		{
			selectedR = hasReflection && active;
			bs.pdf = 1.f;
		}
		else
		{
			return { bs, zero };
		}
	}

	bool selectedT = !selectedR && active;

	bs.sampledComponent = selectedR ? 0u : 1u;
	bs.sampledType      = selectedR ? +BSDFFlags::DeltaReflection : +BSDFFlags::DeltaTransmission;

	bs.wo = select(selectedR, reflect(si.wi), refract(si.wi, cosThetaT, etaTi));

	bs.eta = select(selectedR, 1.f, etaIt);

	Col3f reflectance = GetSpecularReflection();
	Col3f transmittance = GetSpecularTransmittance();

	float weight = 0.f;
	if (likely(hasReflection && hasTransmission))
	{
		weight = 1.f;
	}
	else if (hasReflection || hasTransmission)
	{
		weight = hasReflection ? rI : tI;
	}

	if (selectedT)
	{
		/* For transmission, radiance must be scaled to account for the solid
			 angle compression that occurs when crossing the interface. */
		float factor = (ctx.mode == TransportMode::Radiance) ? etaTi : 1.f;
		transmittance *= sqr(factor);
	}

	Col3f value = (selectedR ? reflectance : transmittance) * weight;

	return { bs, value };
}

Col3f CPUSmoothDielectric::Eval(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		const Vec3f& wo,
		uint32_t active) const
{
	return zero;
}

float CPUSmoothDielectric::Pdf(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		const Vec3f& wo,
		uint32_t active) const
{
	return zero;
}

CPU_NAMESPACE_CLOSE_SCOPE
