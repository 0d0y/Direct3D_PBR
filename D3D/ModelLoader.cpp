#include "../Engine/Actor.h"
#include "../Engine/RenderComponent.h"
#include "../Engine/AnimationComponent.h"
#include "Animation.h"
#include "directxtk/WICTextureLoader.h"
#include "ModelLoader.h"
#include "Node.h"
#include "NodeAnimation.h"
#include <set>
#include <stdexcept>

ModelLoader* ModelLoader::instance = nullptr;


//ModelLoader::ModelLoader(ID3D11Device* dev, ID3D11DeviceContext* devcon, HWND hwnd) :
//	dev_(dev),
//	devcon_(devcon),
//	directory_(),
//	hwnd_(hwnd) {
//	// empty
//}

ModelLoader::ModelLoader() :
	dev_(nullptr),
	devcon_(nullptr),
	directory_(),
	hwnd_(nullptr)
{
	// empty
}

ModelLoader::~ModelLoader() {
	// empty
}

ModelLoader* ModelLoader::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new ModelLoader();
	}
	return instance;
}

void ModelLoader::Initialize(ID3D11Device* dev, ID3D11DeviceContext* devcon, HWND hwnd)
{
	dev_ = dev;
	devcon_ = devcon;
	hwnd_ = hwnd;
}

/// Load > processNode > processMesh > loadMaterialTextures > loadEmbeddedTexture
void ModelLoader::Load(std::string filename, Actor* owner)
{
	/// Importer
	///	: Open Asset Import Library에 대한 인터페이스
	///	  파일을 import하기 위해 해당 클래스의 객체를 통해 ReadFile()을 호출한다.
	///	  import한 데이터는 Imprter 객체와 함께 파괴된다.
	///   Importer 오브젝트를 생성하는 것은 꽤 많은 재시도를 수반할 수 있으므로 가능한 여러 번 재사용하는 편이 좋다.
	///   기본적으로 Read-only로 파일에 접근하지만, 수정하고 싶다면 커스텀 IOSystem, Stream을 통해 수정 가능하다.
	///	  Thread-safe하지 않으므로 멀티스레드 환경에서 사용한다면 스레드마다 각자의 Importer Instance를 유지해야 한다.
	Assimp::Importer importer;

	/// aiScene
	///	: import된 데이터에 대한 상위 구조체. import된 모든 데이터는 aiScene 구조체를 통해서 접근된다.
	///	  해당 클래스의 오브젝트는 호출자가 아닌 Assimp에 의해서 유지되므로 이를 인스턴스화하거나 생성할 수 없다.
	///	  aiNode, aiMesh, aiMaterial, aiAnimation, aiTexture, aiLight, aiCamera, aiMetadata, aiString, aiSekleton
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);    // $assimp_fbx$ 노드 생성안함

	const aiScene* pScene = importer.ReadFile(filename,
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_LimitBoneWeights |
		aiProcess_ConvertToLeftHanded
	);


	this->directory_ = filename.substr(0, filename.find_last_of("/\\"));

	auto renderComponent = owner->GetComponent<RenderComponent>();
	auto animationComponent = owner->GetComponent<AnimationComponent>();
	auto ownerRootNode = renderComponent->GetRootNode();

	processNode(pScene->mRootNode, pScene, renderComponent, ownerRootNode);
	processAnmaition(pScene, animationComponent);
	auto ownerRootNode1 = renderComponent->GetRootNode();
	connectBoneNode(ownerRootNode1);

	textures_loaded_.clear();
	nodes_loaded_.clear();
}

/// 한 개의 aiMesh에 대해 Mesh로 대입할 정보 생성
Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<Node> ownerNode) {
	// Data to fill
	std::vector<VERTEX> vertices;
	std::vector<UINT> indices;

	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		VERTEX vertex;

		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.TextureCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.TextureCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		if (mesh->mNormals)
		{
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
		}

		if (mesh->mTangents)
		{
			vertex.Tangent.x = mesh->mTangents[i].x;
			vertex.Tangent.y = mesh->mTangents[i].y;
			vertex.Tangent.z = mesh->mTangents[i].z;
		}

		if (mesh->mBitangents)
		{
			vertex.BiTangent.x = mesh->mBitangents[i].x;
			vertex.BiTangent.y = mesh->mBitangents[i].y;
			vertex.BiTangent.z = mesh->mBitangents[i].z;
		}

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		/// aiFace
		///	: Mesh를 구성하는 vertex의 인덱스 집합을 나타내는 구조체
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//Bone
	UINT boneIndexCounter = 0;
	std::vector<BoneInfo> boneReferences(mesh->mNumBones);
	std::map<std::string, int> boneMap;

	for (UINT i = 0; i < mesh->mNumBones; ++i)
	{
		aiBone* bone = mesh->mBones[i];
		std::string boneName(mesh->mBones[i]->mName.C_Str());
		boneReferences[i].BoneName = boneName;
		boneReferences[i].BoneOffset = Matrix(&bone->mOffsetMatrix.a1).Transpose();

		for (UINT j = 0; j < bone->mNumWeights; j++)
		{
			UINT vertexID = bone->mWeights[j].mVertexId;
			float weight = bone->mWeights[j].mWeight;
			vertices[vertexID].AddBoneData(i, weight);
		}
	}

	return Mesh(dev_, vertices, indices, boneReferences);
}

void ModelLoader::connectBoneNode(std::shared_ptr<Node> node)
{
	for (auto& mesh : node->GetMeshes())
	{
		for (auto& bone : mesh.GetBoneReferences())
		{
			auto find = nodes_loaded_.find(bone.BoneName);

			if (find != nodes_loaded_.end())
			{
				bone.Onwer = find->second;
			}
		}
	}

	for (auto& child : node->GetNodes())
	{
		connectBoneNode(child);
	}
}


std::vector<Material> ModelLoader::processMaterial(aiMesh* mesh, const aiScene* scene, Mesh* _mesh)
{
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		//맵 디버깅용
		std::vector<size_t> textureCounts(21);
		for (size_t i = 0; i < 21; ++i)
		{
			textureCounts[i] = material->GetTextureCount(static_cast<aiTextureType>(i));
		}

		aiTextureType textureTypes[] =
		{
		aiTextureType_DIFFUSE,
		aiTextureType_NORMALS,
		aiTextureType_SPECULAR,
		aiTextureType_OPACITY,
		aiTextureType_EMISSIVE,
		aiTextureType_SHININESS,
		aiTextureType_METALNESS
		};

		std::vector<Material> Textures;

		for (aiTextureType textureType : textureTypes)
		{
			std::vector<Material> loadedTextures;
			loadedTextures = loadMaterialTextures(material, textureType, scene, _mesh);
			Textures.insert(Textures.end(), loadedTextures.begin(), loadedTextures.end());
		}
		return Textures;
	}
}

/// 한 개의 Material이 가진 해당 타입의 텍스처들을 로드
std::vector<Material> ModelLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene, Mesh* _mesh)
{
	std::vector<Material> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);	// 텍스처 경로 받아오기

		// 이미 로드된 텍스처라면, Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (UINT j = 0; j < textures_loaded_.size(); j++)
		{
			if (std::strcmp(textures_loaded_[j].GetPath().c_str(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded_[j]);
				textures.back().GetType() = type;
				_mesh->GetMaterials().SetHasMap(type, static_cast<int>(true));
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)// 로드되지 않은 텍스처라면, If texture hasn't been loaded already, load it
		{
			HRESULT hr;
			Material texture;

			/// embeddedTexture : 3D모델 파일에 포함된 텍스처
			/// 모델의 용량을 줄이고, 텍스처 재사용 및 교체를 위해 일반적으로 모델과 텍스처를 분리하여 관리한다.
			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
			if (embeddedTexture != nullptr)
			{
				texture.GetTexture() = loadEmbeddedTexture(embeddedTexture);
			}
			else // 텍스처가 별도의 파일로 존재하는 경우
			{
				std::string filename = std::string(str.C_Str());
				if (int at = filename.find_last_of(".tga"))
				{
					filename = filename.substr(0, at - 2) + "png";
				}
				if (std::string::npos != filename.find_last_of("/\\"))
				{
					filename = "../Textures/" + filename.substr(filename.find_last_of("/\\"));
				}
				else
				{
					filename = "../Textures/" + filename;
				}
				//filename = directory_ + '/' + filename;
				std::wstring filenamews = std::wstring(filename.begin(), filename.end());
				hr = CreateWICTextureFromFile(dev_, devcon_, filenamews.c_str(), nullptr, &texture.GetTexture());
				if (FAILED(hr))
					MessageBox(hwnd_, L"Texture couldn't be loaded", L"Error!", MB_ICONERROR | MB_OK);
			}
			texture.GetType() = type;
			_mesh->GetMaterials().SetHasMap(type, static_cast<int>(true));
			texture.GetPath() = str.C_Str();
			textures.push_back(texture);
			this->textures_loaded_.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

void ModelLoader::Close() {
	//for (auto& t : textures_loaded_)
	//	t.Release();

	//for (size_t i = 0; i < meshes_.size(); i++) {
	//	meshes_[i].Close();
	//}
}

/// 한 개의 노드에 대해 processMesh()를 호출하여 Mesh단위로 처리하고 meshes_ 벡터를 채운다.
void ModelLoader::processNode(aiNode* node, const aiScene* scene, std::shared_ptr<RenderComponent> owner, std::shared_ptr<Node> parentNode) {
	/// aiNode를 통해 aiScene을 탐색한다
	///	각 Node는 원본 모델을 나눈 단위이다.
	// 현재 Node의 Mesh index를 통해 전체 모델에서의 Mesh에 접근하고,
	// 해당 Mesh를 처리하여 ModelLoader의 mesh 벡터에 넣는다.
	std::shared_ptr<Node> custumNode = std::make_shared<Node>();
	std::string nodeName = node->mName.C_Str();
	custumNode->SetName(nodeName);
	custumNode->SetLocalTransform(Matrix(&node->mTransformation.a1).Transpose());

	nodes_loaded_.insert({ nodeName, custumNode });

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh _mesh = this->processMesh(mesh, scene, custumNode);
		_mesh.GetMaterials().GetMaterial() = this->processMaterial(mesh, scene, &_mesh);
		_mesh.GetMaterials().UpdateHasMapConstantBuffer(devcon_);
		custumNode->GetMeshes().push_back(_mesh);
	}

	if (node == scene->mRootNode)
	{
		owner->SetRootNode(custumNode);
	}
	else
	{
		custumNode->SetParentNode(parentNode);
		parentNode->GetNodes().push_back(custumNode);
	}

	// 현재 Node의 자식 Node에 대해서도 동일한 Mesh처리를 해준다.
	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene, owner, custumNode);
	}
}

void ModelLoader::processAnmaition(const aiScene* scene, std::shared_ptr<AnimationComponent> owner)
{
	for (UINT i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* animationInfo = scene->mAnimations[i];

		auto animation = std::make_shared<Animation>();

		animation->SetAnimationDuration(static_cast<float>(animationInfo->mDuration / animationInfo->mTicksPerSecond));
		animation->SetName(animationInfo->mName.C_Str());

		for (UINT j = 0; j < animationInfo->mNumChannels; j++)
		{
			aiNodeAnim* channel = animationInfo->mChannels[j];

			std::shared_ptr<NodeAnimation> nodeAnimation = std::make_shared<NodeAnimation>();
			nodeAnimation->SetNodeName(channel->mNodeName.C_Str());

			for (UINT k = 0; k < channel->mNumPositionKeys; k++)
			{
				Keyframe keyframe;
				keyframe.time = static_cast<float>(channel->mPositionKeys[k].mTime / animationInfo->mTicksPerSecond);
				keyframe.position = Vector3(
					channel->mPositionKeys[k].mValue.x,
					channel->mPositionKeys[k].mValue.y,
					channel->mPositionKeys[k].mValue.z
				);

				keyframe.rotation = Quaternion(
					channel->mRotationKeys[k].mValue.x,
					channel->mRotationKeys[k].mValue.y,
					channel->mRotationKeys[k].mValue.z,
					channel->mRotationKeys[k].mValue.w
				);

				keyframe.scale = Vector3(
					channel->mScalingKeys[k].mValue.x,
					channel->mScalingKeys[k].mValue.y,
					channel->mScalingKeys[k].mValue.z
				);

				nodeAnimation->AddKeyframe(keyframe);
			}

			animation->AddNodeAnimation(nodeAnimation);
		}
		owner->AddAnimation(animation);
	}
}
/// 3D 모델에 포함된 텍스처 로드, aiTexture > custom Texture로 매핑
ID3D11ShaderResourceView* ModelLoader::loadEmbeddedTexture(const aiTexture* embeddedTexture) {
	HRESULT hr;
	ID3D11ShaderResourceView* texture = nullptr;

	if (embeddedTexture->mHeight != 0) {	// 압축되지 않은 이미지라면
		// Load an uncompressed ARGB8888 embedded texture
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = embeddedTexture->mWidth;
		desc.Height = embeddedTexture->mHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subresourceData;
		subresourceData.pSysMem = embeddedTexture->pcData;
		subresourceData.SysMemPitch = embeddedTexture->mWidth * 4;
		subresourceData.SysMemSlicePitch = embeddedTexture->mWidth * embeddedTexture->mHeight * 4;

		ID3D11Texture2D* texture2D = nullptr;
		hr = dev_->CreateTexture2D(&desc, &subresourceData, &texture2D);
		if (FAILED(hr))
			MessageBox(hwnd_, L"CreateTexture2D failed!", L"Error!", MB_ICONERROR | MB_OK);

		hr = dev_->CreateShaderResourceView(texture2D, nullptr, &texture);
		if (FAILED(hr))
			MessageBox(hwnd_, L"CreateShaderResourceView failed!", L"Error!", MB_ICONERROR | MB_OK);

		return texture;
	}

	// 압축된 이미지라면,
	// mHeight is 0, so try to load a compressed texture of mWidth bytes
	const size_t size = embeddedTexture->mWidth;

	hr = CreateWICTextureFromMemory(dev_, devcon_, reinterpret_cast<const unsigned char*>(embeddedTexture->pcData), size, nullptr, &texture);
	if (FAILED(hr))
		MessageBox(hwnd_, L"Texture couldn't be created from memory!", L"Error!", MB_ICONERROR | MB_OK);

	return texture;
}
