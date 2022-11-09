#ifndef CURVE_H
#define CURVE_H

#include <vector>

#include "../spindulysFrontend.h"

#include "geometry.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

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
		const std::vector<Vec3f>& GetPoints()    const { return _points;     }
		const std::vector<Vec3f>& GetNormals()   const { return _normals;    }
		const std::vector<float>& GetWidths()    const { return _widths;     }
		CurveTypes                GetCurveType() const { return _curveType;  }

		// Set Methods
		bool SetPoints(const std::vector<Vec3f> points)   { return points != std::exchange(_points, points);          }
		bool SetNormals(const std::vector<Vec3f> normals) { return normals != std::exchange(_normals, normals);       }
		bool SetWidths(const std::vector<float> widths)   { return widths != std::exchange(_widths, widths);          }
		bool SetCurveType(CurveTypes curveType)           { return curveType != std::exchange(_curveType, curveType); }

		bool HasVertexNormals() const { return _normals.size() != 0; }

	protected:
		std::vector<Vec3f> _points;
		std::vector<Vec3f> _normals;
		std::vector<float> _widths;

		CurveTypes _curveType = CurveTypes::FlatLinear;

	protected:
		Vec3f GetVertexPosition(int index) const { return _points[index];  }
		Vec3f GetVertexNormal(int index)   const { return _normals[index]; }

	private:

};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // CURVE_H
