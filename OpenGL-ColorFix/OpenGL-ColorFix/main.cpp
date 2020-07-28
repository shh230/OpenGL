//
//  main.cpp
//  OpenGL环境搭建
//
//  Created by 司航航 on 2020/7/11.
//  Copyright © 2020 网易(杭州)有限公司. All rights reserved.
//

#include "GLShaderManager.h"
#include "GLTools.h"
#include <glut/glut.h>

GLBatch squareBatch;
GLBatch greenBatch;
GLBatch redBatch;
GLBatch blueBatch;
GLBatch blackBatch;

GLShaderManager shaderManager;

GLfloat blockSize = 0.2f;
GLfloat vVerts[] = {
    -blockSize, -blockSize, 0.0f,
    blockSize, -blockSize, 0.0f,
    blockSize, blockSize, 0.0f,
    -blockSize, blockSize, 0.0f
};


/// 窗口大小改变时接受新的宽度和高度，其中0,0代表窗口中视口的左下角坐标，w，h代表像素
/// @param w 视口的宽
/// @param h 视口的高
void ChangeSize(int w,int h) {
    
    glViewport(0,0, w, h);
    
}


/// 为程序作一次性的设置
void SetupRC() {
    
    //设置背影颜色
    glClearColor(1.0f,1.0f,1.0f,1.0f);

    //初始化着色管理器
    shaderManager.InitializeStockShaders();
    
    // 绘制一个移动的矩形
    squareBatch.Begin(GL_TRIANGLE_FAN, 4);
    squareBatch.CopyVertexData3f(vVerts);
    squareBatch.End();
    
    GLfloat vBlock[] = {
        0.25f, 0.25f, 0.0f,
        0.75f, 0.25f, 0.0f,
        0.75f, 0.75f, 0.0f,
        0.25f, 0.75f, 0.0f
    };
       
   greenBatch.Begin(GL_TRIANGLE_FAN, 4);
   greenBatch.CopyVertexData3f(vBlock);
   greenBatch.End();
    
    GLfloat vBlock2[] = {
        -0.75f, 0.25f, 0.0f,
        -0.25f, 0.25f, 0.0f,
        -0.25f, 0.75f, 0.0f,
        -0.75f, 0.75f, 0.0f
    };
    
    redBatch.Begin(GL_TRIANGLE_FAN, 4);
    redBatch.CopyVertexData3f(vBlock2);
    redBatch.End();
    
    
    GLfloat vBlock3[] = {
        -0.75f, -0.75f, 0.0f,
        -0.25f, -0.75f, 0.0f,
        -0.25f, -0.25f, 0.0f,
        -0.75f, -0.25f, 0.0f
    };
    
    blueBatch.Begin(GL_TRIANGLE_FAN, 4);
    blueBatch.CopyVertexData3f(vBlock3);
    blueBatch.End();
    
    
    GLfloat vBlock4[] = {
        0.25f, -0.75f, 0.0f,
        0.75f, -0.75f, 0.0f,
        0.75f, -0.25f, 0.0f,
        0.25f, -0.25f, 0.0f
    };
    
    blackBatch.Begin(GL_TRIANGLE_FAN, 4);
    blackBatch.CopyVertexData3f(vBlock4);
    blackBatch.End();
    
}

//开始渲染

void RenderScene(void) {
    
    //清除一个或一组特定的缓冲区
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    //设置一组浮点数来表示红色
    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    GLfloat vRedHalfAlpa[] = { 1.0f, 0.0f, 0.0f, 0.5f };
    GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 0.5f };
    GLfloat vBlue[] = { 0.0f, 0.0f, 1.0f, 0.5f };
    GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 0.5f };
    
    // 开启混合
    glEnable(GL_BLEND);
    // 开启组合函数，计算混合颜色银子
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //传递到存储着色器，即GLT_SHADER_IDENTITY着色器，这个着色器只是使用指定颜色以默认笛卡尔坐标第在屏幕上渲染几何图形
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRed);
    //提交着色器
    squareBatch.Draw();
    
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vGreen);
    greenBatch.Draw();
    
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRedHalfAlpa);
    redBatch.Draw();
    
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vBlue);
    blueBatch.Draw();
    
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vBlack);
    blackBatch.Draw();
    
    glDisable(GL_BLEND);
    
    //将在后台缓冲区进行渲染，然后在结束时交换到前台
    glutSwapBuffers();
    
}

void SpecialKeys(int key, int x, int y) {
    
    GLfloat stepSize = 0.025f;
    
    GLfloat blockX = vVerts[0];
    GLfloat blockY = vVerts[7];
    
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
    
    // 边缘碰撞处理
    if (blockX < -1.0f) {
        blockX = -1.0f;
    }
    if (blockX > (1.0f - blockSize * 2)) {
        blockX = 1.0f - blockSize * 2;
    }
    if (blockY > 1.0f) {
        blockY = 1.0f;
    }
    if (blockY < -1.0f + (blockSize * 2)) {
        blockY = -1.0f + blockSize * 2;
    }
    
    vVerts[0] = blockX;
    vVerts[1] = blockY - blockSize * 2;
    
    vVerts[3] = blockX + blockSize * 2;
    vVerts[4] = blockY - blockSize * 2;
    
    vVerts[6] = blockX + blockSize * 2;
    vVerts[7] = blockY;
    
    vVerts[9] = blockX;
    vVerts[10] = blockY;
    
    squareBatch.CopyVertexData3f(vVerts);
    
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
    glutCreateWindow("矩形移动，观察颜色");
    // 注册改变窗口大小的回调函数
    glutReshapeFunc(ChangeSize);
    // 注册显示的回调函数
    glutDisplayFunc(RenderScene);
    glutSpecialFunc(SpecialKeys);
    
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
