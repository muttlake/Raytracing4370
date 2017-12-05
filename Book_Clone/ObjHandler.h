#ifndef	_OBJLOADER_H
#define _OBJLOADER_H

#include <glm_dir/glm/glm.hpp>
//#include <glm_dir/glm/gtx/transform.hpp>
#include <glm_dir/glm/gtc/type_ptr.hpp>
#include "OBJ-Loader/Source/OBJ_Loader.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>


class ObjHandler
{
	public:
	ObjHandler();
	void printObjContents(objl::Loader loader);
	uint* buildIndicesListFromObj(objl::Loader loader);
	glm::vec3* buildPositionsVec3sFromObj(objl::Loader loader);
	glm::vec3* buildNormalsVec3sFromObj(objl::Loader loader);
	glm::vec2* buildTexCoordVec2sFromObj(objl::Loader loader);
	void printAllPositions(glm::vec3* allPositions, int numVertices, std::ofstream& outFile);
	void printAllNormals(glm::vec3* allNormals, int numVertices, std::ofstream& outFile);
	void printAllIndices(uint* triangleIndices, int numIndices, std::ofstream& outFile);
	void printAllTexCoords(glm::vec2* allTexCoords, int numVertices, std::ofstream& outFile);
};

ObjHandler::ObjHandler()
{

}


void ObjHandler::printObjContents(objl::Loader loader) {
	// Create/Open e1Out.txt
	std::ofstream file("e1Out.txt");

	// Go through each loaded mesh and out its contents
	for (int i = 0; i < loader.LoadedMeshes.size(); i++)
	{
		// Copy one of the loaded meshes to be our current mesh
		objl::Mesh curMesh = loader.LoadedMeshes[i];

		// Print Mesh Name
		file << "Mesh " << i << ": " << curMesh.MeshName << "\n";

		// Print Vertices
		file << "Vertices:\n";

		// Go through each vertex and print its number,
		//  position, normal, and texture coordinate
		for (int j = 0; j < curMesh.Vertices.size(); j++)
		{
			file << "V" << j << ": " <<
				"P(" << curMesh.Vertices[j].Position.X << ", " << curMesh.Vertices[j].Position.Y << ", " << curMesh.Vertices[j].Position.Z << ") " <<
				"N(" << curMesh.Vertices[j].Normal.X << ", " << curMesh.Vertices[j].Normal.Y << ", " << curMesh.Vertices[j].Normal.Z << ") " <<
				"TC(" << curMesh.Vertices[j].TextureCoordinate.X << ", " << curMesh.Vertices[j].TextureCoordinate.Y << ")\n";
		}

		// Print Indices
		file << "Indices:\n";

		// Go through every 3rd index and print the
		//      triangle that these indices represent
		for (int j = 0; j < curMesh.Indices.size(); j += 3)
		{
			file << "T" << j / 3 << ": " << curMesh.Indices[j] << ", " << curMesh.Indices[j + 1] << ", " << curMesh.Indices[j + 2] << "\n";
		}

		// Print Material
		file << "Material: " << curMesh.MeshMaterial.name << "\n";
		file << "Ambient Color: " << curMesh.MeshMaterial.Ka.X << ", " << curMesh.MeshMaterial.Ka.Y << ", " << curMesh.MeshMaterial.Ka.Z << "\n";
		file << "Diffuse Color: " << curMesh.MeshMaterial.Kd.X << ", " << curMesh.MeshMaterial.Kd.Y << ", " << curMesh.MeshMaterial.Kd.Z << "\n";
		file << "Specular Color: " << curMesh.MeshMaterial.Ks.X << ", " << curMesh.MeshMaterial.Ks.Y << ", " << curMesh.MeshMaterial.Ks.Z << "\n";
		file << "Specular Exponent: " << curMesh.MeshMaterial.Ns << "\n";
		file << "Optical Density: " << curMesh.MeshMaterial.Ni << "\n";
		file << "Dissolve: " << curMesh.MeshMaterial.d << "\n";
		file << "Illumination: " << curMesh.MeshMaterial.illum << "\n";
		file << "Ambient Texture Map: " << curMesh.MeshMaterial.map_Ka << "\n";
		file << "Diffuse Texture Map: " << curMesh.MeshMaterial.map_Kd << "\n";
		file << "Specular Texture Map: " << curMesh.MeshMaterial.map_Ks << "\n";
		file << "Alpha Texture Map: " << curMesh.MeshMaterial.map_d << "\n";
		file << "Bump Map: " << curMesh.MeshMaterial.map_bump << "\n";

		// Leave a space to separate from the next mesh
		file << "\n";
	}

	// Close File
	file.close();
}

uint* ObjHandler::buildIndicesListFromObj(objl::Loader loader)
{
	uint* triangleIndicesList;
	objl::Mesh curMesh = loader.LoadedMeshes[0];
	triangleIndicesList = new uint[curMesh.Indices.size()];
	//      triangle that these indices represent
	for (int j = 0; j < curMesh.Indices.size(); j++)
	{
		triangleIndicesList[j] = curMesh.Indices[j];
	}
	return triangleIndicesList;
}

glm::vec3* ObjHandler::buildPositionsVec3sFromObj(objl::Loader loader)
{
	glm::vec3* allPositions;
	objl::Mesh curMesh = loader.LoadedMeshes[0];
	allPositions = new glm::vec3[curMesh.Vertices.size()];
	for (int j = 0; j < curMesh.Vertices.size(); j++)
	{
		allPositions[j] = glm::vec3(curMesh.Vertices[j].Position.X, curMesh.Vertices[j].Position.Y, curMesh.Vertices[j].Position.Z);
	}
	return allPositions;
}

glm::vec3* ObjHandler::buildNormalsVec3sFromObj(objl::Loader loader)
{
	glm::vec3* allNormals;
	objl::Mesh curMesh = loader.LoadedMeshes[0];
	allNormals = new glm::vec3[curMesh.Vertices.size()];
	for (int j = 0; j < curMesh.Vertices.size(); j++)
	{
		allNormals[j] = glm::vec3(curMesh.Vertices[j].Normal.X, curMesh.Vertices[j].Normal.Y, curMesh.Vertices[j].Normal.Z);
	}
	return allNormals;
}

glm::vec2* ObjHandler::buildTexCoordVec2sFromObj(objl::Loader loader)
{
	glm::vec2* allTexCoords;
	objl::Mesh curMesh = loader.LoadedMeshes[0];
	allTexCoords = new glm::vec2[curMesh.Vertices.size()];
	for (int j = 0; j < curMesh.Vertices.size(); j++)
	{
		allTexCoords[j] = glm::vec2(curMesh.Vertices[j].TextureCoordinate.X, curMesh.Vertices[j].TextureCoordinate.Y);
	}
	return allTexCoords;
}



void ObjHandler::printAllPositions(glm::vec3* allPositions, int numVertices, std::ofstream& outFile)
{
        outFile << "Printing all position vec3:\n";
        for (int i = 0; i < numVertices; i++)
        {
                if (i % 3 == 0)
                {
                        outFile << "Triangle: " << int(i / 3 + 0.5) << "\n";
                }
                outFile << "Position For Vertex " << i << ": ";
                outFile << allPositions[i][0] << " " << allPositions[i][1];
                outFile << " " << allPositions[i][2] << std::endl;
        }
}


void ObjHandler::printAllNormals(glm::vec3* allNormals, int numVertices, std::ofstream& outFile)
{
        outFile << "Printing all normals vec3:\n";
        for (int i = 0; i < numVertices; i++)
        {
                if (i % 3 == 0)
                {
                        outFile << "Triangle: " << int(i / 3 + 0.5) << "\n";
                }
                outFile << "Normal For Vertex " << i << ": ";
                outFile << allNormals[i][0] << " " << allNormals[i][1];
                outFile << " " << allNormals[i][2] << std::endl;
        }
}

void ObjHandler::printAllIndices(uint* triangleIndices, int numIndices, std::ofstream& outFile)
{
        outFile << "Printing all indices.\n";
        for (int i = 0; i < numIndices; i++)
        {
                outFile << triangleIndices[i] << "\n";
        }
}


void ObjHandler::printAllTexCoords(glm::vec2* allTexCoords, int numVertices, std::ofstream& outFile)
{
	outFile << "Printing all texture coordinates vec2:\n";
	for (int i = 0; i < numVertices; i++)
	{
		outFile << "Texture Coordinate For Vertex " << i << ": ";
		outFile << allTexCoords[i][0] << " " << allTexCoords[i][1];
		outFile << std::endl;
	}
}




	//Read obj file
	//objl::Loader loader;
	//loader.LoadFile("teapot2.obj");
	//printf("\nLoaded teapot2.obj\n\n");
	//printObjContents(loader);

	//int numIndices = loader.LoadedMeshes[0].Indices.size();
	//uint* vindices;
	//vindices = buildIndicesListFromObj(loader);
	//printAllIndices(vindices, numVertices);

	//int numVertices = loader.LoadedMeshes[0].Vertices.size();
	//glm::vec3* vpositions;
	//vpositions = buildPositionsVec3sFromObj(loader);
	//printAllPositions(vpositions, numVertices);

	//printf("--------------------------------------------------------------\n");
	//std::cout << "The real number of indices is: " << numIndices << "\n";
	//std::cout << "The real number of vertices is: " << numVertices << "\n";
	//printf("--------------------------------------------------------------\n");

#endif
