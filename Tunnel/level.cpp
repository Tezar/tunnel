#include "level.h"


void Level::makeLevel(vector3df* pos){
    ISceneManager* smgr = device->getSceneManager();

	
	IMesh* cylinder = createMesh(50, 80, 30, video::SColor(0,255,200,100), false, 10);

	IMeshSceneNode* cylinderNode = smgr->addMeshSceneNode(cylinder);
	
	cylinderNode->setMaterialFlag(EMF_LIGHTING, false);

}; 




IMesh* Level::createMesh(f32 radius, f32 length, 
			u32 tesselation, const video::SColor& color, 
			bool closeTop, f32 oblique) const
{
	SMeshBuffer* buffer = new SMeshBuffer();

	const f32 recTesselation = core::reciprocal((f32)tesselation);
	const f32 recTesselationHalf = recTesselation * 0.5f;
	const f32 angleStep = (core::PI * 2.f ) * recTesselation;
	const f32 angleStepHalf = angleStep*0.5f;

	u32 i;
	video::S3DVertex v;
	v.Color = color;
	buffer->Vertices.reallocate(tesselation*4);
	buffer->Indices.reallocate(tesselation*2);
	f32 tcx = 0.f;

	for ( i = 0; i <= tesselation; ++i )
	{
		const f32 angle = angleStep * i;
		v.Pos.X = radius * cosf(angle);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle);
		v.Normal = v.Pos;
		v.Normal.normalize().invert();
		v.TCoords.X=tcx;
		v.TCoords.Y=0.f;
		buffer->Vertices.push_back(v);

		v.Pos.X += oblique;
		v.Pos.Y = length;
		v.Normal = v.Pos;
		v.Normal.normalize().invert();
		v.TCoords.Y=1.f;
		buffer->Vertices.push_back(v);

		v.Pos.X = radius * cosf(angle + angleStepHalf);
		v.Pos.Y = 0.f;
		v.Pos.Z = radius * sinf(angle + angleStepHalf);
		v.Normal = v.Pos;
		v.Normal.normalize().invert();
		v.TCoords.X=tcx+recTesselationHalf;
		v.TCoords.Y=0.f;
		buffer->Vertices.push_back(v);

		v.Pos.X += oblique;
		v.Pos.Y = length;
		v.Normal = v.Pos;
		v.Normal.normalize().invert();
		v.TCoords.Y=1.f;
		buffer->Vertices.push_back(v);
		tcx += recTesselation;
	}

	// indices for the main hull part
	const u32 nonWrappedSize = tesselation* 4;
	for (i=0; i != nonWrappedSize; i += 2)
	{
		buffer->Indices.push_back(i + 3);
		buffer->Indices.push_back(i + 1);
		buffer->Indices.push_back(i + 0);

		buffer->Indices.push_back(i + 3);
		buffer->Indices.push_back(i + 0);
		buffer->Indices.push_back(i + 2);
	}

	// two closing quads between end and start
	buffer->Indices.push_back(0);
	buffer->Indices.push_back(i + 0);
	buffer->Indices.push_back(i + 1);

	buffer->Indices.push_back(0);
	buffer->Indices.push_back(i + 1);
	buffer->Indices.push_back(1);


	buffer->recalculateBoundingBox();
	SMesh* mesh = new SMesh();
	mesh->addMeshBuffer(buffer);
	mesh->setHardwareMappingHint(EHM_STATIC);
	mesh->recalculateBoundingBox();
	buffer->drop();
	return mesh;

}