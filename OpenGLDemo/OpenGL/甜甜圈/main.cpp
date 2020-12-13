//
//  main.cpp
//  甜甜圈
//
//  Created by 程倩的MacBook on 2020/12/1.
//

#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLFrame viewFrame;
GLFrustum viewFrustum;
GLTriangleBatch torusBatch;
GLMatrixStack modelViewMatix;
GLMatrixStack projectionMatix;
GLGeometryTransform transformPipeline;
GLShaderManager shaderManager;

// 标记：背面剔除
int isCull = 0;
// 深度测试
int isDepth = 0;

void RenderScene() {
    // 清楚窗口和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (isCull) {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }
    
    if (isDepth) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    // 把摄像机矩阵压入模型矩阵中
    modelViewMatix.PushMatrix(viewFrame);
    
    // 设置绘图颜色
    GLfloat vRed[] = {1.0f, 0.0f, 0.0f, 1.0f};
    
    // 使用默认光源着色器
    /**
     参数：
     GLT_SHADER_DEFAULT_LIGHT 默认光源着色器
     GetModelViewMatrix ： 模型视图矩阵
     GetProjectionMatrix ： 投影矩阵
     颜色
     */
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vRed);
    
    // 绘制
    torusBatch.Draw();
    
    // 出栈，绘制完成后恢复
    modelViewMatix.PopMatrix();
    
    // 交换缓冲区
    glutSwapBuffers();
}

void setUpRC () {
    // 设置背景颜色
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    // 初始化着色管理器
    shaderManager.InitializeStockShaders();
    // 将相机往后移动7个单元：肉眼到物体之间的距离
    viewFrame.MoveForward(7.0);
    
    // 创建一个甜甜圈
    /*
     参数
     参数1：GLTriangleBatch ： 容器帮助类
     参数2：内边缘半径
     参数3：外边缘半径
     参数4：5 主半径和从半径的细分单元数量
     
     */
    
//    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 100, 26);
    
    // 点的大小
    glPointSize(4.0f);
}

void SpecialKeys (int key, int x,int y) {
    // 判断方向
    if (key == GLUT_KEY_UP) {
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
    } else if (key == GLUT_KEY_DOWN) {
        viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
    } else if (key == GLUT_KEY_LEFT) {
        viewFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    } else if (key == GLUT_KEY_RIGHT) {
        viewFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    }
    glutPostRedisplay();
}

void ChangeSize(int w,int h) {
    if (h == 0) {
        h = 1;
    }
    // 设置窗口尺寸
    glViewport(0, 0, w, h);
    // 设置通透模式。初始化透视矩阵
    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 100.0f);
    
    projectionMatix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    transformPipeline.SetMatrixStacks(modelViewMatix, projectionMatix);
}

void ProcessMenu(int value) {
    switch (value) {
        case 1:
            isDepth = !isDepth;
            break;
        case 2:
            isCull = !isCull;
            break;
        case 3:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case 4:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case 5:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

int main(int argc,char * argv[]) {
    
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(1000, 1000);
    glutCreateWindow("甜甜圈");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    
    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("深度测试", 1);
    glutAddMenuEntry("正背面剔除", 2);
    glutAddMenuEntry("纯色填充", 3);
    glutAddMenuEntry("线填充", 4);
    glutAddMenuEntry("点填充", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW ERROR: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    setUpRC();
    
    glutMainLoop();
    
    return 0;
}
