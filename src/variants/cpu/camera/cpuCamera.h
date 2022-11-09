#ifndef CPU_CAMERA_H
#define CPU_CAMERA_H

#include "../spindulysCPU.h"

#include <camera/camera.h>

#include "../utils/ray.h"


CPU_NAMESPACE_OPEN_SCOPE

class CPUCamera final : public Camera
{
	public:
		CPUCamera(const std::string& name);
		~CPUCamera() = default;
};

CPU_NAMESPACE_CLOSE_SCOPE

#endif //CPU_CAMERA_H
