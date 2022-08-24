#ifndef USD_CAMERA_TRANSLATOR_H
#define USD_CAMERA_TRANSLATOR_H

#include <string>
#include <mutex>
#include <vector>
#include <unordered_map>

#include "../../spindulysFrontend.h"

#include "usdTranslator.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

class UsdCameraTranslator final : public UsdTranslator
{
	public:
		UsdCameraTranslator() = default;
		virtual ~UsdCameraTranslator() = default;

		virtual void* GetObjectFromPrim(const pxr::UsdPrim& prim) override;

	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // USD_TRANSLATOR_H
