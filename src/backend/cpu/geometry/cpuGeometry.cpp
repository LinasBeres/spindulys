#include "cpuGeometry.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

CPUGeometry::CPUGeometry()
{
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


BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
