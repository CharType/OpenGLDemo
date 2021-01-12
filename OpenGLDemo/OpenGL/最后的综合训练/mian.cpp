//
//  mian.cpp
//  最后的综合训练
//
//  Created by 程倩的MacBook on 2020/12/13.
//

#include <stdio.h>
#include "GLTools.h"
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>

#include <GLUT/glut.h>
//#ifdef __APPLE__
//#include <GLUT/glut.h>
//#else
//#define FREEGLUT_STATIC
//#include <GL/glut.h>
//#endif

// 着色管理器
GLShaderManager shaderManager;
// 模型视图矩阵
GLMatrixStack modelViewMatrix;
// 投影矩阵
GLMatrixStack projectionMatrix;
// 视景体
GLFrustum viewFrustum;
// 几何图形变换管道
GLGeometryTransform transformPipeline;

// 大球
GLTriangleBatch torusBatch;
// 小球
GLTriangleBatch sphereBatch;
// 地板
GLBatch flootBatch;

// 观察者视角
GLFrame cameraFrame;
// 物体视角
GLFrame objectFrame;

// 小球数量
#define NUM_SPHERES 50
// 保存小球顶点数据
GLFrame spheres[NUM_SPHERES];

void SetupRC() {
    // 清理颜色
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // 初始化着色管理器
    shaderManager.InitializeStockShaders();
    // 物体往后移动5的位置
    objectFrame.MoveForward(5.0f);
    
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    // 设置地板顶点数据
    flootBatch.Begin(GL_LINES, 324);
    for (GLfloat x = -20.0f; x <= 20.0f; x+=0.5) {
        flootBatch.Vertex3f(x, -0.55f, 20.0f);
        flootBatch.Vertex3f(x, -0.55f, -20.0f);
        flootBatch.Vertex3f(20.0f, -0.55f, x);
        flootBatch.Vertex3f(-20.0f, -0.55f, x);
    }
    flootBatch.End();
    
    // 设置大球模型
    gltMakeSphere(torusBatch, 0.4f, 40, 80);
    
    // 设置小球模型
    gltMakeSphere(sphereBatch, 0.1f, 26, 13);
    
    // 随机位置放置小球
    for (int i = 0;i < NUM_SPHERES; i++) {
        GLfloat x = ((GLfloat)((rand() % 400) - 200) * 0.1f);
        GLfloat z = ((GLfloat)((rand() % 400) - 200) * 0.1f);
        spheres[i].SetOrigin(x, 0.0f, z);
    }
}

void RenderScene(void) {
    // 地板颜色值 绿
    static GLfloat vFloorColor[] = {0.0f, 1.0f, 0.0f,1.0f};
    // 大球颜色值 红
    static GLfloat vTorusColor[] = {1.0f,0.0f,0.0f,1.0f};
    // 小球颜色值 蓝
    static GLfloat vSphereColor[] = {0.0f,0.0f,1.0f,1.0f};
    
    // 基于时间动画
    static CStopWatch rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;
    
    // 清楚颜色缓冲区和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // push 视图矩阵
    modelViewMatrix.PushMatrix(objectFrame);
    
    // 获取光源位置
    M3DVector4f vLightPos = {0.0f,10.f,5.0f,1.0f};
    
    // 绘制地面
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vFloorColor);
    flootBatch.Draw();
    
    // 使大球位置平移3.0向屏幕里面
    modelViewMatrix.Translate(0.0f, 0.0f, 3.0f);
    // 复制栈顶
    modelViewMatrix.PushMatrix();
    // 大球自转
    modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
    // 指定适合的着色器
    shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(),vLightPos,vTorusColor);
    
    torusBatch.Draw();
    // 绘制完毕需要pop
    modelViewMatrix.PopMatrix();
    
    // 画小球
    for (int i = 0;i < NUM_SPHERES;i++) {

        modelViewMatrix.PushMatrix();
        modelViewMatrix.MultMatrix(spheres[i]);
        shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vLightPos, vSphereColor);
        sphereBatch.Draw();
        modelViewMatrix.PopMatrix();
    }
    
    // 让一个小球围绕大球公转
    modelViewMatrix.Rotate(yRot * 0.6f, 0.0f, 1.0f, 0.0f);
    modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vSphereColor);
    sphereBatch.Draw();
    modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

void ChangeSize(int nWidth, int nHeight) {
    // 1.设置视口
    glViewport(0, 0, nWidth, nHeight);
    
    // 创建投影矩阵
    viewFrustum.SetPerspective(35.0f, float(nWidth) / float(nHeight), 1.0f, 100.0f);
    // 将投影矩阵添加到投影矩阵堆栈上
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    // 设置变换管道和使用的两个矩阵堆栈（变化矩阵modelViewMatrix, 头晕矩阵projectMatrix）
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void SpeacialKeys(int key, int x, int y) {
    // 移动步长
    float linear = 0.1f;
    // 旋转度数
    float angular = float(m3dDegToRad(5.0f));
    if (key == GLUT_KEY_UP) {
        objectFrame.MoveForward(linear);
    } else if (key == GLUT_KEY_DOWN) {
        objectFrame.MoveForward(-linear);
    } else if (key == GLUT_KEY_LEFT) {
        objectFrame.RotateWorld(angular, 0,1,0);
    } else if (key == GLUT_KEY_RIGHT) {
        objectFrame.RotateWorld(-angular, 0, 1, 0);
    }
}

int main(int argc, char * argv[]) {
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1800, 1600);
    
    glutCreateWindow("OpenGL最后的综合训练");
    
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpeacialKeys);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error:%s \n", glewGetErrorString(err));
        return 1;
    }
    
    SetupRC();
    glutMainLoop();
    
    return 0;
}
