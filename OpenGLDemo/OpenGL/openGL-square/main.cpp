//
//  main.cpp
//  openGL-square
//
//  Created by 程倩的MacBook on 2020/11/19.
//

#include <stdio.h>
#include "GLShaderManager.h"
#include "GLTools.h"
#include <GLUT/GLUT.h>

// 定义一个着色管理器
GLShaderManager shaderManager;

GLBatch triangleBatch;

GLfloat blockSize = 0.1f;

// 正方形的四个顶点
GLfloat vVerts[] = {
    -blockSize,-blockSize,0.0f,
    blockSize,-blockSize,0.0f,
    blockSize,blockSize,0.0f,
    -blockSize,blockSize,0.0f,
};

// 在窗口大小改变的时候，接收新的宽度和高度
void changeSize(int w,int h) {
    glViewport(0, 0, w, h);
}

void RenderScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // 设置一组浮点数来表示红色
    GLfloat vred[] = {1.0,0.0,0.0,1.0};
    
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vred);
    
    triangleBatch.Draw();
    
    glutSwapBuffers();
}

void SpecialKeys(int key,int x,int y) {
    GLfloat stepSize = 0.025f;
    GLfloat blockX = vVerts[0];
    GLfloat blockY = vVerts[10];
    
    printf("v[0] = %f\n", blockX);
    printf("v[10] = %f\n", blockY);
    if (key == GLUT_KEY_UP) {
        blockY += stepSize;
    }
    
    if (key == GLUT_KEY_DOWN) {
        blockY -= stepSize;
    }
    
    if (key == GLUT_KEY_LEFT) {
        blockX -= stepSize;
    }
    
    if (key == GLUT_KEY_RIGHT) {
        blockX += stepSize;
    }
    
    if (blockX < -1.0f) {
        blockX = -1.0f;
    }
    
    if (blockX > (1.0f - blockSize * 2)) {
        blockX = 1.0f - blockSize * 2;
    }
    
    if (blockY < -1.0f + blockSize * 2) {
        blockY = -1.0f + blockSize * 2;
    }
    
    // 当正方形移动到最上面的时候
    if (blockY > 1.0f) {
        blockY = 1.0f;
    }
    
    vVerts[0] = blockX;
    vVerts[1] = blockY - blockSize * 2;
    
    vVerts[3] = blockX + blockSize * 2;
    vVerts[4] = blockY - blockSize * 2;
    
    vVerts[6] = blockX + blockSize * 2;
    vVerts[7] = blockY;
    
    vVerts[9] = blockX;
    vVerts[10] = blockY;
    
    triangleBatch.CopyVertexData3f(vVerts);
    glutPostRedisplay();
}

void setupRC() {
    glClearColor(0.65f, 0.65f, 0.65f, 1);
    
    shaderManager.InitializeStockShaders();
    
    triangleBatch.Begin(GL_TRIANGLE_FAN, 4);
    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.End();
}

int main(int argc,char *argv[]) {
    gltSetWorkingDirectory(argv[0]);
    // 初始化GLUT库
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("chengqian");
    
    glutReshapeFunc(changeSize);
    
    glutDisplayFunc(RenderScene);
    
    glutSpecialFunc(SpecialKeys);
    
    GLenum status = glewInit();
    if (GLEW_OK != status) {
        return 1;
    }
    
    setupRC();
    glutMainLoop();
    
    return 0;
}
