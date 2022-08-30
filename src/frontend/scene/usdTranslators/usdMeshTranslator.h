#ifndef USD_MESH_TRANSLATOR_H
#define USD_MESH_TRANSLATOR_H

#include <string>
#include <mutex>
#include <vector>
#include <unordered_map>

#include "../../spindulysFrontend.h"

#include "usdTranslator.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

class UsdMeshTranslator final : public UsdTranslator
{
	public:
		UsdMeshTranslator() = default;
		virtual ~UsdMeshTranslator() = default;

		virtual void* GetObjectFromPrim(const pxr::UsdPrim& prim) override;

	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // USD_TRANSLATOR_H
