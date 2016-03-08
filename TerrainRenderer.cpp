#include "TerrainRenderer.h"

#include <GL/glew.h>
#include <cmath>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
,chunkMap(chunkSubdivisionLength * 2 - 1, chunkSubdivisionLength * 2 - 1, {0, false})
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
      
      chunkMap(j, i) = {buffer_id, false};
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
  int buffer_id = chunkMap(x, y).buffer_id;
  
  int rel_x = toRelative(x);
  int rel_y = toRelative(y);
  
  int subdivisions = getChunkSubdivision(rel_x, rel_y);
  
  int chunk_subdiv_size = getChunkSize1D(subdivisions);
  
  chunk_subdiv_size = std::min(chunk_subdiv_size, chunkSize);
  
  int chunk_abs_x = chunkCenterX + rel_x;
  int chunk_abs_y = chunkCenterY + rel_y;
  
  if(
        chunk_abs_x < 0 || chunk_abs_x >= heightMap.width() / chunkSize
    ||  chunk_abs_y < 0 || chunk_abs_y >= heightMap.height() / chunkSize
  ){
    chunkMap(x, y).valid = false;
    return;
  }
  
  int chunk_world_x = chunk_abs_x * chunkSize;
  int chunk_world_y = chunk_abs_y * chunkSize;
  
  int step_size = chunkSize / chunk_subdiv_size;
  
  Array2D<glm::fvec3> buffer_data(chunk_subdiv_size, chunk_subdiv_size, glm::vec3(0.f, 0.f, 0.f));
  
  for(int i = 0; i < chunk_subdiv_size; ++i){
    for(int j = 0; j < chunk_subdiv_size; ++j){
      int vert_x = j * step_size;
      int vert_y = i * step_size;
      
      int vert_world_x =  chunk_world_x + vert_x;
      int vert_world_y =  chunk_world_y + vert_y;
      
      float vert_height = heightMap(vert_world_x, vert_world_y) * terrainHeight;
      
      buffer_data(j, i) = glm::vec3((float)vert_x, vert_height, (float)vert_y);
    }
  }
  
  int chunk_area = getChunkSize2D(subdivisions);
  
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(
    GL_ARRAY_BUFFER, 
    sizeof(float) * 3 * chunk_area, 
    buffer_data.raw(), 
    GL_DYNAMIC_DRAW
  );
  
  chunkMap(x, y).valid = true;
}

void TerrainRenderer::render(GLuint modelMatrixUniform){
  for(int i = 0; i < chunkMap.height(); ++i){
    for(int j = 0; j < chunkMap.width(); ++j){
      if(!chunkMap(j, i).valid)
        continue;
      
      int rel_x = toRelative(j);
      int rel_y = toRelative(i);
      int chunk_vert_count = getChunkSize2D(getChunkSubdivision(rel_x, rel_y));
      
      glm::mat4 m_mat;
      m_mat = glm::translate(m_mat, glm::vec3(
        (chunkCenterX + rel_x) * chunkSize, 
        0.f, 
        (chunkCenterY + rel_y) * chunkSize
      ));
      glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, (GLfloat*)&m_mat);
      
      glPointSize(3.f);
      glBindBuffer(GL_ARRAY_BUFFER, chunkMap(j, i).buffer_id);
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

void TerrainRenderer::setObserverPosition(float x, float y){
  int chunk_x = (int)x / chunkSize;
  int chunk_y = (int)y / chunkSize;
  
  std::cout << chunk_x << ", " << chunk_y << std::endl;
  
  if(chunk_x != chunkCenterX || chunk_y != chunkCenterY){
    chunkCenterX = chunk_x;
    chunkCenterY = chunk_y;
  
    loadAllChunks();
  }
}