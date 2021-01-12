//
//  main.cpp
//  金字塔
//
//  Created by 程倩的MacBook on 2021/1/7.
//

#include <stdio.h>
#include "GLTools.h"
#include "GLShaderManager.h"
#include "GLFrame.h"
#include "GLBatch.h"
#include "GLFrustum.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLShaderManager shaderManager;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLFrame cameraFrame;
GLFrame objectFrame;
GLFrustum viewFrustum;
GLBatch pyramidBatch;
// 纹理id
GLuint textureId;

GLGeometryTransform transformPipeline;
M3DMatrix44f shadowMatrix;

void MakePryamid(GLBatch &pyramidBatch) {
    
    /**
     GL_TRIANGLES 三角形
     顶点数
     纹理数
     */
    pyramidBatch.Begin(GL_TRIANGLES, 18, 1);
    // 塔顶
    M3DVector3f vApex = {0.0f, 1.0f, 0.0f};
    // 前左
    M3DVector3f vFrontLeft = {-1.0f, -1.0f, 1.0f};
    // 前右
    M3DVector3f vFrontRight = {1.0f, -1.0f,1.0f};
    // 后左
    M3DVector3f vBackLeft = {-1.0f,-1.0f,-1.0f};
    // 后右
    M3DVector3f vBackRight = {1.0f,-1.0f,-1.0f};
    M3DVector3f n;
    
    m3dFindNormal(n, vBackLeft, vBackRight, vFrontRight);
    // 金字塔底部 （底部2个三角形）
    //vBackLeft
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    //vBackRight
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    //vFrontRight
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontRight);

    m3dFindNormal(n, vFrontLeft, vBackLeft, vFrontRight);
    
    //VFrontLeft
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    //VBackLeft
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    //vFontRight
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    // 金字塔前面 (-个三角形)
    m3dFindNormal(n, vApex, vBackLeft, vFrontRight);
    //vApex
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    //vFrontLeft
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    //vFontRight
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    // 金字塔左边
    // 三角形（vApex,vBackLeft,VFrontLeft）
    m3dFindNormal(n, vApex, vBackLeft, vFrontLeft);
    // vApex
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    //vBackLeft
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    //VFrontLeft
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontLeft);
    
    // 金字塔右边
    // 三角形（vApex,vFrontRight,vBackRight);
    m3dFindNormal(n, vApex, vFrontRight, vBackRight);
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vFrontRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    // 金字塔后面
    // 三角形(vApex,vBackRight,vBackLeft)
    m3dFindNormal(n, vApex, vBackRight, vBackLeft);
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.5f, 1.0f);
    pyramidBatch.Vertex3fv(vApex);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackRight);
    
    pyramidBatch.Normal3fv(n);
    pyramidBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
    pyramidBatch.Vertex3fv(vBackLeft);
    
    
    pyramidBatch.End();
}

bool loadTGATexture(const char *sizeFileName, GLenum minFilter, GLenum magFilter,GLenum wrapModel) {
    GLbyte *pBits;
    int nWidth,nHeight,nComponents;
    GLenum eFormat;
    
    // 读取纹理，读取像素
    /**
     sizeFileName 纹理文件名称
     
     */
    pBits = gltReadTGABits(sizeFileName, &nWidth, &nHeight, &nComponents, &eFormat);
    if (pBits == NULL) {
        return false;
    }
    
    // 设置纹理参数
    // 参数一 纹理维度
    // 参数二 为S/T坐标设置模式
    // 蚕食三 wrapModel 环绕模式
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapModel);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapModel);
    
    // 参数一 纹理维度
    // 参数二 线性过滤
    // 参数三
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    // 载入纹理
    glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBits);
    
    free(pBits);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    return true;
}

void SetupRC() {
    
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    shaderManager.InitializeStockShaders();
    
    glEnable(GL_DEPTH_TEST);
    // 分配纹理对象
    // 参数1 纹理对象个数
    // 参数2 纹理对象指针
    glGenTextures(1,&textureId);
    // 绑定纹理状态
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // 将TGA文件加载为2D纹理
    // 参数1 纹理文件名称
    // 参数2 参数3 要缩小或者放大的过滤器
    // 参数4： 纹理左边环绕样式
    loadTGATexture("ceiling.tga", GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR, GL_CLAMP_TO_EDGE);
    
    // 创造金字塔
    MakePryamid(pyramidBatch);
    // 相机移动
    cameraFrame.MoveForward(-10);
    
}

void ShutDownRc(void) {
    glDeleteTextures(1, &textureId);
}

void RenderScene(void) {
    //1.  颜色值
    static GLfloat vLightPos [] = {1.0f,1.0f,0.0f};
    //2.  光源位置
    static GLfloat vWhite [] = {1.0f,1.0f,1.0f,1.0f};
    
    //3.  清理缓存区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // 当前模型视频压栈
    modelViewMatrix.PushMatrix();
    // 添加照相机矩阵
    M3DMatrix44f mCamera;
    // 从camraFrame中取出 4 * 4的矩阵
    cameraFrame.GetCameraMatrix(mCamera);
    // 矩阵乘以矩阵堆栈顶部的矩阵，结果存储到栈顶的顶部
    modelViewMatrix.MultMatrix(mCamera);
    
    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    modelViewMatrix.MultMatrix(mObjectFrame);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, textureId);
    // 点光源着色器
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(), vLightPos, vWhite, 0);
    // 绘制
    pyramidBatch.Draw();
    // 模型视图出栈，恢复矩阵（push一次就要pop一次）
    modelViewMatrix.PopMatrix();
    // 交换缓冲区
    glutSwapBuffers();
}

void SpecialKeys(int key, int x,int y) {
    if (key == GLUT_KEY_UP) {
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    }
    
    if (key == GLUT_KEY_DOWN) {
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    }
    
    if (key == GLUT_KEY_LEFT) {
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    }
    
    if (key == GLUT_KEY_RIGHT) {
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    
    glutPostRedisplay();
}

void ChangeSize(int w, int h) {
    // 1 设置视口
    glViewport(0, 0, w, h);
    // 2 创建投影矩阵
    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 500.0f);
    // 添加到投影矩阵上
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    // 设置变换管道和使用的两个矩阵
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

int main(int argc, char * argv[]) {
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Pyramid");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s \n", glewGetErrorString(err));
        return 1;
    }
    
    SetupRC();
    glutMainLoop();
    ShutDownRc();
    
    return 0;
}
