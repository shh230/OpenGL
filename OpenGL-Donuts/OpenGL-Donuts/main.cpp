//
//  main.cpp
//  OpenGL环境搭建
//
//  Created by 司航航 on 2020/7/11.
//  Copyright © 2020 网易(杭州)有限公司. All rights reserved.
//

#include "GLTools.h"
// GLMatrixStack.h 要放在 GLGeometryTransform.h 上面，不然会报错
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

// 设置角色帧
GLFrame             viewFrame;
// 观察者
GLFrame             cameraFrame;
// 使用 GLFrustum 类来设置透视投影
GLFrustum           viewFrustum;
// 三角形批次类（圆环）
GLTriangleBatch     torusBatch;
// GLMatrixStack 矩阵堆栈类
// 模型视图距证
GLMatrixStack       modelViewMatix;
// 投影矩阵
GLMatrixStack       projectionMatrix;
// 变换管线（管理对阵矩阵）
GLGeometryTransform transformPipeline;
GLShaderManager shaderManager;

// 标记: 背面剔除
int iCull = 0;
// 标记：深度测试
int iDepth = 0;


/// 窗口大小改变时接受新的宽度和高度，其中0,0代表窗口中视口的左下角坐标，w，h代表像素
/// @param w 视口的宽
/// @param h 视口的高
void ChangeSize(int w,int h) {
    
    // 防止高度为0
    if (h == 0) {
        h = 1;
    }
    glViewport(0,0, w, h);
    
    // 设置透视模式，初始化透视矩阵
    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 100.0f);
    
    // 把透视矩阵加载到投影矩阵堆栈中
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    // 初始化渲染管线
    transformPipeline.SetMatrixStacks(modelViewMatix, projectionMatrix);
    
}


/// 键位设置，通过不同的键位对其进行设置
/// 控制Camera的移动，从而改变视口
void SpecialKeys(int key, int x, int y) {
    
    if (key == GLUT_KEY_UP) {
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
    }
    
    if (key == GLUT_KEY_DOWN) {
        viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
    }
    
    if (key == GLUT_KEY_LEFT) {
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
    }
    
    if (key == GLUT_KEY_RIGHT) {
        viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
    }
    
    glutPostRedisplay();
}


/// 为程序作一次性的设置
void SetupRC() {
    
    // 设置背景颜色
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    // 初始化着色器管理器
    shaderManager.InitializeStockShaders();
    // 设置观察者
    viewFrame.MoveForward(10);
    
    /**
     * 创建一个甜甜圈
     * 参数一：GLTriangleBatch 容器帮助类
     * 参数二：外边缘半径
     * 参数三：内边缘本经
     * 参数四：主半径细分单元数量
     * 参数五：从半径细分单元数量
     */
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
    
    // 点的大小（方便点填充时，肉眼观察）
    glPointSize(4.0f);
    
}

//开始渲染
void RenderScene(void) {
    
    // 设置绘图颜色
    GLfloat vRed[] = {1.0f, 0.0f, 0.0f, 1.0f};
    
    // 清除窗口和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (iCull) {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }
    
    if (iDepth) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    
    glDepthMask(GL_FALSE);
    // 观察者矩阵压入模型矩阵中
    modelViewMatix.PushMatrix(viewFrame);
    
    /**
     * 使用默认光源着色器
     * 通过光源、阴影效果体现立体想过
     *
     * 参数一：GLT_SHADER_DEFAULT_LIGHT 默认光源着色器
     * 参数二：模型视图矩阵
     * 参数三：投影矩阵
     * 参数四：基本颜色
     */
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vRed);
    
    // 绘制
    torusBatch.Draw();
    
    // 出栈，绘制完成恢复
    modelViewMatix.PopMatrix();
    
    // 交换缓冲区
    glutSwapBuffers();
}

void ProcessMenu(int value) {
    
    switch (value) {
        case 1:
            iDepth = !iDepth;
            break;
            
        case 2:
            iCull = !iCull;
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


int main(int argc, char * argv[]) {
    // insert code here...
    
    // 设置当前工作目录，针对MAC OS X
    gltSetWorkingDirectory(argv[0]);

    // 初始化GLUT库
    glutInit(&argc, argv);
    
    /* 初始化双缓冲窗口，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
     双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区*/
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    // GLUT窗口大小，标题窗口
    glutInitWindowSize(800,600);
    // Window的标题
    glutCreateWindow("Donuts");
    // 注册改变窗口大小的回调函数
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    // 注册显示的回调函数
    glutDisplayFunc(RenderScene);
    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("Toggle depth test", 1);
    glutAddMenuEntry("Toggle cull backface", 2);
    glutAddMenuEntry("Set Fill Mode", 3);
    glutAddMenuEntry("Set Line Mode", 4);
    glutAddMenuEntry("Set Point Mode", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    //驱动程序的初始化中没有出现任何问题。
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        return 1;
    }
    
    //调用SetupRC
    SetupRC();
    glutMainLoop();
    
    return 0;
}
