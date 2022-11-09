#ifndef USD_BASIS_CURVE_TRANSLATOR_H
#define USD_BASIS_CURVE_TRANSLATOR_H

#include <string>
#include <mutex>
#include <vector>
#include <unordered_map>

#include "../../spindulysBase.h"

#include "usdTranslator.h"

BASE_NAMESPACE_OPEN_SCOPE

class UsdBasisCurveTranslator final : public UsdTranslator
{
	public:
		UsdBasisCurveTranslator() = default;
		virtual ~UsdBasisCurveTranslator() = default;

		virtual void* GetObjectFromPrim(const pxr::UsdPrim& prim) override;

	private:
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // USD_BASIS_CURVE_TRANSLATOR_H
