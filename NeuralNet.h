#ifndef _NEURALNET_H_
#define _NEURALNET_H_

#include <vector>
#include <string>

class NeuralNet {
public:
    // 构造函数：指定输入节点数、隐藏节点数、输出节点数
    NeuralNet(int inputSize, int hiddenSize, int outputSize);

    // 前向传播：给输入，算输出 (0.0 ~ 1.0)
    double Forward(const std::vector<double>& inputs);

    // 反向传播训练：告诉它正确答案(target)，它自动调整权重
    void Train(const std::vector<double>& inputs, double target);

    // 文件操作
    void Save(std::string filename);
    void Load(std::string filename);

private:
    int m_iInputSize;
    int m_iHiddenSize;
    int m_iOutputSize;

    // 权重矩阵
    // weights1: 输入层 -> 隐藏层
    std::vector<std::vector<double>> m_weights1;
    // weights2: 隐藏层 -> 输出层
    std::vector<std::vector<double>> m_weights2;

    // 神经元的值 (用于反向传播计算)
    std::vector<double> m_hiddenLayer;
    std::vector<double> m_outputLayer;

    // 学习率 (控制学得有多快)
    double m_dLearningRate;

    // 激活函数 Sigmoid
    double Sigmoid(double x);
    // Sigmoid 的导数 (用于反向传播)
    double SigmoidDerivative(double x);
    
    // 初始化权重
    void InitWeights();
};

#endif