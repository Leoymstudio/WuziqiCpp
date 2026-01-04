#include "NeuralNet.h"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;

NeuralNet::NeuralNet(int input, int hidden, int output) 
    : m_iInputSize(input), m_iHiddenSize(hidden), m_iOutputSize(output) {
    
    m_dLearningRate = 0.05; // 学习率
    
    // 调整 vector 大小
    m_hiddenLayer.resize(hidden);
    m_outputLayer.resize(output);
    
    m_weights1.resize(input, vector<double>(hidden));
    m_weights2.resize(hidden, vector<double>(output));

    InitWeights();
}

void NeuralNet::InitWeights() {
    // 随机初始化 (-0.5 到 0.5)
    // 稍微缩小一点范围，防止初始输出过饱和
    for(int i=0; i<m_iInputSize; i++) {
        for(int j=0; j<m_iHiddenSize; j++) {
            m_weights1[i][j] = ((double)rand() / RAND_MAX) - 0.5;
        }
    }
    for(int i=0; i<m_iHiddenSize; i++) {
        for(int j=0; j<m_iOutputSize; j++) {
            m_weights2[i][j] = ((double)rand() / RAND_MAX) - 0.5;
        }
    }
}

double NeuralNet::Sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

double NeuralNet::SigmoidDerivative(double x) {
    return x * (1.0 - x); // x 必须是已经经过 Sigmoid 的输出值
}

double NeuralNet::Forward(const vector<double>& inputs) {
    // 1. 输入层 -> 隐藏层
    for(int j=0; j<m_iHiddenSize; j++) {
        double sum = 0.0;
        for(int i=0; i<m_iInputSize; i++) {
            sum += inputs[i] * m_weights1[i][j];
        }
        m_hiddenLayer[j] = Sigmoid(sum);
    }

    // 2. 隐藏层 -> 输出层
    for(int k=0; k<m_iOutputSize; k++) {
        double sum = 0.0;
        for(int j=0; j<m_iHiddenSize; j++) {
            sum += m_hiddenLayer[j] * m_weights2[j][k];
        }
        m_outputLayer[k] = Sigmoid(sum);
    }

    // 我们只需要一个输出 (胜率)
    return m_outputLayer[0];
}

void NeuralNet::Train(const vector<double>& inputs, double target) {
    // 先跑一遍前向，算当前值
    double output = Forward(inputs);

    // --- 反向传播 (Backpropagation) ---
    
    // 1. 计算输出层误差
    // Error = (Target - Output) * Derivative
    double outputError = (target - output) * SigmoidDerivative(output);

    // 2. 计算隐藏层误差
    vector<double> hiddenErrors(m_iHiddenSize);
    for(int j=0; j<m_iHiddenSize; j++) {
        double error = 0.0;
        for(int k=0; k<m_iOutputSize; k++) {
            error += outputError * m_weights2[j][k];
        }
        hiddenErrors[j] = error * SigmoidDerivative(m_hiddenLayer[j]);
    }

    // 3. 更新 隐藏层->输出层 的权重
    for(int j=0; j<m_iHiddenSize; j++) {
        for(int k=0; k<m_iOutputSize; k++) {
            m_weights2[j][k] += m_dLearningRate * outputError * m_hiddenLayer[j];
        }
    }

    // 4. 更新 输入层->隐藏层 的权重
    for(int i=0; i<m_iInputSize; i++) {
        for(int j=0; j<m_iHiddenSize; j++) {
            m_weights1[i][j] += m_dLearningRate * hiddenErrors[j] * inputs[i];
        }
    }
}

void NeuralNet::Save(string filename) {
    ofstream file(filename);
    if(file.is_open()) {
        // 保存 weights1
        for(auto& row : m_weights1) {
            for(double w : row) file << w << " ";
            file << endl;
        }
        // 保存 weights2
        for(auto& row : m_weights2) {
            for(double w : row) file << w << " ";
            file << endl;
        }
        file.close();
        cout << "[NN] 模型已保存至 " << filename << endl;
    }
}

void NeuralNet::Load(string filename) {
    ifstream file(filename);
    if(file.is_open()) {
        for(int i=0; i<m_iInputSize; i++) {
            for(int j=0; j<m_iHiddenSize; j++) {
                file >> m_weights1[i][j];
            }
        }
        for(int i=0; i<m_iHiddenSize; i++) {
            for(int j=0; j<m_iOutputSize; j++) {
                file >> m_weights2[i][j];
            }
        }
        file.close();
        cout << "[NN] 模型加载成功！" << endl;
    } else {
        cout << "[NN] 未找到旧模型，初始化随机大脑。" << endl;
    }
}