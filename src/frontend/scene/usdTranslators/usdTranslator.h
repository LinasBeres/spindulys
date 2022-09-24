#ifndef USD_TRANSLATOR_H
#define USD_TRANSLATOR_H

#include <string>
#include <mutex>
#include <vector>
#include <unordered_map>

#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usdGeom/xformCache.h>

#include <pxr/base/tf/token.h>

#include "../../spindulysFrontend.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

class UsdTranslator
{
	public:
		UsdTranslator() = default;
		virtual ~UsdTranslator() = default;

		virtual void* GetObjectFromPrim(const pxr::UsdPrim& prim) = 0;

		template<typename T>
		static bool GetAttribute(const pxr::UsdPrim& prim, const pxr::TfToken& token, T* value)
		{
			if (pxr::UsdAttribute attr = prim.GetAttribute(token))
			{
				if (attr.Get(value))
					return true;
				return false;
			}
			return false;
		}

	protected:

	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // USD_TRANSLATOR_H
