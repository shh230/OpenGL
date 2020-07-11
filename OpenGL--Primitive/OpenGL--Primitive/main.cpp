//
//  main.cpp
//  OpenGL--Primitive
//
//  Created by 司航航 on 2020/7/11.
//  Copyright © 2020 网易(杭州)有限公司. All rights reserved.
//

#include <iostream>
#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

/*
 GLMatrixStack 变化管线使用矩阵堆栈
 
 GLMatrixStack 构造函数允许指定堆栈的最大深度、默认的堆栈深度为64.这个矩阵堆在初始化时已经在堆栈中包含了单位矩阵。
 GLMatrixStack::GLMatrixStack(int iStackDepth = 64);
 
 //通过调用顶部载入这个单位矩阵
 void GLMatrixStack::LoadIndentiy(void);
 
 //在堆栈顶部载入任何矩阵
 void GLMatrixStack::LoadMatrix(const M3DMatrix44f m);
 */
// 各种需要的类
GLShaderManager        shaderManager;
GLMatrixStack        modelViewMatrix; // 模型视图矩阵
GLMatrixStack        projectionMatrix; // 投影矩阵
GLFrame                cameraFrame;
GLFrame             objectFrame;
//投影矩阵
GLFrustum            viewFrustum;

//容器类（7种不同的图元对应7种容器对象）
GLBatch                pointBatch;
GLBatch                lineBatch;
GLBatch                lineStripBatch;
GLBatch                lineLoopBatch;
GLBatch                triangleBatch;
GLBatch             triangleStripBatch;
GLBatch             triangleFanBatch;

// 画笔颜色
GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };

//几何变换的管道
GLGeometryTransform    transformPipeline;

// 跟踪效果步骤，用于处理是哪一步
int nStep = 0;


/// 窗口大小改变时接受新的宽度和高度，其中0,0代表窗口中视口的左下角坐标，w，h代表像素
/// @param w 视口的宽
/// @param h 视口的高
void ChangeSize(int w,int h) {
    
    glViewport(0,0, w, h);
    // 创建投影矩阵(透视投影)，并将它载入投影矩阵堆栈中
    // 参数1，观察者的角度
    // 参数2，纵横比（宽/高）
    // 参数3，近距离
    // 参数3，远距离
    viewFrustum.SetPerspective(35.0f, float(w) / float(h), 1.0f, 500.f);
    // 在堆栈顶部载入任何矩阵
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    // 调用顶部载入单元矩阵
    modelViewMatrix.LoadIdentity();
    
}


/// 为程序作一次性的设置
void SetupRC() {
    
    // 灰色的背景
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f );
    shaderManager.InitializeStockShaders();
//    glEnable(GL_DEPTH_TEST);
    //设置变换管线以使用两个矩阵堆栈
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
    // 设置观察者
    cameraFrame.MoveForward(-15.0f);
    
    /*
     常见函数：
     void GLBatch::Begin(GLenum primitive,GLuint nVerts,GLuint nTextureUnits = 0);
      参数1：表示使用的图元
      参数2：顶点数
      参数3：纹理坐标（可选）
     
     //负责顶点坐标
     void GLBatch::CopyVertexData3f(GLFloat *vNorms);
     
     //结束，表示已经完成数据复制工作
     void GLBatch::End(void);
     
     
     */
    //定义一些点，三角形形状。
    
    GLfloat vCoast[9] = {
        3,3,0,
        0,3,0,
        3,0,0
    };
    
    // 用点的形式
    pointBatch.Begin(GL_POINTS, 3);
    pointBatch.CopyVertexData3f(vCoast);
    pointBatch.End();
    
    //用点的形式
    pointBatch.Begin(GL_POINTS, 3);
    pointBatch.CopyVertexData3f(vCoast);
    pointBatch.End();
    
    //通过线的形式
    lineBatch.Begin(GL_LINES, 3);
    lineBatch.CopyVertexData3f(vCoast);
    lineBatch.End();
    
    //通过线段的形式
    lineStripBatch.Begin(GL_LINE_STRIP, 3);
    lineStripBatch.CopyVertexData3f(vCoast);
    lineStripBatch.End();
    
    //通过线环的形式
    lineLoopBatch.Begin(GL_LINE_LOOP, 3);
    lineLoopBatch.CopyVertexData3f(vCoast);
    lineLoopBatch.End();
    
    //    通过三角形创建金字塔
    GLfloat vPyramid[12][3] = {
        -2.0f, 0.0f, -2.0f,
        2.0f, 0.0f, -2.0f,
        0.0f, 4.0f, 0.0f,

        2.0f, 0.0f, -2.0f,
        2.0f, 0.0f, 2.0f,
        0.0f, 4.0f, 0.0f,

        2.0f, 0.0f, 2.0f,
        -2.0f, 0.0f, 2.0f,
        0.0f, 4.0f, 0.0f,

        -2.0f, 0.0f, 2.0f,
        -2.0f, 0.0f, -2.0f,
        0.0f, 4.0f, 0.0f
        
    };
    
    //GL_TRIANGLES 每3个顶点定义一个新的三角形
    triangleBatch.Begin(GL_TRIANGLES, 12);
    triangleBatch.CopyVertexData3f(vPyramid);
    triangleBatch.End();
    
    // 三角形扇形--六边形
    GLfloat vPoints[100][3];
    int nVerts = 0;
    //半径
    GLfloat r = 3.0f;
    //原点(x,y,z) = (0,0,0);
    vPoints[nVerts][0] = 0.0f;
    vPoints[nVerts][1] = 0.0f;
    vPoints[nVerts][2] = 0.0f;
    
    //M3D_2PI 就是2Pi 的意思，就一个圆的意思。 绘制圆形
    for(GLfloat angle = 0; angle < M3D_2PI; angle += M3D_2PI / 6.0f) {
        
        //数组下标自增（每自增1次就表示一个顶点）
        nVerts++;
        /*
         弧长=半径*角度,这里的角度是弧度制,不是平时的角度制
         既然知道了cos值,那么角度=arccos,求一个反三角函数就行了
         */
        //x点坐标 cos(angle) * 半径
        vPoints[nVerts][0] = float(cos(angle)) * r;
        //y点坐标 sin(angle) * 半径
        vPoints[nVerts][1] = float(sin(angle)) * r;
        //z点的坐标
        vPoints[nVerts][2] = -0.5f;
    }
    
    // 结束扇形 前面一共绘制7个顶点（包括圆心）
    //添加闭合的终点
    //课程添加演示：屏蔽177-180行代码，并把绘制节点改为7.则三角形扇形是无法闭合的。
    nVerts++;
    vPoints[nVerts][0] = r;
    vPoints[nVerts][1] = 0;
    vPoints[nVerts][2] = 0.0f;
    
    // 加载！
    //GL_TRIANGLE_FAN 以一个圆心为中心呈扇形排列，共用相邻顶点的一组三角形
    triangleFanBatch.Begin(GL_TRIANGLE_FAN, 8);
    triangleFanBatch.CopyVertexData3f(vPoints);
    triangleFanBatch.End();
    
    //三角形条带，一个小环或圆柱段
    //顶点下标
    int iCounter = 0;
    //半径
    GLfloat radius = 3.0f;
    //从0度~360度，以0.3弧度为步长
    for(GLfloat angle = 0.0f; angle <= (2.0f*M3D_PI); angle += 0.3f)
    {
        //或许圆形的顶点的X,Y
        GLfloat x = radius * sin(angle);
        GLfloat y = radius * cos(angle);
        
        //绘制2个三角形（他们的x,y顶点一样，只是z点不一样）
        vPoints[iCounter][0] = x;
        vPoints[iCounter][1] = y;
        vPoints[iCounter][2] = -0.5;
        iCounter++;
        
        vPoints[iCounter][0] = x;
        vPoints[iCounter][1] = y;
        vPoints[iCounter][2] = 0.5;
        iCounter++;
    }
    
    // 关闭循环
    printf("三角形带的顶点数：%d\n",iCounter);
    //结束循环，在循环位置生成2个三角形
    vPoints[iCounter][0] = vPoints[0][0];
    vPoints[iCounter][1] = vPoints[0][1];
    vPoints[iCounter][2] = -0.5;
    iCounter++;
    
    vPoints[iCounter][0] = vPoints[1][0];
    vPoints[iCounter][1] = vPoints[1][1];
    vPoints[iCounter][2] = 0.5;
    iCounter++;
    
    // GL_TRIANGLE_STRIP 共用一个条带（strip）上的顶点的一组三角形
    triangleStripBatch.Begin(GL_TRIANGLE_STRIP, iCounter);
    triangleStripBatch.CopyVertexData3f(vPoints);
    triangleStripBatch.End();
    
}

void DrawWireFramedBatch(GLBatch* pBatch) {
    /*------------画绿色部分----------------*/
    /* GLShaderManager 中的Uniform 值——平面着色器
     参数1：平面着色器
     参数2：运行为几何图形变换指定一个 4 * 4变换矩阵
          --transformPipeline 变换管线（指定了2个矩阵堆栈）
     参数3：颜色值
    */
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vGreen);
    pBatch->Draw();
    
    /*-----------边框部分-------------------*/
    /*
        glEnable(GLenum mode); 用于启用各种功能。功能由参数决定
        参数列表：http://blog.csdn.net/augusdi/article/details/23747081
        注意：glEnable() 不能写在glBegin() 和 glEnd()中间
        GL_POLYGON_OFFSET_LINE  根据函数glPolygonOffset的设置，启用线的深度偏移
        GL_LINE_SMOOTH          执行后，过虑线点的锯齿
        GL_BLEND                启用颜色混合。例如实现半透明效果
        GL_DEPTH_TEST           启用深度测试 根据坐标的远近自动隐藏被遮住的图形（材料
     
     
        glDisable(GLenum mode); 用于关闭指定的功能 功能由参数决定
     
     */
    
    //画黑色边框
    glPolygonOffset(-1.0f, -1.0f);// 偏移深度，在同一位置要绘制填充和边线，会产生z冲突，所以要偏移
    glEnable(GL_POLYGON_OFFSET_LINE);
    
    // 画反锯齿，让黑边好看些
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //绘制线框几何黑色版 三种模式，实心，边框，点，可以作用在正面，背面，或者两面
    //通过调用glPolygonMode将多边形正面或者背面设为线框模式，实现线框渲染
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //设置线条宽度
    glLineWidth(2.5f);
    
    /* GLShaderManager 中的Uniform 值——平面着色器
     参数1：平面着色器
     参数2：运行为几何图形变换指定一个 4 * 4变换矩阵
         --transformPipeline.GetModelViewProjectionMatrix() 获取的
          GetMatrix函数就可以获得矩阵堆栈顶部的值
     参数3：颜色值（黑色）
     */
    
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    pBatch->Draw();

    // 复原原本的设置
    //通过调用glPolygonMode将多边形正面或者背面设为全部填充模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}

//开始渲染
void RenderScene(void) {
    
    //清除一个或一组特定的缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // 模型视图矩阵压栈
    modelViewMatrix.PushMatrix();
    
    // 创建观察者
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    
    // 矩阵乘以矩阵堆栈的顶部矩阵，相乘的结果随后存储在栈顶
    modelViewMatrix.MultMatrix(mCamera);
    
    // 物体视图矩阵
    M3DMatrix44f mObjectFrame;
    // 只要使用 GetMatrix 函数就可以获取矩阵堆栈顶部的值，这个函数可以进行2次重载。用来使用GLShaderManager 的使用。或者是获取顶部矩阵的顶点副本数据
    objectFrame.GetMatrix(mObjectFrame);
    // 矩阵乘以矩阵堆栈的顶部矩阵，相乘的结果随后简存储在堆栈的顶部
    modelViewMatrix.MultMatrix(mObjectFrame);
    
    /* GLShaderManager 中的Uniform 值——平面着色器
        参数1：平面着色器
        参数2：运行为几何图形变换指定一个 4 * 4变换矩阵
        --transformPipeline.GetModelViewProjectionMatrix() 获取的
        GetMatrix函数就可以获得矩阵堆栈顶部的值
        参数3：颜色值（黑色）
    */
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    
    switch(nStep) {
        case 0:
            //设置点的大小
            glPointSize(4.0f);
            pointBatch.Draw();
            glPointSize(1.0f);
            break;
            
        case 1:
            //设置线的宽度
            glLineWidth(2.0f);
            lineBatch.Draw();
            glLineWidth(1.0f);
            break;
            
        case 2:
            glLineWidth(2.0f);
            lineStripBatch.Draw();
            glLineWidth(1.0f);
            break;
            
        case 3:
            glLineWidth(2.0f);
            lineLoopBatch.Draw();
            glLineWidth(1.0f);
            break;
            
        case 4:
            DrawWireFramedBatch(&triangleBatch);
            break;
            
        case 5:
            DrawWireFramedBatch(&triangleStripBatch);
            break;
            
        case 6:
            DrawWireFramedBatch(&triangleFanBatch);
            break;
            
        default:
            break;
    }
    
    // 还原到以前的模型视图矩阵（单位矩阵）
    modelViewMatrix.PopMatrix();
    
    //将在后台缓冲区进行渲染，然后在结束时交换到前台
    glutSwapBuffers();
    
}


/// 根据空格次数。切换不同的“窗口名称”
/// @param key 按下键位
/// @param x
/// @param y
void KeyPressFunc(unsigned char key, int x, int y) {
    
    if (key == 32) {
        nStep++;
        
        // 最多6步，多了则重置
        if (nStep > 6) {
            nStep = 0;
        }
    }
    
    // 设置 Window 的标题
    switch (nStep) {
        case 0:
            glutSetWindowTitle("GL_POINTS");
            break;
        case 1:
            glutSetWindowTitle("GL_LINES");
            break;
        case 2:
            glutSetWindowTitle("GL_LINE_STRIP");
            break;
        case 3:
            glutSetWindowTitle("GL_LINE_LOOP");
            break;
        case 4:
            glutSetWindowTitle("GL_TRIANGLES");
            break;
        case 5:
            glutSetWindowTitle("GL_TRIANGLE_STRIP");
            break;
        case 6:
            glutSetWindowTitle("GL_TRIANGLE_FAN");
            break;
            
        default:
            break;
    }
    
    glutPostRedisplay();
}


/// 特殊键位处理（上、下、左、右移动）
/// @param key 按下键位
/// @param x
/// @param y
void SpecialKeys(int key, int x, int y) {
    
    if(key == GLUT_KEY_UP)
        //围绕一个指定的X,Y,Z轴旋转。
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_DOWN)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
    
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
    glutCreateWindow("GL_POINTS");
    // 注册改变窗口大小的回调函数
    glutReshapeFunc(ChangeSize);
    // 注册点击空格时，调用的函数
    glutKeyboardFunc(KeyPressFunc);
    // 注册特殊键位函数（上下左右）
    glutSpecialFunc(SpecialKeys);
    // 注册显示的回调函数
    glutDisplayFunc(RenderScene);
    
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

