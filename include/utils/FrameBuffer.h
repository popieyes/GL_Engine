#pragma once
#include <iostream>
#include <glad/glad.h>

class FrameBuffer {
  public:
    FrameBuffer(int width, int height);
    ~FrameBuffer();

    unsigned int ID;
    unsigned int RBO;
    const unsigned int GetColorTextureID() const { return textureColorBuffer; }
    
  private:
    unsigned int textureColorBuffer;
  };