#define TINYOBJLOADER_IMPLEMENTATION

#include "objSceneLoader.h"

#include "../../geometry/mesh.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

bool ObjSceneLoader::LoadScene(const std::string& filepath)
{
	// We assume that the incoming file is a valid obj file

	scene->AddFilePath(filepath);

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str(),
			NULL, /* triangulate = */ true);

	if (!ret)
		return false;

	std::vector<Vec3f> vertices;
	for (size_t i = 0; i < attrib.vertices.size(); i += 3)
		vertices.emplace_back(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]);

	for (size_t i = 0; i < shapes.size(); ++i)
		LoadMesh(vertices, shapes[i]);

	scene->CommitScene();

	return true;
}

// TODO:
// ----
// This code is very inefficient but it works so ¯\_(ツ)_/¯
void ObjSceneLoader::LoadMesh(const std::vector<Vec3f>& vertices, tinyobj::shape_t& shape)
{
	Mesh* mesh = new Mesh(Geometry::GeometryTypes::Mesh, shape.name);

	mesh->SetPoints(vertices);

	std::vector<int> indices;
	for (size_t i = 0; i < shape.mesh.indices.size(); ++i)
		indices.emplace_back(size_t(shape.mesh.indices[i].vertex_index));
	mesh->SetIndices(indices);

	mesh->SetMeshType(Mesh::MeshType::TriangleMesh);

	scene->CreateGeomerty(mesh);
}

FRONTEND_NAMESPACE_CLOSE_SCOPE
