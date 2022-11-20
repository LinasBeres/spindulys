#include "cpuGeometry.h"

#include <utils/ior.h>

#include "../utils/interaction.h"

// TODO: We currently only have diffuse bsdfs,
// but shapes should be able to choose what kind they are.
#include "../bsdf/smooth/cpuDiffuse.h"
#include "../bsdf/smooth/cpuConductor.h"
#include "../bsdf/smooth/cpuDielectric.h"


CPU_NAMESPACE_OPEN_SCOPE

CPUGeometry::CPUGeometry()
{
	// TODO: This is only here for testing reasons
	m_bsdf = std::make_unique<CPUSmoothDielectric>(LookupIOR("amber"), LookupIOR("air"), Col3f(0.9f, 0.6f, 0.7f), Col3f(0.9f, 0.9f, 0.9f));
	m_bsdf.reset(nullptr);
	// m_bsdf = std::make_unique<CPUSmoothConductor>(1.5, 0.4, Col3f(0.9f, 0.6f, 0.7f));
	m_bsdf = std::make_unique<CPUSmoothDiffuse>(Col3f(1.f, 1.f, 1.f));
}

CPUGeometry::~CPUGeometry()
{

}

bool CPUGeometry::Create(const RTCDevice& device,
		const RTCScene& topScene)
{
	CreatePrototype(device);

	rtcCommitGeometry(_geom);
	rtcReleaseGeometry(_geom);

	rtcCommitScene(_scene);

	_geomInstance = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_INSTANCE);
	_geomInstanceID = rtcAttachGeometry(topScene, _geomInstance);

	rtcSetGeometryInstancedScene(_geomInstance, _scene);
	rtcSetGeometryTimeStepCount(_geomInstance, 1);

	rtcSetGeometryTransform(_geomInstance,
			0,
			RTC_FORMAT_FLOAT3X4_COLUMN_MAJOR,
			(float*)&_transform);

	rtcCommitGeometry(_geomInstance);
	rtcReleaseGeometry(_geomInstance);

	return true;
}

void CPUGeometry::ComputeInstanceSurfaceInteraction(SurfaceInteraction& si, const Ray& ray) const
{
	si.p = xfmPoint(GetTransform(), si.p);

	si.n = xfmVector(GetTransform(), si.n);
	si.n = normalize(si.n);

	si.shadingFrame.vz = xfmVector(GetTransform(), si.shadingFrame.vz);
	si.shadingFrame.vz = normalize(si.shadingFrame.vz);

	si.instID = _geomInstanceID;

	si.instance = this;
}


CPU_NAMESPACE_CLOSE_SCOPE
