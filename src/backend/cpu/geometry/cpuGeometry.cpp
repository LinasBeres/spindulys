#include "cpuGeometry.h"

#include "../utils/interaction.h"

// TODO: We currently only have diffuse bsdfs,
// but shapes should be able to choose what kind they are.
#include "../bsdf/diffuse.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

CPUGeometry::CPUGeometry()
{
	// TODO: This is only here for testing reasons
	m_bsdf = std::make_unique<SmoothDiffuse>(Col3f(0.5f));
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


BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
