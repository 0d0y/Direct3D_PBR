#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Mesh.h"
#include "Material.h"
#include "Materials.h"
#include "struct.h"
#include <map>

using namespace DirectX;

class Model;
class Node;
class RenderComponent;
class AnimationComponent;

class ModelLoader
{
private:
	static ModelLoader* instance;

	//ModelLoader(ID3D11Device* dev, ID3D11DeviceContext* devcon, HWND hwnd);
	ModelLoader();
	ModelLoader(const ModelLoader&) = delete;
	ModelLoader& operator=(const ModelLoader&) = delete;
	~ModelLoader();

public:
	static ModelLoader* GetInstance();
	void Initialize(ID3D11Device* dev, ID3D11DeviceContext* devcon, HWND hwnd);

	void Load(std::string filename, Actor* onwer);
	void Close();

private:
	ID3D11Device* dev_;
	ID3D11DeviceContext* devcon_;
	std::string directory_;
	std::vector<Material> textures_loaded_;
	std::map<std::string, std::shared_ptr<Node>> nodes_loaded_;
	HWND hwnd_;

	void processNode(aiNode* node, const aiScene* scene, std::shared_ptr<RenderComponent> owner, std::shared_ptr<Node> parentNode);
	void processAnmaition(const aiScene* scene, std::shared_ptr<AnimationComponent> owner);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Node> ownerNode);
	void connectBoneNode(std::shared_ptr<Node> node);
	std::vector<Material> processMaterial(aiMesh* mesh, const aiScene* scene, Mesh* _mesh);
	std::vector<Material> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene, Mesh* _mesh);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture);
};

#endif // !MODEL_LOADER_H