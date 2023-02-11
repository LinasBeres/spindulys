#ifndef CURVE_H
#define CURVE_H

#include <vector>

#include "../spindulysBase.h"

#include "geometry.h"

BASE_NAMESPACE_OPEN_SCOPE

class Curve : virtual public Geometry
{
	public:
		enum CurveTypes
		{
			FlatLinear = 0,
			RoundBezier,
			FlatBezier,
			NormalOrientatedBezier,
			RoundBSpline,
			FlatBSpline,
			NormalOrientatedBSpline,
		};

		Curve(Geometry::GeometryTypes type, const std::string& name = "DefaultCurve");

		// Get Methods
		const std::vector<Vec3f>& GetPoints()    const { return m_points;     }
		const std::vector<Vec3f>& GetNormals()   const { return m_normals;    }
		const std::vector<float>& GetWidths()    const { return m_widths;     }
		CurveTypes                GetCurveType() const { return m_curveType;  }

		// Set Methods
		bool SetPoints(const std::vector<Vec3f> points)   { return points != std::exchange(m_points, points);          }
		bool SetNormals(const std::vector<Vec3f> normals) { return normals != std::exchange(m_normals, normals);       }
		bool SetWidths(const std::vector<float> widths)   { return widths != std::exchange(m_widths, widths);          }
		bool SetCurveType(CurveTypes curveType)           { return curveType != std::exchange(m_curveType, curveType); }

		bool HasVertexNormals() const { return m_normals.size() != 0; }

	protected:
		std::vector<Vec3f> m_points;
		std::vector<Vec3f> m_normals;
		std::vector<float> m_widths;

		CurveTypes m_curveType = CurveTypes::FlatLinear;

	protected:
		Vec3f GetVertexPosition(int index) const { return m_points[index];  }
		Vec3f GetVertexNormal(int index)   const { return m_normals[index]; }

	private:

};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // CURVE_H
