#include "TerrainRenderer.h"

#include <GL/glew.h>
#include <cmath>
#include <stdexcept>
#include <glm/glm.hpp>
#include <iostream>

TerrainRenderer::TerrainRenderer(
	int width, 
	int height, 
  float terrainHeight,
	int chunkSize,
	int* chunkSubdivision,
	int chunkSubdivisionLength
)
:chunkSize(chunkSize)
,chunkSubdivision(chunkSubdivision)
,chunkSubdivisionLength(chunkSubdivisionLength)
,chunkCenterX(0)
,chunkCenterY(0)
,terrainHeight(terrainHeight)
,heightMap(width, height, -1.f)
,chunkMap(chunkSubdivisionLength * 2 - 1, chunkSubdivisionLength * 2 - 1, -1)
{
  // GET GPU BUFFERS  
  for(int i = 0; i < chunkMap.height(); ++i){
    for(int j = 0; j < chunkMap.width(); ++j){
      int rel_x = toRelative(j);
      int rel_y = toRelative(i);
      
      int vertex_count = getChunkSize2D(getChunkSubdivision(rel_x, rel_y));
      
      GLuint buffer_id;
      glGenBuffers(1, &buffer_id);
      glBufferData(buffer_id, sizeof(float) * 3 * vertex_count, NULL, GL_DYNAMIC_DRAW);
      
      chunkMap(j, i) = buffer_id;
    }
  }
}

TerrainRenderer::~TerrainRenderer(){
  
}

int TerrainRenderer::getChunkSubdivision(int x, int y){
  int dist = std::max(abs(x), abs(y));
  if(dist >= chunkSubdivisionLength)
    throw std::runtime_error("Invalid chunk!");
  
  return chunkSubdivision[dist];
}

int TerrainRenderer::getChunkDrawDistance(){
  return chunkSubdivisionLength;
}

int TerrainRenderer::getChunkSize1D(int subdivisions){
  return pow(2, subdivisions + 1);
}

int TerrainRenderer::getChunkSize2D(int subdivisions){
  int chunk_size_1d = getChunkSize1D(subdivisions);
  return chunk_size_1d * chunk_size_1d;
}


int TerrainRenderer::setChunkCenter(int x, int y){
  chunkCenterX = x;
  chunkCenterY = y;
  
  // TODO: Shift chunks relative to old location and invalidate
  // chunks that need to be sent at a different resolution 
   
}

int TerrainRenderer::toRelative(int v){
  return v - (getChunkDrawDistance() - 1);
}

int TerrainRenderer::toAbsolute(int v){
  return v + (getChunkDrawDistance() - 1);
}

void TerrainRenderer::loadChunk(int x, int y){  
  int buffer_id = chunkMap(x, y);
  
  int rel_x = toRelative(x);
  int rel_y = toRelative(y);
  
  int subdivisions = getChunkSubdivision(rel_x, rel_y);
  
  int chunk_subdiv_size = getChunkSize1D(subdivisions);
  
  chunk_subdiv_size = std::min(chunk_subdiv_size, chunkSize);
  
  int chunk_area = getChunkSize2D(subdivisions);
  
  int chunk_abs_x = chunkCenterX + rel_x;
  int chunk_abs_y = chunkCenterY + rel_y;
  
  if(
        chunk_abs_x < 0 || chunk_abs_x >= heightMap.width() / chunkSize
    ||  chunk_abs_y < 0 || chunk_abs_y >= heightMap.height() / chunkSize
  ){
    return;
    //TODO: flag buffer as non-draw
  }
  
  int chunk_world_x = chunk_abs_x * chunkSize;
  int chunk_world_y = chunk_abs_y * chunkSize;
  
  int step_size = chunkSize / chunk_subdiv_size;
  
  Array2D<glm::fvec3> buffer_data(chunk_subdiv_size, chunk_subdiv_size, glm::vec3(0.f, 0.f, 0.f));
  
  for(int i = 0; i < chunk_subdiv_size; ++i){
    for(int j = 0; j < chunk_subdiv_size; ++j){
      int vert_x = chunk_world_x + j * step_size;
      int vert_y = chunk_world_y + i * step_size;
      
      float vert_height = heightMap(vert_x, vert_y) * terrainHeight;
      
      buffer_data(j, i) = glm::vec3((float)vert_x, vert_height, (float)vert_y);
    }
  }
  
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(
    GL_ARRAY_BUFFER, 
    sizeof(float) * 3 * chunk_area, 
    buffer_data.raw(), 
    GL_DYNAMIC_DRAW
  );
}

void TerrainRenderer::render(){
  for(int i = 0; i < chunkMap.height(); ++i){
    for(int j = 0; j < chunkMap.width(); ++j){
      int rel_x = toRelative(j);
      int rel_y = toRelative(i);
      int chunk_vert_count = getChunkSize2D(getChunkSubdivision(rel_x, rel_y));
      
      glPointSize(3.f);
      glBindBuffer(GL_ARRAY_BUFFER, chunkMap(j, i));
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
      glEnableVertexAttribArray(0);
      glDrawArrays(GL_POINTS, 0, chunk_vert_count);
      glDisableVertexAttribArray(0);
    }
  }
}

void TerrainRenderer::loadAllChunks(){
  for(int i = 0; i < chunkMap.height(); ++i){
    for(int j = 0; j < chunkMap.width(); ++j){
      loadChunk(j, i);
    }
  }
}

void TerrainRenderer::setHeightmap(const Array2D<float>& heightmap){
  this->heightMap = heightmap;
}

void TerrainRenderer::setHeightmap(Array2D<float>&& heightmap){
  this->heightMap = heightmap;
}